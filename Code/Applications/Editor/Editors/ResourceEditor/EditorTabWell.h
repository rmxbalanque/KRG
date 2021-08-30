#pragma once

#include "ResourceEditor_Model.h"
#include "System\Render\RenderViewportManager.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class EditorTabWell
    {

    private:

        char const* const GetName() { return "Open Files"; }
        void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager );
    };
}