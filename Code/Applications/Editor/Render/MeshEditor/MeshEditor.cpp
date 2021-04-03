#include "MeshEditor.h"


//-------------------------------------------------------------------------

namespace KRG::Render
{
    class MeshEditorFactory : public EditorFactory
    {
    public:

        virtual StringID GetID() const override  { return StringID( "MeshEditor" ); }
        virtual Editor* CreateEditor() const override { return KRG::New<MeshEditor>(); }
    };

    static const MeshEditorFactory g_meshEditorFactory;
}

//-------------------------------------------------------------------------

namespace KRG::Render
{
    void MeshEditor::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        Editor::Initialize( context, settingsRegistry );
        static char const* const allowedExtensions[] = { ".msh", ".smsh", ".fbx", ".gltf", 0 };
        m_dataDirectoryModel.Initialize( allowedExtensions );
    }

    void MeshEditor::Shutdown()
    {
        m_dataDirectoryModel.Shutdown();
        Editor::Shutdown();
    }

    void MeshEditor::DrawMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        ImGui::TextColored( (Float4) Colors::LimeGreen, KRG_ICON_CUBES );

        if ( ImGui::BeginMenu( "Windows" ) )
        {
            ImGui::Checkbox( "Mesh Browser", &m_showDataBrowser );
            ImGui::Checkbox( "Mesh Info", &m_showMeshInfoWindow );
            ImGui::Checkbox( "Skeletal Mesh Info", &m_showSkeletalMeshInfoWindow );

            ImGui::EndMenu();
        }
    }

    void MeshEditor::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        m_dataDirectoryModel.Update( context );

        //-------------------------------------------------------------------------

        if ( m_showDataBrowser )
        {
            DrawMeshBrowserWindow();
            DrawDataFileInfoWindow();
        }

        //-------------------------------------------------------------------------

        if ( m_showMeshInfoWindow )
        {
            DrawMeshInfoWindow();
        }
    }
}