#pragma once
#include "EditorTool.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_TOOLS_CORE_API EditorTabWell : public TEditorTool<EditorModel>
    {
    public:

        using TEditorTool::TEditorTool;

    private:

        virtual char const* const GetName() { return "Open Files"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;
    };
}