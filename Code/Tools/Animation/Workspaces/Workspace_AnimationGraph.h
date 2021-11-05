#pragma once

#include "Tools/Animation/GraphEditor/AnimationGraphEditor_Model.h"
#include "Tools/Animation/GraphEditor/AnimationGraphEditor_ControlParameterEditor.h"
#include "Tools/Animation/GraphEditor/AnimationGraphEditor_GraphView.h"
#include "Tools/Animation/GraphEditor/AnimationGraphEditor_PropertyGrid.h"
#include "Tools/Animation/GraphEditor/AnimationGraphEditor_VariationEditor.h"
#include "Tools/Core/Editor/ResourceWorkspace.h"
#include "Engine/Animation/Graph/AnimationGraphResources.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class AnimationGraphWorkspace final : public TResourceWorkspace<AnimationGraphDefinition>
    {
    public:

        AnimationGraphWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID );
        ~AnimationGraphWorkspace();

    private:

        virtual void Initialize() override;
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        virtual bool HasViewportToolbar() const override { return true; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) override;

        virtual bool IsDirty() const override;
        virtual bool Save() override;

    private:

        String                              m_controlParametersWindowName;
        String                              m_graphViewWindowName;
        String                              m_propertyGridWindowName;
        String                              m_variationEditorWindowName;

        GraphEditorModel                    m_graphEditorModel;
        GraphControlParameterEditor         m_controlParameterEditor;
        GraphView                           m_graphView;
        GraphPropertyGrid                   m_propertyGrid;
        GraphVariationEditor                m_variationEditor;
    };
}