#pragma once

#include <cstring>

#include "common.hpp"
#include "collect/string.hpp"
#include "debug/log.hpp"
#include "file_system/path.hpp"
#include "math/color.hpp"
#include "math/vector.hpp"
#include "memory/ref.hpp"

namespace Flock::Asset {
  using AssetID                          = u64;
  static constexpr AssetID INVALID_ASSET = INVALID_64;

  constexpr AssetID get_asset_id(const char *filepath) {
    return hash(filepath, strlen(filepath));
  }

  constexpr AssetID get_asset_id(const Filepath &filepath) {
    return get_asset_id(filepath.to_string().c_str());
  }

  struct FLK_API Mesh {
    Vec<Vector3f> vertices   = {};
    Vec<Vector3f> normals    = {};
    Vec<Vector2f> tex_coords = {};
    Vec<u32>      indices    = {};
  };

  struct FLK_API Image {
    Vec<Color4u8> pixels = {}; // Left-to-right Top-to-bottom
    u32           width  = 0;
    u32           height = 0;
  };

  template<typename T>
  struct AssetHandle {
    AssetID id = INVALID_ASSET;

    static constexpr AssetHandle from_path(const char *filepath) {
      return {
        .id = get_asset_id(filepath),
      };
    }

    static constexpr AssetHandle from_path(const Filepath &filepath) {
      return from_path(filepath.to_string().c_str());
    }

    [[nodiscard]] bool is_valid() const {
      return id != INVALID_ASSET;
    }
  };

  enum class AssetType : u8 {
    None,
    Mesh,
    Image,
  };

  template<typename T>
  constexpr AssetType asset_type() {
    if constexpr (std::is_same_v<T, Mesh>) {
      return AssetType::Mesh;
    }

    if constexpr (std::is_same_v<T, Image>) {
      return AssetType::Image;
    }

    return AssetType::None;
  }

  constexpr usize asset_type_size(const AssetType type) {
    switch (type) {
      case AssetType::Mesh:
        return sizeof(Mesh);
      case AssetType::Image:
        return sizeof(Image);
      default:
        PANIC();
    }

    UNREACHABLE();
  }

  constexpr usize asset_type_align(const AssetType type) {
    switch (type) {
      case AssetType::Mesh:
        return alignof(Mesh);
      case AssetType::Image:
        return alignof(Image);
      default:
        PANIC();
    }

    UNREACHABLE();
  }

  class FLK_API AssetLoader {
    struct Asset {
      AssetType type = AssetType::None;
      void *    data = nullptr;
    };

    Filepath                        asset_dir_        = {};
    HashMap<AssetID, Asset>         assets_           = {};
    HashMap<AssetType, Allocator *> asset_allocators_ = {};

  public:
    static Maybe<AssetLoader> create(const char *asset_directory);
    static Maybe<AssetLoader> create(const Filepath &asset_directory);

    AssetLoader() = default;

    AssetLoader(const AssetLoader &) = delete;
    AssetLoader(AssetLoader &&other) noexcept;

    AssetLoader &operator=(const AssetLoader &) = delete;
    AssetLoader &operator=(AssetLoader &&other) noexcept;

    ~AssetLoader();

    void release();
    void set_allocator(AssetType type, Allocator *allocator);

    template<typename T>
    Maybe<AssetHandle<T>> load(const Filepath &asset_path) {
      Maybe<T> data = {};

      Filepath path = asset_dir_;
      path.join(asset_path);
      if constexpr (std::is_same_v<T, Mesh>) {
        data = load_mesh_impl(path);
      } else if constexpr (std::is_same_v<T, Image>) {
        data = load_image_impl(path);
      }

      if (!data) {
        return {};
      }

      void *data_ptr = alloc(asset_allocators_[asset_type<T>()], sizeof(T), alignof(T));
      ASSERT(data_ptr, "Allocation failed");
      new(data_ptr) T(data.move());

      const Asset asset{
        .type = asset_type<T>(),
        .data = data_ptr,
      };

      assets_[get_asset_id(asset_path)] = asset;
      return AssetHandle<T>::from_path(asset_path);
    }

    template<typename T>
    void unload(const AssetHandle<T> &asset) {
      Debug::Logger *logger = Debug::Logger::get("Flock");

      if (!asset.is_valid()) {
        return;
      }

      const AssetID id = asset.id;
      if (assets_[id].type != asset_type<T>()) {
        return;
      }

      T *ptr = static_cast<T *>(assets_[id].data);
      ptr->~T();
      Flock::release(asset_allocators_[asset_type<T>()], ptr, asset_type_size(asset_type<T>()));
      assets_.remove(id);

      logger->log_inf("Unloaded asset with ID %llu", id);
    }

    template<typename T>
    T *resolve(AssetHandle<T> handle) {
      if (!handle.is_valid()) {
        return nullptr;
      }

      Asset *asset = assets_.get(handle.id);
      if (!asset) {
        // TODO: Search for asset in asset directory and load it
        return nullptr;
      }

      void *data = asset->data;
      if (asset->type != asset_type<T>()) {
        return nullptr;
      }

      return static_cast<T *>(data);
    }

  private:
    static Maybe<Mesh>  load_mesh_impl(const Filepath &asset_path);
    static Maybe<Image> load_image_impl(const Filepath &asset_path);
  };
}

namespace Flock {
  using namespace Asset;
}
