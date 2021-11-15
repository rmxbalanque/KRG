#include "Workspace_AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    KRG_RESOURCE_WORKSPACE_FACTORY( AnimationGraphEditorFactory, AnimationGraphDefinition, AnimationGraphWorkspace );

    //-------------------------------------------------------------------------

    AnimationGraphWorkspace::AnimationGraphWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
        : TResourceWorkspace<AnimationGraphDefinition>( context, pWorld, resourceID, false )
        , m_graphEditorModel( context, resourceID )
        , m_controlParameterEditor( m_graphEditorModel )
        , m_graphView( m_graphEditorModel )
        , m_propertyGrid( m_graphEditorModel )
        , m_variationEditor( m_graphEditorModel )
    {}

    AnimationGraphWorkspace::~AnimationGraphWorkspace()
    {
        if ( m_graphEditorModel.IsPreviewing() )
        {
            m_graphEditorModel.StopPreview( m_pWorld );
        }
    }

    void AnimationGraphWorkspace::Initialize( UpdateContext const& context )
    {
        TResourceWorkspace<AnimationGraphDefinition>::Initialize( context );

        m_controlParametersWindowName.sprintf( "Control Parameters##%u", GetID() );
        m_graphViewWindowName.sprintf( "Graph View##%u", GetID() );
        m_propertyGridWindowName.sprintf( "Properties##%u", GetID() );
        m_variationEditorWindowName.sprintf( "Variation Editor##%u", GetID() );
    }

    void AnimationGraphWorkspace::InitializeDockingLayout( ImGuiID dockspaceID ) const
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

    void AnimationGraphWorkspace::UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        m_controlParameterEditor.UpdateAndDraw( context, pWindowClass, m_controlParametersWindowName.c_str() );
        m_graphView.UpdateAndDraw( context, pWindowClass, m_graphViewWindowName.c_str() );
        m_propertyGrid.UpdateAndDraw( context, pWindowClass, m_propertyGridWindowName.c_str() );
        m_variationEditor.UpdateAndDraw( context, pWindowClass, m_variationEditorWindowName.c_str() );
    }

    void AnimationGraphWorkspace::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        if ( m_graphEditorModel.IsPreviewing() )
        {
            if ( ImGui::Button( "Stop Preview" ) )
            {
                m_graphEditorModel.StopPreview( m_pWorld );
            }
        }
        else
        {
            if ( ImGui::Button( "Start Preview" ) )
            {
                m_graphEditorModel.StartPreview( m_pWorld );
            }
        }
    }

    //-------------------------------------------------------------------------

    bool AnimationGraphWorkspace::IsDirty() const
    {
        return true;
    }

    bool AnimationGraphWorkspace::Save()
    {
        return m_graphEditorModel.SaveGraph();
    }
}