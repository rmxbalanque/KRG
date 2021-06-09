#include "MeshEditor.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class MeshEditorFactory : public EditorFactory
    {
    public:

        virtual const char* GetID() const override { return "MeshEditor" ; }
        virtual Editor* CreateEditor() const override { return KRG::New<MeshEditor>(); }
    };

    static const MeshEditorFactory g_meshEditorFactory;
}

//-------------------------------------------------------------------------

namespace KRG::Render
{
    MeshEditor::~MeshEditor()
    {
        KRG_ASSERT( m_pDataBrowser == nullptr );
        KRG_ASSERT( m_pTabWell == nullptr );
    }

    void MeshEditor::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        TEditor<EditorModel>::Initialize( context, settingsRegistry );
     
        m_pDataBrowser = CreateTool<DataBrowser>( context, &GetModel() );
        m_pTabWell = CreateTool<EditorTabWell>( context, &GetModel() );
    }

    void MeshEditor::Shutdown( UpdateContext const& context )
    {
        DestroyTool( context, m_pTabWell );
        DestroyTool( context, m_pDataBrowser );

        TEditor<EditorModel>::Shutdown( context );
    }

    void MeshEditor::SetUserFlags( uint64 flags )
    {
        m_pDataBrowser->SetOpen( flags & ( 1 << 0 ) );
        m_pTabWell->SetOpen( flags & ( 1 << 1 ) );
    }

    uint64 MeshEditor::GetUserFlags() const
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