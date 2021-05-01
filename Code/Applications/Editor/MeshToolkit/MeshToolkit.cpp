#include "MeshToolkit.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
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

namespace KRG::Render::MeshTools
{
    MeshToolkit::~MeshToolkit()
    {
        KRG_ASSERT( m_pDataBrowser == nullptr );
        KRG_ASSERT( m_pDocumentWell == nullptr );
    }

    void MeshToolkit::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        TEditorToolkit<EditorModel>::Initialize( context, settingsRegistry );
     
        m_pDataBrowser = CreateTool<DataBrowser>( context, &GetModel() );
        m_pDocumentWell = CreateTool<DocumentWell>( context, &GetModel() );
    }

    void MeshToolkit::Shutdown( UpdateContext const& context )
    {
        DestroyTool( context, m_pDocumentWell );
        DestroyTool( context, m_pDataBrowser );

        TEditorToolkit<EditorModel>::Shutdown( context );
    }

    void MeshToolkit::SetUserFlags( uint64 flags )
    {
        m_pDataBrowser->SetOpen( flags & ( 1 << 0 ) );
        m_pDocumentWell->SetOpen( flags & ( 1 << 1 ) );
    }

    uint64 MeshToolkit::GetUserFlags() const
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