#include "asset/asset.hpp"

#define TOBJ_ENABLE_MMAP
#define TOBJ_ENABLE_MULTITHREADING
#define TOBJ_ENABLE_SIMD
#include <tiny_obj_c.h>

#include "stbi.h"

#include "file_system/directory.hpp"
#include "file_system/file.hpp"
#include "debug/log.hpp"

namespace Flock::Asset {
  Maybe<AssetLoader> AssetLoader::create(const char *asset_directory) {
    return create(Filepath::from_c_str(asset_directory));
  }

  Maybe<AssetLoader> AssetLoader::create(const Filepath &asset_directory) {
    Debug::Logger *logger = Debug::Logger::get("Flock");

    if (!FileSystem::Directory::exists(asset_directory)) {
      logger->log_inf("Directory %s does not exist, creating directory", asset_directory.to_string().c_str());
      if (!FileSystem::Directory::create(asset_directory)) {
        logger->log_err("Failed to create asset directory %s", asset_directory.to_string().c_str());
        return {};
      }
    }

    AssetLoader loader = {};
    loader.asset_dir_  = asset_directory;
    auto files         = FileSystem::Directory::list_files(asset_directory, true).get();
    for (auto &filepath: files) {
      if (filepath.extension() == "png") {
        auto path = filepath.relative_to(asset_directory).get();
        loader.load<Image>(path);
      }

      if (filepath.extension() == "obj") {
        auto path = filepath.relative_to(asset_directory).get();
        loader.load<Mesh>(path);
      }
    }

    return loader;
  }

  AssetLoader::AssetLoader(AssetLoader &&other) noexcept {
    assets_           = std::move(other.assets_);
    asset_allocators_ = std::move(other.asset_allocators_);
    asset_dir_        = std::move(other.asset_dir_);
  }

  AssetLoader &AssetLoader::operator=(AssetLoader &&other) noexcept {
    if (this == &other) {
      return *this;
    }

    release();

    assets_           = std::move(other.assets_);
    asset_allocators_ = std::move(other.asset_allocators_);
    asset_dir_        = std::move(other.asset_dir_);

    return *this;
  }

  AssetLoader::~AssetLoader() {
    release();
  }

  void AssetLoader::release() {
    for (const auto &[id, asset]: assets_) {
      if (asset.type == AssetType::Image) {
        unload<Image>({.id = id});
      } else if (asset.type == AssetType::Mesh) {
        unload<Mesh>({.id = id});
      } else {
        ASSERT(false, "Unknown asset type");
      }
    }

    assets_.release();
    asset_allocators_.release();
    asset_dir_ = {};
  }

  void AssetLoader::set_allocator(const AssetType type, Allocator *allocator) {
    for (auto &[id, asset]: assets_) {
      if (asset.type != type) {
        continue;
      }

      asset.data = realloc_to(
        asset_allocators_[asset.type],
        allocator, asset.data,
        asset_type_size(asset.type),
        asset_type_size(asset.type),
        asset_type_align(asset.type)
      );

      ASSERT(asset.data, "Allocation failed");
    }

    asset_allocators_[type] = allocator;
  }

  Maybe<Mesh> AssetLoader::load_mesh_impl(const Filepath &asset_path) {
    Debug::Logger *logger = Debug::Logger::get("Flock");

    if (asset_path.extension() != "obj") {
      return {};
    }

    tobj_scene_f           scene;
    const tobj_load_config cfg  = tobj_default_config(); // triangulate = true
    tobj_diag              diag = {.on_message = nullptr};

    String path_string = asset_path.to_string();
    path_string.push('\0');

    Maybe<File> file = File::open(asset_path, FileMode::ReadOnly);
    if (!file) {
      logger->log_err("Failed to load mesh %s: Failed to open file", asset_path.to_string().c_str());
      return {};
    }

    usize size = file->size();
    auto  ptr  = static_cast<byte *>(alloc(nullptr, size));
    file->read_exact(ptr, size, 0);
    if (tobj_load_obj_from_memory_f(&scene, ptr, size, &cfg, &diag) != TOBJ_OK) {
      if (diag.err) {
        logger->log_err("Failed to load mesh %s: %s", asset_path.to_string().c_str(), diag.err);
      }

      return {};
    }

    Mesh result{};

    const tobj_mesh_f *mesh = &scene.shapes[0].mesh;
    for (size_t i = 0; i < mesh->num_indices; i++) {
      const auto [vertex_index, normal_index, tex_coord_index] = mesh->indices[i];

      const f32 x  = scene.attrib.vertices.ptr[3 * vertex_index + 0];
      const f32 y  = scene.attrib.vertices.ptr[3 * vertex_index + 1];
      const f32 z  = scene.attrib.vertices.ptr[3 * vertex_index + 2];
      const f32 u  = scene.attrib.texcoords.ptr[2 * tex_coord_index + 0];
      const f32 v  = scene.attrib.texcoords.ptr[2 * tex_coord_index + 1];
      const f32 nx = scene.attrib.normals.ptr[3 * normal_index + 0];
      const f32 ny = scene.attrib.normals.ptr[3 * normal_index + 1];
      const f32 nz = scene.attrib.normals.ptr[3 * normal_index + 2];

      result.vertices.push(Vector3f::xyz(x, y, z));
      result.normals.push(Vector3f::xyz(nx, ny, nz));
      result.tex_coords.push(Vector2f::xy(u, v));
      result.indices.push(i);
    }

    tobj_scene_free_f(&scene);
    tobj_diag_free(&diag, nullptr);
    Flock::release(nullptr, ptr, size);

    logger->log_inf("Loaded mesh %s", asset_path.to_string().c_str());

    return result;
  }

  Maybe<Image> AssetLoader::load_image_impl(const Filepath &asset_path) {
    Debug::Logger *logger = Debug::Logger::get("Flock");

    using FileSystem::File;
    Maybe<File> file = File::open(asset_path, FileMode::ReadOnly);
    if (!file) {
      return {};
    }

    const usize size = file->size();
    void *      ptr  = alloc(nullptr, size);
    if (!ptr) {
      logger->log_err("Failed to load image %s: Failed to allocate memory", asset_path.to_string().c_str());
      return {};
    }

    file->read_exact(ptr, size, 0);

    constexpr i32 N = 4;

    i32   x, y, channels;
    byte *data = stbi_load_from_memory(static_cast<const stbi_uc *>(ptr), size, &x, &y, &channels, N);
    if (!data) {
      return {};
    }

    Image result;
    result.width  = x;
    result.height = y;
    result.pixels.reserve(x * y);
    for (usize j = 0; j < static_cast<usize>(y); j++) {
      for (usize i = 0; i < static_cast<usize>(x); i++) {
        Color4u8 color{};
        color.r = data[(j * x + i) * N + 0];
        color.g = data[(j * x + i) * N + 1];
        color.b = data[(j * x + i) * N + 2];
        color.a = data[(j * x + i) * N + 3];

        result.pixels.push(color);
      }
    }

    stbi_image_free(data);
    Flock::release(nullptr, ptr, size);

    logger->log_inf("Loaded image %s", asset_path.to_string().c_str());
    return result;
  }
}
