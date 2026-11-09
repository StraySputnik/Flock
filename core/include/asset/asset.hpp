#pragma once

#include <cstring>

#include "common.hpp"
#include "collect/string.hpp"
#include "debug/log.hpp"
#include "file_system/path.hpp"
#include "graphics/graphics.hpp"
#include "math/vector.hpp"

namespace Flock::Asset {
  using AssetID                          = u64;
  static constexpr AssetID INVALID_ASSET = INVALID_64;

  constexpr AssetID get_asset_id(const char *filepath) {
    return hash(filepath, strlen(filepath));
  }

  constexpr AssetID get_asset_id(const Filepath &filepath) {
    return get_asset_id(filepath.to_string().c_str());
  }

  enum class AssetType : u8 {
    Mesh,
    Image,
    Pipeline,
    Material,
  };

  template<typename T>
  AssetType asset_type_of() {
    if constexpr (std::is_same_v<T, Mesh>) {
      return AssetType::Mesh;
    } else if constexpr (std::is_same_v<T, Image>) {
      return AssetType::Image;
    } else if constexpr (std::is_same_v<T, Pipeline>) {
      return AssetType::Pipeline;
    } else if constexpr (std::is_same_v<T, Material>) {
      return AssetType::Material;
    } else {
      static_assert(false);
    }

    UNREACHABLE();
  }

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

  class FLK_API AssetLoader {
    struct Asset {
      AssetType type = AssetType::Mesh;
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

    template<typename T>
    Maybe<AssetHandle<T>> load(const Filepath &asset_path) {
      Maybe<T> data = {};

      Filepath path = asset_dir_;
      path.join(asset_path);
      if constexpr (std::is_same_v<T, Mesh>) {
        data = std::move(load_mesh_impl(path));
      } else if constexpr (std::is_same_v<T, Image>) {
        data = std::move(load_image_impl(path));
      } else if constexpr (std::is_same_v<T, Pipeline>) {
        data = std::move(load_shader_impl(path));
      } else if constexpr (std::is_same_v<T, Material>) {
        data = std::move(load_material_impl(path));
      }

      if (!data) {
        return {};
      }

      void *data_ptr = alloc(asset_allocators_[asset_type_of<T>()], sizeof(T), alignof(T));
      ASSERT(data_ptr, "Allocation failed");
      new(data_ptr) T(data.move());

      const Asset asset{
        .type = asset_type_of<T>(),
        .data = data_ptr,
      };

      assets_[get_asset_id(asset_path)] = asset;
      return AssetHandle<T>::from_path(asset_path);
    }

    template<typename T>
    void unload(const AssetHandle<T> &asset) {
      if (!asset.is_valid()) {
        return;
      }

      const AssetID id = asset.id;
      if (assets_[id].type != asset_type_of<T>()) {
        return;
      }

      T *ptr = static_cast<T *>(assets_[id].data);
      ptr->~T();
      Flock::release(asset_allocators_[asset_type_of<T>()], ptr, sizeof(T));
      assets_.remove(id);

      LOG_INF("Unloaded asset with ID %llu", id);
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
      if (asset->type != asset_type_of<T>()) {
        return nullptr;
      }

      return static_cast<T *>(data);
    }

    template<typename T>
    void set_allocator(Allocator *allocator) {
      for (auto &[asset_id, asset]: assets_) {
        if (asset.type != asset_type_of<T>()) {
          continue;
        }

        asset.data = realloc_to(
          asset_allocators_[asset.type],
          allocator, asset.data,
          sizeof(T),
          sizeof(T),
          alignof(T)
        );

        ASSERT(asset.data, "Allocation failed");
      }

      asset_allocators_[asset_type_of<T>()] = allocator;
    }

  private:
    static Maybe<Mesh>     load_mesh_impl(const Filepath &asset_path);
    static Maybe<Image>    load_image_impl(const Filepath &asset_path);
    static Maybe<Pipeline> load_shader_impl(const Filepath &asset_path);
    static Maybe<Material> load_material_impl(const Filepath &asset_path);
  };
}

namespace Flock {
  using namespace Asset;
}
