#include "DebugView_Camera.h"
#include "System/Render/Imgui/ImguiX.h"
#include "Engine/Core/Systems/WorldSystem_PlayerManager.h"
#include "Engine/Core/Components/Component_Cameras.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntitySystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "System/Core/Math/MathStringHelpers.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    CameraDebugView::CameraDebugView()
    {
        m_menus.emplace_back( DebugMenu( "Player", [this] ( EntityUpdateContext const& context ) { DrawMenu( context ); } ) );
    }

    void CameraDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        m_pWorld = pWorld;
        m_pPlayerWorldSystem = pWorld->GetWorldSystem<PlayerManager>();
    }

    void CameraDebugView::Shutdown()
    {
        m_pPlayerWorldSystem = nullptr;
        m_pWorld = nullptr;
    }

    void CameraDebugView::DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        KRG_ASSERT( m_pWorld != nullptr );

        if ( m_isCameraDebugWindowOpen )
        {
            if ( pWindowClass != nullptr ) ImGui::SetNextWindowClass( pWindowClass );
            if ( ImGui::Begin( "Camera Debug", &m_isCameraDebugWindowOpen ) )
            {
                DrawCameraWindow( context );
            }
            ImGui::End();
        }
    }

    void CameraDebugView::DrawMenu( EntityUpdateContext const& context )
    {
        if ( ImGui::MenuItem( "Camera" ) )
        {
            m_isCameraDebugWindowOpen = true;
        }
    }
   
    void CameraDebugView::DrawCameraWindow( EntityUpdateContext const& context )
    {
        auto pActivePlayer = m_pPlayerWorldSystem->GetActivePlayer();
        if ( pActivePlayer == nullptr )
        {
            return;
        }

        CameraComponent* pCamera = pActivePlayer->m_pCameraComponent;
        auto pFreeLookCamera = TryCast<FreeLookCameraComponent>( pCamera );
        auto pOrbitCamera = TryCast<OrbitCameraComponent>( pCamera );
        
        if ( pFreeLookCamera != nullptr )
        {
            ImGui::Text( "Camera Type: Free Look" );
            ImGui::Text( "Pitch: %.2f deg", pFreeLookCamera->m_pitch.ToDegrees().ToFloat() );
            ImGui::Text( "Yaw: %.2f deg ", pFreeLookCamera->m_yaw.ToDegrees().ToFloat() );
        }
        else if( pOrbitCamera != nullptr )
        {
            ImGui::Text( "Camera Type: Orbit" );
            ImGui::Text( "Orbit Offset: %s", Math::ToString( pOrbitCamera->m_orbitTargetOffset ).c_str() );
            ImGui::Text( "Orbit Distance: %.2fm", pOrbitCamera->m_orbitDistance );
        }

        ImGui::Separator();

        auto const& camTransform = pCamera->GetWorldTransform();
        ImGui::Text( "Forward: %s", Math::ToString( camTransform.GetForwardVector() ).c_str() );
        ImGui::Text( "Right: %s", Math::ToString( camTransform.GetRightVector() ).c_str() );
        ImGui::Text( "Up: %s", Math::ToString( camTransform.GetUpVector() ).c_str() );
        ImGui::Text( "Pos: %s", Math::ToString( camTransform.GetTranslation() ).c_str() );

        ImGui::Separator();

        if ( ImGui::Button( "Copy Camera Transform", ImVec2( -1, 0 ) ) )
        {
            auto const& camTranslation = camTransform.GetTranslation();
            auto const camRotation = camTransform.GetRotation().ToEulerAngles().GetAsDegrees();

            String const camTransformStr( String::CtorSprintf(), "%.2f, %.2f, %.2f, %.2f, %.2f, %.2f, 1.0f, 1.0f, 1.0f", camRotation.m_x, camRotation.m_y, camRotation.m_z, camTranslation.m_x, camTranslation.m_y, camTranslation.m_z );
            ImGui::SetClipboardText( camTransformStr.c_str() );
        }
    }
}
#endif