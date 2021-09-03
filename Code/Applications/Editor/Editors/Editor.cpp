#include "Editor.h"
#include "System/DevTools/CommonWidgets/Gizmo/OrientationGuide.h"
#include "System/Render/RenderViewportManager.h"

#include "System/Input/InputSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Editor::~Editor()
    {
        KRG::Delete( m_pModel );
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

    void Editor::DrawMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        // Needed to ensure that the perf stats get drawn correctly
        ImGui::NewLine();
    }

    void Editor::DrawViewportWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, char const* const pEditorViewportName )
    {
        // Request that game is rendered to texture
        viewportManager.SetUseCustomRenderTargetForViewport( 0, true );

        // Create viewport window
        ImGui::SetNextWindowSizeConstraints( ImVec2( 128, 128 ), ImVec2( FLT_MAX, FLT_MAX ) );
        if ( ImGui::Begin( pEditorViewportName, nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar ) )
        {
            ImGuiStyle const& style = ImGui::GetStyle();
            ImVec2 const windowPosition = ImGui::GetWindowPos();
            ImVec2 const windowSize = ImGui::GetWindowSize();
            ImVec2 const viewportSize( Math::Max( ImGui::GetContentRegionAvail().x, 64.0f ), Math::Max( ImGui::GetContentRegionAvail().y, 64.0f ) );

            // Update engine viewport dimensions
            //-------------------------------------------------------------------------

            Math::Rectangle const viewportRect( Float2::Zero, viewportSize );
            viewportManager.ResizePrimaryViewport( viewportRect );
            m_mouseWithinEditorViewport = ImGui::IsWindowHovered();

            // Draw 3D scene
            //-------------------------------------------------------------------------

            ImTextureID vpTextureID = (void*) &viewportManager.GetViewportRenderTargetTextureSRV( 0 );
            ImGui::Image( vpTextureID, viewportSize );

            // Draw orientation guides
            //-------------------------------------------------------------------------

            Float2 const orientationGuidePosition( 4, viewportSize.y - ImGuiX::OrientationGuide::GetRequiredDimensions().m_y );
            ImGui::SetCursorPos( orientationGuidePosition );
            ImGuiX::OrientationGuide::DrawAsChildWindow( *viewportManager.GetPrimaryViewport() );

            // Draw viewport toolbar
            //-------------------------------------------------------------------------

            if ( HasViewportToolbar() )
            {
                ImGui::SetCursorPos( style.WindowPadding + ImVec2( 4, 4 ) );

                ImGui::SetNextWindowBgAlpha( 0.0f );
                if ( ImGui::BeginChild( "EditorViewportToolbar", ImVec2(0, 0), 0 ) )
                {
                    DrawViewportToolbar( context, viewportManager );
                }
                ImGui::EndChild();
            }
        }
        ImGui::End();
    }

    void Editor::Update( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        UpdateStage const updateStage = context.GetUpdateStage();

        switch ( updateStage )
        {
            case UpdateStage::FrameStart:
            {
                m_pModel->Update( context );

                if ( ImGui::BeginMainMenuBar() )
                {
                    DrawMainMenu( context, viewportManager );

                    // Draw Performance Stats
                    //-------------------------------------------------------------------------

                    ImGuiViewport const* viewport = ImGui::GetMainViewport();
                    ImGui::SameLine( viewport->WorkSize.x - 165 );

                    float const currentFPS = 1.0f / context.GetDeltaTime();
                    float const allocatedMemory = Memory::GetTotalAllocatedMemory() / 1024.0f / 1024.0f;
                    ImGui::Text( "FPS: %3.0f | Mem: %.2fMB", currentFPS, allocatedMemory );

                    ImGui::EndMainMenuBar();
                }

                FrameStartUpdate( context, viewportManager );
            }
            break;

            case UpdateStage::FrameEnd:
            {
                m_pModel->Update( context );

                FrameEndUpdate( context, viewportManager );
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
