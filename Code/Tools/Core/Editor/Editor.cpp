#include "Editor.h"
#include "System/DevTools/CommonWidgets/Gizmo/OrientationGuide.h"
#include "System/Render/RenderViewportManager.h"
#include "System/Input/InputSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Editor::~Editor()
    {
        KRG_ASSERT( m_editorTools.empty() );
        KRG::Delete( m_pModel );
    }

    void Editor::DestroyTool( UpdateContext const& context, EditorTool* pTool )
    {
        KRG_ASSERT( pTool != nullptr );

        pTool->Shutdown( context );

        auto itr = eastl::find( m_editorTools.begin(), m_editorTools.end(), pTool );
        KRG_ASSERT( itr != m_editorTools.end() );

        KRG::Delete( *itr );
        m_editorTools.erase( itr );
    }

    void Editor::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
        KRG_ASSERT( m_pModel != nullptr );
        m_pModel->Initialize( context );
    }

    void Editor::Shutdown( UpdateContext const& context )
    {
        KRG_ASSERT( m_pModel != nullptr );
        m_pModel->Shutdown( context );
    }

    void Editor::Update( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        UpdateStage const updateStage = context.GetUpdateStage();

        switch ( updateStage )
        {
            case UpdateStage::FrameStart:
            {
                m_pModel->Update( context );

                DrawEditorMainMenu( context, viewportManager );
                DrawEditorDockSpaceAndViewport( context, viewportManager );
                FrameStartUpdate( context, viewportManager );

                for ( auto pTool : m_editorTools )
                {
                    if ( pTool->IsOpen() )
                    {
                        pTool->FrameStartUpdate( context, viewportManager );
                    }
                }
            }
            break;

            case UpdateStage::FrameEnd:
            {
                m_pModel->Update( context );

                FrameEndUpdate( context, viewportManager );

                for ( auto pTool : m_editorTools )
                {
                    if ( pTool->IsOpen() )
                    {
                        pTool->FrameEndUpdate( context, viewportManager );
                    }
                }

                DrawPopups( context );
            }
            break;
        }

        //-------------------------------------------------------------------------

        // Disable input when we the mouse is not within the editor viewport
        auto pInputSystem = context.GetSystem<Input::InputSystem>();
        KRG_ASSERT( pInputSystem != nullptr );
        pInputSystem->SetEnabled( m_mouseWithinEditorViewport );
    }

    //-------------------------------------------------------------------------

    void Editor::DrawMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        ImGui::TextColored( (Float4) Colors::LimeGreen, KRG_ICON_COG );
    }

    void Editor::DrawEditorMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( ImGui::BeginMainMenuBar() )
        {
            //-------------------------------------------------------------------------
            // Draw Custom Menu
            //-------------------------------------------------------------------------

            DrawMainMenu( context, viewportManager );

            //-------------------------------------------------------------------------
            // Draw Tools Menu
            //-------------------------------------------------------------------------

            if ( ImGui::BeginMenu( "Tools" ) )
            {
                bool windowState = false;

                for ( auto pTool : m_editorTools )
                {
                    windowState = pTool->IsOpen();
                    if ( ImGui::Checkbox( pTool->GetName(), &windowState ) )
                    {
                        pTool->SetOpen( windowState );
                    }
                }

                ImGui::EndMenu();
            }

            //-------------------------------------------------------------------------
            // Draw Performance Stats
            //-------------------------------------------------------------------------

            ImGuiViewport const* viewport = ImGui::GetMainViewport();
            ImGui::SameLine( viewport->WorkSize.x - 165 );

            float const currentFPS = 1.0f / context.GetDeltaTime();
            float const allocatedMemory = Memory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;
            ImGui::Text( "FPS: %3.0f | Mem: %.2fMB", currentFPS, allocatedMemory );

            ImGui::EndMainMenuBar();
        }
    }

    void Editor::DrawEditorDockSpaceAndViewport( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        ImGuiID dockspaceID = ImGui::GetID( "EditorDockSpace" );

        //-------------------------------------------------------------------------
        // Create Dock Space Window
        //-------------------------------------------------------------------------

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        ImGuiViewport const* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos( viewport->WorkPos );
        ImGui::SetNextWindowSize( viewport->WorkSize );
        ImGui::SetNextWindowViewport( viewport->ID );

        ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );

        ImGui::SetNextWindowBgAlpha( 0.0f );
        ImGui::Begin( "EditorDockSpaceWindow", nullptr, windowFlags );
        {
            // Always ensure that the viewport is docked into the central node
            if ( ImGuiDockNode* pCentralNode = ImGui::DockBuilderGetCentralNode( dockspaceID ) )
            {
                pCentralNode->LocalFlags |= ImGuiDockNodeFlags_NoTabBar;
                ImGui::DockBuilderDockWindow( "EditorViewport", pCentralNode->ID );
            }

            // Create the actual dock space
            ImGuiDockNodeFlags const dockSpaceFlags = ImGuiDockNodeFlags_NoDockingInCentralNode;
            ImGui::DockSpace( dockspaceID, viewport->WorkSize, dockSpaceFlags );
        }
        ImGui::PopStyleVar( 3 );
        ImGui::End();

        //-------------------------------------------------------------------------
        // Create viewport window
        //-------------------------------------------------------------------------

        windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse;

        ImGui::SetNextWindowBgAlpha( 0.0f );
        if ( ImGui::Begin( "EditorViewport", nullptr, windowFlags ) )
        {
            ImVec2 const windowPosition = ImGui::GetWindowPos();
            ImVec2 const windowSize = ImGui::GetWindowSize();

            // Update engine viewport dimensions
            //-------------------------------------------------------------------------

            Math::Rectangle const viewportRect( windowPosition, windowSize );
            viewportManager.ResizePrimaryViewport( viewportRect );
            m_mouseWithinEditorViewport = ImGui::IsWindowHovered();

            // Draw orientation guides
            //-------------------------------------------------------------------------

            ImGuiX::DrawOrientationGuide( viewportManager.GetPrimaryViewport() );

            // Draw viewport toolbar
            //-------------------------------------------------------------------------

            if ( HasViewportToolbar() )
            {
                ImGui::SetNextWindowPos( windowPosition );
                ImGui::SetNextWindowBgAlpha( 0.0f );

                ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0 );
                uint32 const viewportToolbarflags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
                if ( ImGui::Begin( "EditorViewportToolbar", nullptr, viewportToolbarflags ) )
                {
                    DrawViewportToolbar( context, viewportManager );
                }
                ImGui::End();
                ImGui::PopStyleVar();
            }
        }
        ImGui::End();
    }

    void Editor::DrawPopups( UpdateContext const& context )
    {
        // Get any new warnings/errors and create pop-ups for them
        //-------------------------------------------------------------------------

        auto const unhandledWarningsAndErrors = Log::GetUnhandledWarningsAndErrors();
        for ( auto const& entry : unhandledWarningsAndErrors )
        {
            auto& popupMessage = m_modalPopups.emplace_back( ModalPopupMessage() );

            UUID const ID = UUID::GenerateID();
            popupMessage.m_ID = ( entry.m_severity == Log::Severity::Warning ) ? "Warning##" : "Error##";
            popupMessage.m_ID += ID.ToString().c_str();
            popupMessage.m_channel = entry.m_channel;
            popupMessage.m_severity = entry.m_severity;
            popupMessage.m_message = entry.m_message;
        }

        // Remove closed warnings/errors
        //-------------------------------------------------------------------------

        for ( auto i = 0; i < m_modalPopups.size(); i++ )
        {
            if ( !m_modalPopups[i].m_isOpen )
            {
                m_modalPopups.erase( m_modalPopups.begin() + i );
                i--;
            }
        }

        // Draw pop-ups
        //-------------------------------------------------------------------------

        if ( !m_modalPopups.empty() )
        {
            // Always draw latest first
            auto& message = m_modalPopups.back();

            ImVec4 const titleBarColor = ( message.m_severity == Log::Severity::Warning ) ? ImGuiX::ConvertColor( Colors::Yellow ) : ImGuiX::ConvertColor( Colors::Red );
            ImVec4 const titleBarTextColor = ( message.m_severity == Log::Severity::Warning ) ? ImGuiX::ConvertColor( Colors::Black ) : ImGuiX::ConvertColor( Colors::Black );

            ImGui::OpenPopup( message.m_ID.c_str() );
            ImGui::PushStyleColor( ImGuiCol_Text, titleBarTextColor );
            ImGui::PushStyleColor( ImGuiCol_TitleBgActive, titleBarColor );
            if ( ImGui::BeginPopupModal( message.m_ID.c_str(), &message.m_isOpen, ImGuiWindowFlags_NoSavedSettings ) )
            {
                ImGui::PopStyleColor( 2 );

                ImGui::BeginGroup();
                {
                    ImGui::PushStyleColor( ImGuiCol_Text, titleBarColor );
                    ImGuiX::ScopedFont font( ImGuiX::Font::ExtraLarge );
                    ImGui::Text( KRG_ICON_EXCLAMATION_TRIANGLE );
                    ImGui::PopStyleColor( 1 );
                }
                ImGui::EndGroup();

                ImGui::SameLine();

                ImGui::BeginGroup();
                {
                    ImGui::Text( "Channel: %s", message.m_channel.c_str() );
                    ImGui::Text( "Message: %s", message.m_message.c_str() );
                }
                ImGui::EndGroup();

                //-------------------------------------------------------------------------

                ImGui::NewLine();
                ImGui::NewLine();
                ImGui::SameLine( ( ImGui::GetWindowWidth() - 100 ) / 2 );
                if ( ImGui::Button( "Ok", ImVec2( 100, 0 ) ) )
                {
                    message.m_isOpen = false;
                }

                //-------------------------------------------------------------------------

                ImGui::EndPopup();
            }
        }
    }
}

//-------------------------------------------------------------------------

namespace KRG
{
    EditorFactory* EditorFactory::s_pHead = nullptr;
    EditorFactory* EditorFactory::s_pTail = nullptr;

    //-------------------------------------------------------------------------

    EditorFactory::EditorFactory()
    {
        // Add to global list
        if ( s_pHead != nullptr )
        {
            s_pTail->m_pNext = this;
            s_pTail = this;
        }
        else
        {
            s_pHead = this;
            s_pTail = this;
        }
    }

    Editor* EditorRegistry::TryCreateEditor( StringID const& editorModeID )
    {
        // No ID, create the first registered editor
        if ( !editorModeID.IsValid() )
        {
            KRG_ASSERT( EditorFactory::s_pHead != nullptr );
            return EditorFactory::s_pHead->CreateEditor();
        }

        // Else search through registered editor modes
        EditorFactory* pEditorModeFactory = EditorFactory::s_pHead;
        while ( pEditorModeFactory != nullptr )
        {
            if ( StringID( pEditorModeFactory->GetID() ) == editorModeID )
            {
                return pEditorModeFactory->CreateEditor();
            }

            pEditorModeFactory = pEditorModeFactory->m_pNext;
        }

        // Nothing found
        return nullptr;
    }
}
