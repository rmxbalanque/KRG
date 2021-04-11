#pragma once
#include "MeshEditor_Model.h"
#include "Applications/Editor/EditorTool.h"
#include "Applications/Editor/Widgets/PropertyGrid/PropertyGrid.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    class DataBrowser final : public TEditorTool<Model>
    {
    public:

        DataBrowser( EditorModel* pModel );

        virtual char const * const GetName() { return "Data Browser"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        void DrawBrowser( UpdateContext const& context );
        void DrawInfoPanel( UpdateContext const& context );
        void UpdateVisibility();

    private:

        // Browser
        char                                m_filterBuffer[256];
        bool                                m_showStaticMeshes = true;
        bool                                m_showSkeletalMeshes = true;
        bool                                m_showRawFiles = false;

        // Info
        FileSystem::Path                    m_inspectedFile;
        TypeSystem::PropertyGrid            m_propertyGrid;
        TypeSystem::TypeInstanceModel       m_typeInstance;
    };
}