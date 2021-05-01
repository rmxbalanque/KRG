#include "AnimationToolkit.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    class AnimationEditorFactory : public EditorToolkitFactory
    {
    public:

        virtual const char* GetID() const override { return "AnimationToolkit"; }
        virtual EditorToolkit* CreateEditor() const override { return KRG::New<AnimationToolkit>(); }
    };

    static const AnimationEditorFactory g_animationEditorFactory;
}

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    AnimationToolkit::~AnimationToolkit()
    {
        KRG_ASSERT( m_pDataBrowser == nullptr );
        KRG_ASSERT( m_pDocumentWell == nullptr );
    }

    void AnimationToolkit::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        TEditorToolkit<EditorModel>::Initialize( context, settingsRegistry );
     
        m_pDataBrowser = CreateTool<DataBrowser>( context, &GetModel() );
        m_pDocumentWell = CreateTool<DocumentWell>( context, &GetModel() );
    }

    void AnimationToolkit::Shutdown( UpdateContext const& context )
    {
        DestroyTool( context, m_pDocumentWell );
        DestroyTool( context, m_pDataBrowser );

        TEditorToolkit<EditorModel>::Shutdown( context );
    }

    void AnimationToolkit::SetUserFlags( uint64 flags )
    {
        m_pDataBrowser->SetOpen( flags & ( 1 << 0 ) );
        m_pDocumentWell->SetOpen( flags & ( 1 << 1 ) );
    }

    uint64 AnimationToolkit::GetUserFlags() const
    {
        uint64 flags = 0;

        if ( m_pDataBrowser->IsOpen() )
        {
            flags |= ( 1 << 0 );
        }

        if ( m_pDocumentWell->IsOpen() )
        {
            flags |= ( 1 << 1 );
        }

        return flags;
    }
}