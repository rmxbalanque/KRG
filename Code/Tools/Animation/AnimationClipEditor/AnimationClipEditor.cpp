#include "AnimationClipEditor.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationClipEditorFactory : public EditorFactory
    {
    public:

        virtual const char* GetID() const override { return "AnimationClipEditor"; }
        virtual Editor* CreateEditor() const override { return KRG::New<AnimationClipEditor>(); }
    };

    static const AnimationClipEditorFactory g_animationToolkitFactory;
}

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    AnimationClipEditor::~AnimationClipEditor()
    {
        KRG_ASSERT( m_pDataBrowser == nullptr );
        KRG_ASSERT( m_pTabWell == nullptr );
    }

    void AnimationClipEditor::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        TEditor<EditorModel>::Initialize( context, settingsRegistry );
     
        m_pDataBrowser = CreateTool<DataBrowser>( context, &GetModel() );
        m_pTabWell = CreateTool<EditorTabWell>( context, &GetModel() );
    }

    void AnimationClipEditor::Shutdown( UpdateContext const& context )
    {
        DestroyTool( context, m_pTabWell );
        DestroyTool( context, m_pDataBrowser );

        TEditor<EditorModel>::Shutdown( context );
    }

    void AnimationClipEditor::SetUserFlags( uint64 flags )
    {
        m_pDataBrowser->SetOpen( flags & ( 1 << 0 ) );
        m_pTabWell->SetOpen( flags & ( 1 << 1 ) );
    }

    uint64 AnimationClipEditor::GetUserFlags() const
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