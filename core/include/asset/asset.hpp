#pragma once

#include "common.hpp"
#include "collect/map.hpp"
#include "collect/string.hpp"
#include "math/color.hpp"
#include "math/vector.hpp"

namespace Flock::Asset {
    using AssetID                          = u64;
    static constexpr AssetID INVALID_ASSET = INVALID_64;

    constexpr AssetID get_asset_id(StringSlice filepath) {
        return hash(filepath);
    }

    struct FLK_API Mesh {
        Vec<Vector3f> vertices   = {};
        Vec<Vector2f> tex_coords = {};
        Vec<u32>      indices    = {};
    };

    struct FLK_API Image {
        Vec<Color4u8> pixels = {};
        u32           width  = 0;
        u32           height = 0;
    };

    template <typename T>
    struct AssetHandle {
        AssetID asset_id = INVALID_ASSET;
        String  filepath = {};

        bool is_valid() const {
            return asset_id != INVALID_ASSET;
        }
    };

    template <typename T>
    AssetHandle<T> asset_path(StringSlice filepath) {
        return AssetHandle<T>{
            .asset_id = get_asset_id(filepath),
            .filepath = filepath.to_string()
        };
    }

    class FLK_API AssetLoader {
        Map<AssetID, Mesh>  meshes_ = {};
        Map<AssetID, Image> images_ = {};

    public:
        Maybe<AssetHandle<Mesh>>  load_mesh(StringSlice filepath);
        Maybe<AssetHandle<Image>> load_image(StringSlice filepath);
    };
}
