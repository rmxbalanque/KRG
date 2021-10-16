#pragma once

#include "Tools/Animation/GraphEditor/AnimationGraphEditor_Model.h"
#include "Tools/Animation/GraphEditor/AnimationGraphEditor_ControlParameterEditor.h"
#include "Tools/Animation/GraphEditor/AnimationGraphEditor_GraphView.h"
#include "Tools/Animation/GraphEditor/AnimationGraphEditor_PropertyGrid.h"
#include "Tools/Animation/GraphEditor/AnimationGraphEditor_VariationEditor.h"
#include "Tools/Core/Resource/ResourceEditorWorkspace.h"
#include "Engine/Animation/Graph/AnimationGraphResources.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class AnimationGraphResourceEditor final : public TResourceEditorWorkspace<AnimationGraphDefinition>
    {
        static char const* const s_controlParametersWindowName;
        static char const* const s_graphViewWindowName;
        static char const* const s_propertyGridWindowName;
        static char const* const s_variationEditorWindowName;

    public:

        AnimationGraphResourceEditor( ResourceEditorContext const& context, ResourceID const& resourceID );
        ~AnimationGraphResourceEditor();

    private:

        virtual char const* GetWorkspaceName() const override { return "Animation Graph"; }
        virtual void Activate( EntityWorld* pPreviewWorld ) override;
        virtual void Deactivate( EntityWorld* pPreviewWorld ) override;

        virtual void UpdateAndDraw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass ) override;

        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;

        virtual bool HasViewportToolbar() const override { return true; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

        virtual bool IsDirty() const override;
        virtual bool OnSave() override;

    private:

        EntityWorld*                        m_pPreviewWorld = nullptr;
        GraphEditorModel                    m_graphEditorModel;
        GraphControlParameterEditor         m_controlParameterEditor;
        GraphView                           m_graphView;
        GraphPropertyGrid                   m_propertyGrid;
        GraphVariationEditor                m_variationEditor;
    };
}