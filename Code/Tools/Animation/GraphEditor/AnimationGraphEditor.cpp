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

        if ( ImGui::MenuItem( "Refresh View" ) )
        {
            m_pGraphEditor->RefreshView();
        }
    }

    void AnimationGraphEditor::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        m_pControlParameterEditor->Draw( context, viewportManager );
        m_pGraphEditor->Draw( context, viewportManager );
        m_pPropertyGrid->Draw( context, viewportManager );
        m_pVariationEditor->Draw( context, viewportManager );
    }
}