#pragma once

#include "Tools/Core/Editor/EditorWorkspace.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class MapEditor final : public EditorWorkspace
    {
        static char const* const s_infoWindowName;

    public:

        using EditorWorkspace::EditorWorkspace;

        virtual char const* GetDisplayName() const override { return "Map Editor"; }
        virtual uint32 GetID() const override { return 0xFFFFFFFF; }

        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void OnNewMap();
        void OnLoadMap();
        void OnSaveMap();
        void OnSaveMapAs();

    private:

        virtual bool Save() override;

    private:

        ResourceID      m_loadedMap;
    };
}