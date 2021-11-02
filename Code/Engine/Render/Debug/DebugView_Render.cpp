#include "DebugView_Render.h"
#include "Engine/Render/WorldSystems/WorldRendererSystem.h"
#include "Engine/Render/Components/LightComponents.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Render
{
    RenderDebugView::RenderDebugView()
    {
        m_menus.emplace_back( DebugMenu( "Debug Options", "Render", [this] ( EntityUpdateContext const& context ) { DrawRenderMenu( context ); } ) );
    }

    void RenderDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        m_pWorldRendererSystem = pWorld->GetWorldSystem<WorldRendererSystem>();
        m_gizmo.SetTargetTransform( &m_componentTransform );
    }

    void RenderDebugView::Shutdown()
    {
        m_gizmo.SetTargetTransform( nullptr );
        m_pWorldRendererSystem = nullptr;
    }

    //-------------------------------------------------------------------------

    void RenderDebugView::DrawRenderMenu( EntityUpdateContext const& context )
    {
        if ( ImGui::Button( "Show Light Debugger" ) )
        {
            m_isLightDebuggerWindowOpen = true;
        }
    }

    void RenderDebugView::DrawWindows( EntityUpdateContext const& context )
    {
        if ( m_isLightDebuggerWindowOpen )
        {
            DrawLightDebuggerWindow( context );
        }
        else
        {
            m_pSelectedComponent = nullptr;
        }
    }

    void RenderDebugView::DrawOverlayElements( EntityUpdateContext const& context )
    {
        if ( !m_isLightDebuggerWindowOpen )
        {
            return;
        }

        //-------------------------------------------------------------------------

        auto pOverlayDrawList = ImGui::GetWindowDrawList();
        auto pViewport = context.GetViewport();

        //-------------------------------------------------------------------------

        auto DrawLightButton = [this, &pViewport, &pOverlayDrawList] ( LightComponent* pLightComponent )
        {
            ImVec2 const iconSize( 36, 46 );
            ImVec2 const buttonOffset( iconSize.x / 2, iconSize.y );
            ImVec2 const lightPositionScreenSpace = pViewport->WorldSpaceToScreenSpace( pLightComponent->GetPosition() );

            {
                ImGuiX::ScopedFont scopedFont( ImGuiX::Font::ExtraLarge );
                ImVec2 const textSize = ImGui::CalcTextSize( KRG_ICON_LIGHTBULB_O );
                ImGui::SetCursorPos( lightPositionScreenSpace - buttonOffset );
                ImGui::PushID( pLightComponent );
                ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 2, 2 ) );
                ImGui::PushStyleVar( ImGuiStyleVar_ButtonTextAlign, ImVec2( 0.5f, 1.0f ) );
                ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );
                if ( ImGui::Button( KRG_ICON_LIGHTBULB_O, iconSize ) )
                {
                    if ( m_pSelectedComponent == pLightComponent )
                    {
                        m_gizmo.SwitchToNextMode();
                    }
                    else
                    {
                        m_pSelectedComponent = pLightComponent;
                    }
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleVar( 2 );
                ImGui::PopID();
            }
        };

        //-------------------------------------------------------------------------

        for ( auto& lightComponentRecord : m_pWorldRendererSystem->m_registeredDirectionLightComponents.GetRecords() )
        {
            DrawLightButton( lightComponentRecord.m_pComponent );
        }

        for ( auto& lightComponentRecord : m_pWorldRendererSystem->m_registeredPointLightComponents.GetRecords() )
        {
            DrawLightButton( lightComponentRecord.m_pComponent );
        }

        for ( auto& lightComponentRecord : m_pWorldRendererSystem->m_registeredSpotLightComponents.GetRecords() )
        {
            DrawLightButton( lightComponentRecord.m_pComponent );
        }

        // Draw editing gizmo
        if ( m_pSelectedComponent != nullptr )
        {
            m_componentTransform = m_pSelectedComponent->GetWorldTransform();
            m_gizmo.Draw( *pViewport );
            m_pSelectedComponent->SetWorldTransform( m_componentTransform );
        }
    }

    //-------------------------------------------------------------------------

    void RenderDebugView::DrawLightDebuggerWindow( EntityUpdateContext const& context )
    {
        ImGui::SetNextWindowBgAlpha( 0.5f );
        if ( ImGui::Begin( "World", &m_isLightDebuggerWindowOpen ) )
        {
            bool selectedComponentFound = false;

            ImGui::SetNextWindowBgAlpha( 0.5f );
            if ( ImGui::BeginChild( "Light Components", ImVec2( 200, -1 ), true, ImGuiWindowFlags_AlwaysUseWindowPadding ) )
            {
                ImGui::Text( "Lights" );
                ImGui::Separator();

                for ( auto& lightComponentRecord : m_pWorldRendererSystem->m_registeredDirectionLightComponents.GetRecords() )
                {
                    DirectionalLightComponent* pLightComponent = lightComponentRecord.m_pComponent;
                    if ( ImGui::Selectable( pLightComponent->GetName().c_str() ) )
                    {
                        m_pSelectedComponent = pLightComponent;
                    }

                    if ( pLightComponent == m_pSelectedComponent )
                    {
                        selectedComponentFound = true;
                    }
                }

                for ( auto& lightComponentRecord : m_pWorldRendererSystem->m_registeredPointLightComponents.GetRecords() )
                {
                    PointLightComponent* pLightComponent = lightComponentRecord.m_pComponent;
                    if ( ImGui::Selectable( pLightComponent->GetName().c_str() ) )
                    {
                        m_pSelectedComponent = pLightComponent;
                    }

                    if ( pLightComponent == m_pSelectedComponent )
                    {
                        selectedComponentFound = true;
                    }
                }

                for ( auto& lightComponentRecord : m_pWorldRendererSystem->m_registeredSpotLightComponents.GetRecords() )
                {
                    SpotLightComponent* pLightComponent = lightComponentRecord.m_pComponent;
                    if ( ImGui::Selectable( pLightComponent->GetName().c_str() ) )
                    {
                        m_pSelectedComponent = pLightComponent;
                    }

                    if ( pLightComponent == m_pSelectedComponent )
                    {
                        selectedComponentFound = true;
                    }
                }
            }
            ImGui::EndChild();

            // If we didnt find the selected component (i.e. it was destroyed), clear the ptr
            if ( !selectedComponentFound )
            {
                m_pSelectedComponent = nullptr;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine();

            //-------------------------------------------------------------------------

            ImGui::SetNextWindowBgAlpha( 0.5f );
            if ( ImGui::BeginChild( "Light Details", ImVec2( -1, -1 ), true, ImGuiWindowFlags_AlwaysUseWindowPadding ) )
            {
                if ( m_pSelectedComponent == nullptr )
                {
                    ImGui::Text( "No light selected!" );
                }
                else
                {
                    ImGui::Text( m_pSelectedComponent->GetName().c_str() );
                    ImGui::Separator();

                    //-------------------------------------------------------------------------

                    ImVec4 color = m_pSelectedComponent->m_color.ToFloat4();
                    if ( ImGui::ColorEdit4( "Color", &color.x, 0 ) )
                    {
                        m_pSelectedComponent->m_color = Color( color );
                    }

                    float intensity = m_pSelectedComponent->m_intensity;
                    ImGui::InputFloat( "Intensity", &m_pSelectedComponent->m_intensity, 0 );

                    //-------------------------------------------------------------------------

                    if ( IsOfType<DirectionalLightComponent>( m_pSelectedComponent ) )
                    {
                        DrawDirectionalLightOptions( context );
                    }
                    else if( IsOfType<SpotLightComponent>( m_pSelectedComponent ) )
                    {
                        DrawSpotLightOptions( context );
                    }
                    else if ( IsOfType<PointLightComponent>( m_pSelectedComponent ) )
                    {
                        DrawPointLightOptions( context );
                    }
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();

        // Draw 3D controls
        //-------------------------------------------------------------------------

        if ( m_pSelectedComponent != nullptr )
        {
            auto drawingCtx = context.GetDrawingContext();

            if ( IsOfType<DirectionalLightComponent>( m_pSelectedComponent ) )
            {
                auto pLightComponent = Cast<DirectionalLightComponent>( m_pSelectedComponent );
                auto forwardDir = pLightComponent->GetForwardVector();
                drawingCtx.DrawSphere( pLightComponent->GetPosition(), Float3( 0.1f ), pLightComponent->m_color, 2.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + forwardDir, pLightComponent->m_color, 3.0f );
            }
            else if ( IsOfType<SpotLightComponent>( m_pSelectedComponent ) )
            {
                auto pLightComponent = Cast<SpotLightComponent>( m_pSelectedComponent );
                drawingCtx.DrawSphere( pLightComponent->GetPosition(), Float3( 0.1f ), pLightComponent->m_color, 2.0f );
                drawingCtx.DrawCone( pLightComponent->GetWorldTransform(), pLightComponent->m_umbraAngle, 1.5f, pLightComponent->m_color, 3.0f );
            }
            else if ( IsOfType<PointLightComponent>( m_pSelectedComponent ) )
            {
                auto pLightComponent = Cast<PointLightComponent>( m_pSelectedComponent );
                auto forwardDir = pLightComponent->GetForwardVector();
                auto upDir = pLightComponent->GetUpVector();
                auto rightDir = pLightComponent->GetRightVector();

                drawingCtx.DrawSphere( pLightComponent->GetPosition(), Float3( 0.1f ), pLightComponent->m_color, 2.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + ( forwardDir * 0.5f ), pLightComponent->m_color, 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() - ( forwardDir * 0.5f ), pLightComponent->m_color, 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + ( upDir * 0.5f ), pLightComponent->m_color, 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() - ( upDir * 0.5f ), pLightComponent->m_color, 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() + ( rightDir * 0.5f ), pLightComponent->m_color, 3.0f );
                drawingCtx.DrawArrow( pLightComponent->GetPosition(), pLightComponent->GetPosition() - ( rightDir * 0.5f ), pLightComponent->m_color, 3.0f );
            }
        }
    }

    //-------------------------------------------------------------------------

    void RenderDebugView::DrawDirectionalLightOptions( EntityUpdateContext const& context )
    {
        auto pLightComponent = Cast<DirectionalLightComponent>( m_pSelectedComponent );
    }

    void RenderDebugView::DrawSpotLightOptions( EntityUpdateContext const& context )
    {
        auto pLightComponent = Cast<SpotLightComponent>( m_pSelectedComponent );

        float angleDegrees = pLightComponent->m_umbraAngle.ToFloat();
        if ( ImGui::SliderFloat( "Umbra Angle", &angleDegrees, 5.0f, 85.0f ) )
        {
            pLightComponent->m_umbraAngle = angleDegrees;
        }
    }

    void RenderDebugView::DrawPointLightOptions( EntityUpdateContext const& context )
    {
        auto pLightComponent = Cast<PointLightComponent>( m_pSelectedComponent );
    }
}
#endif