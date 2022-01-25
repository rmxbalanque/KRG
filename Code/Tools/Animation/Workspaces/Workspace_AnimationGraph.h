#pragma once

#include "Tools/Animation/GraphEditor/AnimationGraphEditor_ControlParameterEditor.h"
#include "Tools/Animation/GraphEditor/AnimationGraphEditor_GraphEditor.h"
#include "Tools/Animation/GraphEditor/AnimationGraphEditor_VariationEditor.h"
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_Common.h"
#include "Tools/Core/Workspaces/ResourceWorkspace.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Resources.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsSystem;
}

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphComponent;
    class GraphUndoableAction;

    //-------------------------------------------------------------------------

    class AnimationGraphWorkspace final : public TResourceWorkspace<GraphDefinition>
    {
        friend GraphUndoableAction;

    public:

        AnimationGraphWorkspace( WorkspaceInitializationContext const& context, EntityWorld* pWorld, ResourceID const& resourceID );
        ~AnimationGraphWorkspace();

    private:

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void InitializeDockingLayout( ImGuiID dockspaceID ) const override;
        virtual void UpdateWorld( EntityWorldUpdateContext const& updateContext ) override;

        virtual bool HasViewportToolbar() const override { return true; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport ) override;
        virtual void UpdateWorkspace( UpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        virtual void OnUndoRedo( UndoStack::Operation operation, IUndoableAction const* pAction ) override;
        virtual bool IsDirty() const override;
        virtual bool AlwaysAllowSaving() const override { return true; }
        virtual bool Save() override;

        void GenerateAnimGraphVariationDescriptors();

        // Preview
        //-------------------------------------------------------------------------

        inline bool IsPreviewing() const { return m_isPreviewing; }
        void StartPreview( UpdateContext const& context );
        void StopPreview();

        void DrawDebuggerWindow( UpdateContext const& context );

    private:

        String                              m_controlParametersWindowName;
        String                              m_graphViewWindowName;
        String                              m_propertyGridWindowName;
        String                              m_variationEditorWindowName;
        String                              m_debuggerWindowName;

        GraphControlParameterEditor*        m_pControlParameterEditor = nullptr;
        GraphVariationEditor*               m_pVariationEditor = nullptr;
        GraphEditor*                        m_pGraphEditor = nullptr;
        PropertyGrid                        m_propertyGrid;

        EventBindingID                      m_rootGraphBeginModificationBindingID;
        EventBindingID                      m_rootGraphEndModificationBindingID;
        EventBindingID                      m_preEditEventBindingID;
        EventBindingID                      m_postEditEventBindingID;

        GraphUndoableAction*                m_pActiveUndoableAction = nullptr;

        EditorGraphDefinition*              m_pGraphDefinition = nullptr;
        FileSystem::Path                    m_graphFilePath;
        StringID                            m_selectedVariationID = GraphVariation::DefaultVariationID;

        // Preview
        Physics::PhysicsSystem*             m_pPhysicsSystem = nullptr;
        Entity*                             m_pPreviewEntity = nullptr;
        AnimationGraphComponent*            m_pGraphComponent = nullptr;
        DebugContext                        m_debugContext;
        Transform                           m_previewStartTransform = Transform::Identity;
        bool                                m_startPaused = false;
        bool                                m_isPreviewing = false;
        RootMotionRecorder::DebugMode       m_rootMotionDebugMode = RootMotionRecorder::DebugMode::Off;
        TaskSystem::DebugMode               m_taskSystemDebugMode = TaskSystem::DebugMode::Off;
    };
}