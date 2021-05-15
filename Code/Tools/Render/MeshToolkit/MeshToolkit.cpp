#include "MeshToolkit.h"

//-------------------------------------------------------------------------

namespace KRG::Render::Tools
{
    class MeshEditorFactory : public EditorToolkitFactory
    {
    public:

        virtual const char* GetID() const override { return "MeshToolkit" ; }
        virtual EditorToolkit* CreateEditor() const override { return KRG::New<MeshToolkit>(); }
    };

    static const MeshEditorFactory g_meshEditorFactory;
}

//-------------------------------------------------------------------------

namespace KRG::Render::Tools
{
    MeshToolkit::~MeshToolkit()
    {
        KRG_ASSERT( m_pDataBrowser == nullptr );
        KRG_ASSERT( m_pTabWell == nullptr );
    }

    void MeshToolkit::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        TEditorToolkit<EditorModel>::Initialize( context, settingsRegistry );
     
        m_pDataBrowser = CreateTool<DataBrowser>( context, &GetModel() );
        m_pTabWell = CreateTool<Editor::TabWell>( context, &GetModel() );
    }

    void MeshToolkit::Shutdown( UpdateContext const& context )
    {
        DestroyTool( context, m_pTabWell );
        DestroyTool( context, m_pDataBrowser );

        TEditorToolkit<EditorModel>::Shutdown( context );
    }

    void MeshToolkit::SetUserFlags( uint64 flags )
    {
        m_pDataBrowser->SetOpen( flags & ( 1 << 0 ) );
        m_pTabWell->SetOpen( flags & ( 1 << 1 ) );
    }

    uint64 MeshToolkit::GetUserFlags() const
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