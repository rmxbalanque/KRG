#pragma once

#include "System/Render/_Module/API.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class RenderDevice;
    class Texture;

    namespace DefaultResources
    {
        void Initialize( RenderDevice* pRenderDevice );
        void Shutdown( RenderDevice* pRenderDevice );

        KRG_SYSTEM_RENDER_API Texture const* GetDefaultTexture();
    };
}