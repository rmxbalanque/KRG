#include "MeshEditor.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    class MeshEditorFactory : public EditorFactory
    {
    public:

        virtual StringID GetID() const override  { return StringID( "MeshEditor" ); }
        virtual Editor* CreateEditor() const override { return KRG::New<MainEditor>(); }
    };

    static const MeshEditorFactory g_meshEditorFactory;
}

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    MainEditor::~MainEditor()
    {
        KRG_ASSERT( m_pDataBrowser == nullptr );
        KRG_ASSERT( m_pMeshInfo == nullptr );
    }

    void MainEditor::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        TEditor<Model>::Initialize( context, settingsRegistry );
     
        m_pDataBrowser = CreateTool<DataBrowser>( &GetModel() );
        m_pMeshInfo = CreateTool<MeshInfo>( &GetModel() );
    }

    void MainEditor::Shutdown( UpdateContext const& context )
    {
        DestroyTool( m_pMeshInfo );
        DestroyTool( m_pDataBrowser );

        TEditor<Model>::Shutdown( context );
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