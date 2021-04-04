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
        Editor::Initialize( context, settingsRegistry );
        m_model.Initialize( context );

        m_pDataBrowser = CreateTool<DataBrowser>( m_model );
        m_pMeshInfo = CreateTool<MeshInfo>( m_model );
    }

    void MainEditor::Shutdown()
    {
        DestroyTool( m_pMeshInfo );
        DestroyTool( m_pDataBrowser );

        m_model.Shutdown();
        Editor::Shutdown();
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

    void MainEditor::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        m_model.Update( context );
    }
}