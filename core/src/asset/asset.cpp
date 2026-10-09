#include "asset/asset.hpp"

#define TOBJ_ENABLE_MMAP
#define TOBJ_ENABLE_MULTITHREADING
#define TOBJ_ENABLE_SIMD
#include <tiny_obj_c.h>

namespace Flock::Asset {
    static Maybe<Mesh> load_obj(StringSlice filepath) {
        tobj_scene_f           scene;
        const tobj_load_config cfg  = tobj_default_config(); // triangulate = true
        tobj_diag              diag = {.on_message = nullptr};

        if (tobj_load_obj_from_file_f(&scene, filepath.to_string().push('\0').first(), &cfg, &diag) != TOBJ_OK) {
            if (diag.err) {
                fprintf(stderr, "%s\n", diag.err);
            }

            return {};
        }

        Mesh result{};

        const tobj_mesh_f *mesh = &scene.shapes[0].mesh;
        for (size_t i = 0; i < mesh->num_indices; i++) {
            const tobj_index idx = mesh->indices[i];
            const f32        x   = scene.attrib.vertices.ptr[3 * idx.vertex_index + 0];
            const f32        y   = scene.attrib.vertices.ptr[3 * idx.vertex_index + 1];
            const f32        z   = scene.attrib.vertices.ptr[3 * idx.vertex_index + 2];
            const f32        u   = scene.attrib.texcoords.ptr[2 * idx.texcoord_index + 0];
            const f32        v   = scene.attrib.texcoords.ptr[2 * idx.texcoord_index + 1];

            result.indices.push(i);
            result.vertices.push(Vector3f::xyz(x, y, z));
            result.tex_coords.push(Vector2f::xy(u, v));
        }

        tobj_scene_free_f(&scene);
        tobj_diag_free(&diag, nullptr);

        return result;
    }

    Maybe<AssetHandle<Mesh>> AssetLoader::load_mesh(StringSlice filepath) {
        auto        asset      = asset_path<Mesh>(filepath);
        Maybe<Mesh> maybe_mesh = load_obj(filepath);
        if (!maybe_mesh) {
            return {};
        }

        meshes_[asset.asset_id] = maybe_mesh.move();
        return asset;
    }

    Maybe<AssetHandle<Image>> AssetLoader::load_image(StringSlice filepath) {
        auto asset = asset_path<Image>(filepath);

        return asset;
    }
}
