#include "AnimationToolkit.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    class AnimationToolkitFactory : public EditorToolkitFactory
    {
    public:

        virtual const char* GetID() const override { return "AnimationToolkit"; }
        virtual EditorToolkit* CreateEditor() const override { return KRG::New<AnimationToolkit>(); }
    };

    static const AnimationToolkitFactory g_animationToolkitFactory;
}

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    AnimationToolkit::~AnimationToolkit()
    {
        KRG_ASSERT( m_pDataBrowser == nullptr );
        KRG_ASSERT( m_pTabWell == nullptr );
    }

    void AnimationToolkit::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        TEditorToolkit<EditorModel>::Initialize( context, settingsRegistry );
     
        m_pDataBrowser = CreateTool<DataBrowser>( context, &GetModel() );
        m_pTabWell = CreateTool<Editor::TabWell>( context, &GetModel() );
    }

    void AnimationToolkit::Shutdown( UpdateContext const& context )
    {
        DestroyTool( context, m_pTabWell );
        DestroyTool( context, m_pDataBrowser );

        TEditorToolkit<EditorModel>::Shutdown( context );
    }

    void AnimationToolkit::SetUserFlags( uint64 flags )
    {
        m_pDataBrowser->SetOpen( flags & ( 1 << 0 ) );
        m_pTabWell->SetOpen( flags & ( 1 << 1 ) );
    }

    uint64 AnimationToolkit::GetUserFlags() const
    {
        uint64 flags = 0;

        if ( m_pDataBrowser != nullptr && m_pDataBrowser->IsOpen() )
        {
            flags |= ( 1 << 0 );
        }

        if ( m_pTabWell != nullptr && m_pTabWell->IsOpen() )
        {
            flags |= ( 1 << 1 );
        }

        return flags;
    }
}