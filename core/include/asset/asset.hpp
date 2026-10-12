#pragma once

#include "common.hpp"
#include "collect/map.hpp"
#include "collect/string.hpp"
#include "math/color.hpp"
#include "math/vector.hpp"

namespace Flock {
  namespace Asset {
    template<typename T>
    struct AssetHandle;
    struct ImageData;
    struct MeshData;
  }

  using Mesh  = Asset::AssetHandle<Asset::MeshData>;
  using Image = Asset::AssetHandle<Asset::ImageData>;
}

namespace Flock::Asset {
  using AssetID                          = u64;
  static constexpr AssetID INVALID_ASSET = INVALID_64;

  constexpr AssetID get_asset_id(const StringSlice filepath) {
    return hash(filepath);
  }

  struct FLK_API MeshData {
    Vec<Vector3f> vertices   = {};
    Vec<Vector2f> tex_coords = {};
    Vec<u32>      indices    = {};
  };

  struct FLK_API ImageData {
    Vec<Color4u8> pixels = {};
    u32           width  = 0;
    u32           height = 0;
  };

  template<typename T>
  struct AssetHandle {
    AssetID asset_id = INVALID_ASSET;
    String  filepath = {};

    [[nodiscard]] bool is_valid() const {
      return asset_id != INVALID_ASSET;
    }
  };

  template<typename T>
  AssetHandle<T> asset_path(const StringSlice filepath) {
    return AssetHandle<T>{
      .asset_id = get_asset_id(filepath),
      .filepath = filepath.to_string()
    };
  }

  class FLK_API AssetLoader {
    Map<AssetID, MeshData>  meshes_ = {};
    Map<AssetID, ImageData> images_ = {};

  public:
    Maybe<Mesh>  load_mesh(StringSlice filepath);
    Maybe<Image> load_image(StringSlice filepath);
  };
}
