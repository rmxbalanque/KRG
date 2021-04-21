#include "MeshToolkit.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshToolkit
{
    class MeshEditorFactory : public EditorToolkitFactory
    {
    public:

        virtual StringID GetID() const override  { return StringID( "MeshEditor" ); }
        virtual EditorToolkit* CreateEditor() const override { return KRG::New<MainEditor>(); }
    };

    static const MeshEditorFactory g_meshEditorFactory;
}

//-------------------------------------------------------------------------

namespace KRG::Render::MeshToolkit
{
    MainEditor::~MainEditor()
    {
        KRG_ASSERT( m_pDataBrowser == nullptr );
        KRG_ASSERT( m_pMeshInfo == nullptr );
    }

    void MainEditor::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        TEditorToolkit<Model>::Initialize( context, settingsRegistry );
     
        m_pDataBrowser = CreateTool<DataBrowser>( context, &GetModel() );
        m_pMeshInfo = CreateTool<MeshInfo>( context, &GetModel() );
    }

    void MainEditor::Shutdown( UpdateContext const& context )
    {
        DestroyTool( context, m_pMeshInfo );
        DestroyTool( context, m_pDataBrowser );

        TEditorToolkit<Model>::Shutdown( context );
    }

    void MainEditor::SetUserFlags( uint64 flags )
    {
        m_pDataBrowser->SetOpen( flags & ( 1 << 0 ) );
        m_pDataBrowser->SetOpen( flags & ( 1 << 0 ) );

        m_pMeshInfo->SetOpen( flags & ( 1 << 1 ) );
    }

    uint64 MainEditor::GetUserFlags() const
    {
        uint64 flags = 0;

        if ( m_pDataBrowser->IsOpen() )
        {
            flags |= ( 1 << 0 );
        }

        if ( m_pMeshInfo->IsOpen() )
        {
            flags |= ( 1 << 1 );
        }

        return flags;
    }
}