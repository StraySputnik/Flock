#pragma once

#include "common.hpp"
#include "maybe.hpp"

namespace Flock::Graphics {
    using RendererID = u32;

    static constexpr RendererID INVALID_RENDERER_ID = INVALID_32;

    struct FLK_API GpuMesh {
        RendererID id = INVALID_RENDERER_ID;
    };

    struct FLK_API GpuTexture {
        RendererID id = INVALID_RENDERER_ID;
    };

    struct FLK_API RenderObject {
        GpuMesh           mesh;
        Maybe<GpuTexture> texture;
    };

    class FLK_API Renderer {
    public:
        virtual ~Renderer() = default;

        virtual void render(const RenderObject &render_object) = 0;
    };
}
