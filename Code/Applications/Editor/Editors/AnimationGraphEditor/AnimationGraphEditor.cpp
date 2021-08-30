#include "AnimationGraphEditor.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class AnimationGraphEditorFactory : public EditorFactory
    {
    public:

        virtual const char* GetID() const override { return "AnimationGraphEditor"; }
        virtual Editor* CreateEditor() const override { return KRG::New<AnimationGraphEditor>(); }
    };

    static const AnimationGraphEditorFactory g_animationGraphToolkitFactory;

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
        TEditor<GraphEditorModel>::Initialize( context, settingsRegistry );

        m_pControlParameterEditor = CreateTool<GraphControlParameterEditor>( context, &GetModel() );
        m_pGraphEditor = CreateTool<GraphEditorView>( context, &GetModel() );
        m_pPropertyGrid = CreateTool<GraphPropertyGrid>( context, &GetModel() );
        m_pVariationEditor = CreateTool<GraphVariationEditor>( context, &GetModel() );
    }

    void AnimationGraphEditor::Shutdown( UpdateContext const& context )
    {
        DestroyTool( context, m_pControlParameterEditor );
        DestroyTool( context, m_pGraphEditor );
        DestroyTool( context, m_pPropertyGrid );
        DestroyTool( context, m_pVariationEditor );

        TEditor<GraphEditorModel>::Shutdown( context );
    }

    void AnimationGraphEditor::SetUserFlags( uint64 flags )
    {
        m_pControlParameterEditor->SetOpen( flags & ( 1 << 0 ) );
        m_pGraphEditor->SetOpen( flags & ( 1 << 1 ) );
        m_pPropertyGrid->SetOpen( flags & ( 1 << 2 ) );
        m_pVariationEditor->SetOpen( flags & ( 1 << 3 ) );
    }

    uint64 AnimationGraphEditor::GetUserFlags() const
    {
        uint64 flags = 0;

        if ( m_pControlParameterEditor != nullptr && m_pControlParameterEditor->IsOpen() )
        {
            flags |= ( 1 << 0 );
        }

        if ( m_pGraphEditor != nullptr && m_pGraphEditor->IsOpen() )
        {
            flags |= ( 1 << 1 );
        }

        if ( m_pPropertyGrid != nullptr && m_pPropertyGrid->IsOpen() )
        {
            flags |= ( 1 << 2 );
        }

        if ( m_pVariationEditor != nullptr && m_pVariationEditor->IsOpen() )
        {
            flags |= ( 1 << 3 );
        }

        return flags;
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
}