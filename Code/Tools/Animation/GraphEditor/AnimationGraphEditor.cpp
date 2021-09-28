#include "AnimationGraphEditor.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    KRG_STANDALONE_EDITOR_FACTORY( AnimationGraphEditorFactory, AnimationGraphEditor, AnimationGraphEditor );

    //-------------------------------------------------------------------------

    AnimationGraphEditor::~AnimationGraphEditor()
    {
        KRG_ASSERT( m_pControlParameterEditor == nullptr );
        KRG_ASSERT( m_pGraphEditor == nullptr );
        KRG_ASSERT( m_pPropertyGrid == nullptr );
        KRG_ASSERT( m_pVariationEditor == nullptr );
    }

    void AnimationGraphEditor::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        TStandaloneEditor<GraphEditorModel>::Initialize( context, settingsRegistry );

        m_pControlParameterEditor = KRG::New<GraphControlParameterEditor>( GetModel() );
        m_pGraphEditor = KRG::New<GraphEditorView>( GetModel() );
        m_pPropertyGrid = KRG::New<GraphPropertyGrid>( GetModel() );
        m_pVariationEditor = KRG::New<GraphVariationEditor>( GetModel() );
    }

    void AnimationGraphEditor::Shutdown( UpdateContext const& context )
    {
        KRG::Delete( m_pControlParameterEditor );
        KRG::Delete( m_pGraphEditor );
        KRG::Delete( m_pPropertyGrid );
        KRG::Delete( m_pVariationEditor );

        TStandaloneEditor<GraphEditorModel>::Shutdown( context );
    }

    void AnimationGraphEditor::DrawMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( ImGui::BeginMenu( "File" ) )
        {
            if ( ImGui::MenuItem( "New" ) )
            {
                GetModel().NewGraph();
            }

            if ( ImGui::MenuItem( "Open" ) )
            {
                GetModel().OpenGraph();
            }

            if ( ImGui::MenuItem( "Save" ) )
            {
                GetModel().SaveGraph();
            }

            if ( ImGui::MenuItem( "Save As" ) )
            {
                GetModel().SaveGraphAsNewFile();
            }

            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu( "Graph" ) )
        {
            if ( ImGui::MenuItem( "Simulate Compilation" ) )
            {
                GetModel().SimulateCompilation();
            }

            ImGui::EndMenu();
        }
    }

    void AnimationGraphEditor::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        ImGuiWindowClass mainEditorWindowClass;
        mainEditorWindowClass.ClassId = ImGui::GetID( "EditorWindowClass" );
        mainEditorWindowClass.DockingAllowUnclassed = false;

        ImGuiID const dockspaceID = ImGui::GetID( "GraphEditorDockSpace" );
        ImGuiID dockRightID = 0, dockLeftID = 0;

        ImGuiWindowFlags const windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGuiViewport const* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->WorkPos );
        ImGui::SetNextWindowSize( viewport->WorkSize );
        ImGui::SetNextWindowViewport( viewport->ID );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
        bool const shouldDrawEditorDockspace = ImGui::Begin( "GraphEditorDockSpaceWindow", nullptr, windowFlags );
        ImGui::PopStyleVar( 3 );

        if ( shouldDrawEditorDockspace )
        {
            // TODO: Initial Layout
            ImGui::DockSpace( dockspaceID, viewport->WorkSize, ImGuiDockNodeFlags_None, &mainEditorWindowClass );
        }
        ImGui::End();

        //-------------------------------------------------------------------------
        // Draw editor windows
        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( &mainEditorWindowClass );
        m_pControlParameterEditor->Draw( context, viewportManager );

        ImGui::SetNextWindowClass( &mainEditorWindowClass );
        m_pGraphEditor->Draw( context, viewportManager );

        ImGui::SetNextWindowClass( &mainEditorWindowClass );
        m_pPropertyGrid->Draw( context, viewportManager );

        ImGui::SetNextWindowClass( &mainEditorWindowClass );
        m_pVariationEditor->Draw( context, viewportManager );
    }
}