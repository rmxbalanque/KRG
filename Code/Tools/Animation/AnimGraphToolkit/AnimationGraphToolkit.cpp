#include "AnimationGraphToolkit.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    class AnimGraphToolkitFactory : public EditorToolkitFactory
    {
    public:

        virtual const char* GetID() const override { return "AnimGraphToolkit"; }
        virtual EditorToolkit* CreateEditor() const override { return KRG::New<AnimGraphToolkit>(); }
    };

    static const AnimGraphToolkitFactory g_animGraphToolkitFactory;

    //-------------------------------------------------------------------------

    AnimGraphToolkit::~AnimGraphToolkit()
    {
        KRG_ASSERT( m_pControlParameterEditor == nullptr );
        KRG_ASSERT( m_pGraphEditor == nullptr );
        KRG_ASSERT( m_pPropertyGrid == nullptr );
    }

    void AnimGraphToolkit::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        TEditorToolkit<EditorModel>::Initialize( context, settingsRegistry );

        m_pControlParameterEditor = CreateTool<GraphControlParameterEditor>( context, &GetModel() );
        m_pGraphEditor = CreateTool<GraphEditor>( context, &GetModel() );
        m_pPropertyGrid = CreateTool<GraphPropertyGrid>( context, &GetModel() );
    }

    void AnimGraphToolkit::Shutdown( UpdateContext const& context )
    {
        DestroyTool( context, m_pControlParameterEditor );
        DestroyTool( context, m_pGraphEditor );
        DestroyTool( context, m_pPropertyGrid );

        TEditorToolkit<EditorModel>::Shutdown( context );
    }

    void AnimGraphToolkit::SetUserFlags( uint64 flags )
    {
        m_pControlParameterEditor->SetOpen( flags & ( 1 << 0 ) );
        m_pGraphEditor->SetOpen( flags & ( 1 << 1 ) );
        m_pPropertyGrid->SetOpen( flags & ( 1 << 2 ) );
    }

    uint64 AnimGraphToolkit::GetUserFlags() const
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

        return flags;
    }
}