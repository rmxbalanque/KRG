#include "ResourceEditor_AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    KRG_RESOURCE_WORKSPACE_FACTORY( AnimationGraphEditorFactory, AnimationGraphDefinition, AnimationGraphResourceEditor );

    //-------------------------------------------------------------------------

    char const* const AnimationGraphResourceEditor::s_controlParametersWindowName = "Control Parameters##AnimationGraph";
    char const* const AnimationGraphResourceEditor::s_graphViewWindowName = "Graph View##AnimationGraph";
    char const* const AnimationGraphResourceEditor::s_propertyGridWindowName = "Properties##AnimationGraph";
    char const* const AnimationGraphResourceEditor::s_variationEditorWindowName = "Variation Editor##AnimationGraph";

    //-------------------------------------------------------------------------

    AnimationGraphResourceEditor::AnimationGraphResourceEditor( ResourceEditorContext const& context, ResourceID const& resourceID )
        : TResourceEditorWorkspace<AnimationGraphDefinition>( context, resourceID, false )
        , m_graphEditorModel( context, resourceID )
        , m_controlParameterEditor( m_graphEditorModel )
        , m_graphView( m_graphEditorModel )
        , m_propertyGrid( m_graphEditorModel )
        , m_variationEditor( m_graphEditorModel )
    {}

    AnimationGraphResourceEditor::~AnimationGraphResourceEditor()
    {
        if ( m_graphEditorModel.IsPreviewing() )
        {
            m_graphEditorModel.StopPreview( m_editorContext.m_pPreviewWorld );
        }
    }

    void AnimationGraphResourceEditor::Activate( EntityWorld* pPreviewWorld )
    {
        m_pPreviewWorld = pPreviewWorld;
    }

    void AnimationGraphResourceEditor::Deactivate( EntityWorld* pPreviewWorld )
    {
        m_pPreviewWorld = nullptr;
    }

    void AnimationGraphResourceEditor::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        //    ImGuiID topDockID = 0;
        //    ImGuiID bottomLeftDockID = 0;
        //    ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.5f, nullptr, &topDockID );
        //    ImGuiID bottomRightDockID = ImGui::DockBuilderSplitNode( bottomDockID, ImGuiDir_Right, 0.25f, nullptr, &bottomLeftDockID );

        //    // Dock viewport
        //    ImGuiDockNode* pTopNode = ImGui::DockBuilderGetNode( topDockID );
        //    pTopNode.LocalFlags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;
        //    ImGui::DockBuilderDockWindow( GetViewportWindowName(), topDockID );

        //    // Dock windows
        //    ImGui::DockBuilderDockWindow( s_timelineWindowName, bottomLeftDockID );
        //    ImGui::DockBuilderDockWindow( s_trackDataWindowName, bottomRightDockID );
        //    ImGui::DockBuilderDockWindow( s_detailsWindowName, bottomRightDockID );
    }

    void AnimationGraphResourceEditor::UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        m_controlParameterEditor.UpdateAndDraw( context, pWindowClass, s_controlParametersWindowName );
        m_graphView.UpdateAndDraw( context, pWindowClass, s_graphViewWindowName );
        m_propertyGrid.UpdateAndDraw( context, pWindowClass, s_propertyGridWindowName );
        m_variationEditor.UpdateAndDraw( context, pWindowClass, s_variationEditorWindowName );
    }

    void AnimationGraphResourceEditor::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        if ( m_graphEditorModel.IsPreviewing() )
        {
            if ( ImGui::Button( "Stop Preview" ) )
            {
                m_graphEditorModel.StopPreview( m_editorContext.m_pPreviewWorld );
            }
        }
        else
        {
            if ( ImGui::Button( "Start Preview" ) )
            {
                m_graphEditorModel.StartPreview( m_editorContext.m_pPreviewWorld );
            }
        }
    }

    //-------------------------------------------------------------------------

    bool AnimationGraphResourceEditor::IsDirty() const
    {
        return false;
    }

    bool AnimationGraphResourceEditor::OnSave()
    {
        return m_graphEditorModel.SaveGraph();
    }
}