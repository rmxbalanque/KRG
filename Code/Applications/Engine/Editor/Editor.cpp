#include "Editor.h"
#include "System/Imgui/Gizmo/TranslationGizmo.h"
#include "System/Imgui/Gizmo/ScaleGizmo.h"
#include "System/Imgui/Gizmo/RotationGizmo.h"
#include "System/Core/Math/Viewport.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Memory/Memory.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Editor
    {
        namespace
        {
            static float const g_windowPadding = 4.0f;
            static float const g_windowFramePadding = 4.0f;
            static float const g_windowRounding = 4.0f;
            static float const g_windowMarginX = 3.0f;
            static float const g_windowMarginY = 4.0f;
        }

        namespace OrientationGuide
        {
            static float const g_axisHalfLength = 25.0f;
            static float const g_worldRenderDistanceZ = 5.0f;
            static float const g_axisThickness = 2.0f;
            static float const g_axisHeadRadius = 4.0f;
        }

        //-------------------------------------------------------------------------

        TempController::TempController( World::EditorWorld* pWorld )
            : m_pWorld( pWorld )
        {}

        TempController::~TempController()
        {
            KRG::Delete( m_pGizmo );
        }

        void TempController::StartFrame( UpdateContext const& context, Math::Viewport const& viewport )
        {
            //// HACK
            //if ( m_pWorld->HasLoadedMaps() && m_pObjectBeingManipulated == nullptr )
            //{
            //    auto pTypeInfo = EditorTestObject::StaticTypeInfo;

            //    m_pObjectBeingManipulated = m_pWorld->FindObject( EntityID( "d076af38-d2cc-4f5f-bba5-0719902ea689" ) );
            //    if ( m_pObjectBeingManipulated != nullptr )
            //    {
            //        m_pGizmo = KRG::New<ImGuiX::TranslationGizmo>( m_pObjectBeingManipulated->GetModifiableTransform() );
            //        m_manipulationMode = ManipulationMode::Translate;
            //    }
            //}

            //HACK_Test( context );
            ////end hack

            ////-------------------------------------------------------------------------

            //DrawEditorUI( context, viewport );

            ////-------------------------------------------------------------------------

            //if ( m_manipulationMode != ManipulationMode::None )
            //{
            //    KRG_ASSERT( m_pGizmo != nullptr );
            //    m_pGizmo->Update( viewport );
            //}
        }

        void TempController::EndFrame( UpdateContext const& context )
        {

        }

        void TempController::DrawEditorUI( UpdateContext const& context, Math::Viewport const& viewport )
        {
            DrawOrientationGuide( context, viewport );
            DrawManipulationToolbar( context, viewport);
            DrawHackToolbar( context, viewport );
        }

        void TempController::DrawManipulationToolbar( UpdateContext const& context, Math::Viewport const& viewport )
        {
            //ImGuiIO& io = ImGui::GetIO();
            //static bool alwaysOpen = true;

            //ImGui::SetNextWindowPos( ImVec2( g_windowMarginX, g_windowMarginY ) );
            //ImGui::SetNextWindowBgAlpha( 1.0f );

            //ImGui::PushStyleVar( ImGuiStyleVar_WindowMinSize, ImVec2( 0.0f, 0.0f ) );
            //ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( g_windowPadding, g_windowPadding ) );
            //ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, g_windowRounding );
            //uint32 const flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
            //if ( ImGui::Begin( "Editor_ManipulationToolbar", &alwaysOpen, flags ) )
            //{
            //    {
            //        ImGuiX::ScopedFont scopedFont( ImGuiX::FontType::Small );
            //        ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( g_windowFramePadding, g_windowFramePadding ) );

            //        //-------------------------------------------------------------------------

            //        // Translation
            //        if ( ImGui::RadioButton( Font::FontAwesome4::ICON_ARROWS, m_manipulationMode == ManipulationMode::Translate ) )
            //        {
            //            if ( m_pObjectBeingManipulated != nullptr )
            //            {
            //                KRG::Delete( m_pGizmo );
            //                m_pGizmo = KRG::New<ImGuiX::TranslationGizmo>( m_pObjectBeingManipulated->GetModifiableTransform() );
            //                m_manipulationMode = ManipulationMode::Translate;
            //            }
            //        }
            //        if ( ImGui::IsItemHovered() )
            //        {
            //            ImGui::SetTooltip( "Translate" );
            //        }
            //        ImGui::SameLine();

            //        // Rotation
            //        if ( ImGui::RadioButton( Font::FontAwesome4::ICON_REPEAT, m_manipulationMode == ManipulationMode::Rotate ) )
            //        {
            //            if ( m_pObjectBeingManipulated != nullptr )
            //            {
            //                KRG::Delete( m_pGizmo );
            //                m_pGizmo = KRG::New<ImGuiX::RotationGizmo>( m_pObjectBeingManipulated->GetModifiableTransform() );
            //                m_manipulationMode = ManipulationMode::Rotate;
            //            }
            //        }
            //        if ( ImGui::IsItemHovered() )
            //        {
            //            ImGui::SetTooltip( "Rotate" );
            //        }
            //        ImGui::SameLine();

            //        // Scaling
            //        if ( ImGui::RadioButton( Font::FontAwesome4::ICON_EXPAND, m_manipulationMode == ManipulationMode::Scale ) )
            //        {
            //            if ( m_pObjectBeingManipulated != nullptr )
            //            {
            //                KRG::Delete( m_pGizmo );
            //                m_pGizmo = KRG::New<ImGuiX::ScalingGizmo>( m_pObjectBeingManipulated->GetModifiableTransform() );
            //                m_manipulationMode = ManipulationMode::Scale;
            //            }
            //        }
            //        if ( ImGui::IsItemHovered() )
            //        {
            //            ImGui::SetTooltip( "Scale" );
            //        }
            //        ImGui::SameLine();

            //        //-------------------------------------------------------------------------

            //        if ( m_manipulationMode != ManipulationMode::None )
            //        {
            //            KRG_ASSERT( m_pGizmo != nullptr );

            //            ImGui::Separator();
            //            ImGui::SameLine();

            //            //-------------------------------------------------------------------------

            //            if ( ImGui::RadioButton( Font::FontAwesome4::ICON_GLOBE, m_pGizmo->IsInWorldSpace() ) )
            //            {
            //                m_pGizmo->SwitchToWorldSpace();
            //            }
            //            if ( ImGui::IsItemHovered() )
            //            {
            //                ImGui::SetTooltip( "World Space" );
            //            }
            //            ImGui::SameLine();

            //            if ( ImGui::RadioButton( Font::FontAwesome4::ICON_CROSSHAIRS, m_pGizmo->IsInObjectSpace() ) )
            //            {
            //                m_pGizmo->SwitchToObjectSpace();
            //            }

            //            if ( ImGui::IsItemHovered() )
            //            {
            //                ImGui::SetTooltip( "Local Space" );
            //            }
            //            ImGui::SameLine();

            //            //-------------------------------------------------------------------------

            //            ImGui::Separator();
            //            ImGui::SameLine();

            //            //-------------------------------------------------------------------------

            //            if ( m_manipulationMode == ManipulationMode::Rotate )
            //            {
            //                auto const rotation = m_pGizmo->GetTransform().GetRotation().ToEulerAngles();
            //                ImGui::Text( "R: %.2f, %.2f, %.2f", (float) rotation.x.ToDegrees(), (float) rotation.y.ToDegrees(), (float) rotation.z.ToDegrees() );
            //            }
            //            else if ( m_manipulationMode == ManipulationMode::Translate )
            //            {
            //                auto const translation = m_pGizmo->GetTransform().GetTranslation();
            //                ImGui::Text( "T: %.2f, %.2f, %.2f", translation.x, translation.y, translation.z );
            //            }
            //            else
            //            {
            //                auto const scale = m_pGizmo->GetTransform().GetScale();
            //                ImGui::Text( "S: %.2f, %.2f, %.2f", scale.x, scale.y, scale.z );
            //            }
            //        }

            //        //-------------------------------------------------------------------------

            //        ImGui::PopStyleVar();
            //    }
            //    ImGui::End();
            //}

            //ImGui::PopStyleVar( 3 );
        }

        void TempController::DrawOrientationGuide( UpdateContext const& context, Math::Viewport const& viewport )
        {
            //ImGuiIO& io = ImGui::GetIO();
            //static bool alwaysOpen = true;

            //Float2 const windowDimensions( 75, 75 );
            //Float2 const windowPosition = ImVec2( g_windowMarginX, io.DisplaySize.y - windowDimensions.y - g_windowMarginY );
            //Float2 const windowCenter = Float2( windowPosition.x + windowDimensions.x / 2, windowPosition.y + windowDimensions.y / 2 );

            //ImGui::SetNextWindowSize( windowDimensions );
            //ImGui::SetNextWindowPos( windowPosition );
            //ImGui::SetNextWindowBgAlpha( 0.0f );

            //ImGui::PushStyleVar( ImGuiStyleVar_WindowMinSize, ImVec2( 0.0f, 0.0f ) );
            //ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( g_windowPadding, g_windowPadding ) );
            //ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, g_windowRounding );
            //uint32 const flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
            //if ( ImGui::Begin( "Editor_OrientationGuide", &alwaysOpen, flags ) )
            //{
            //    // Project world space axis positions to screen space
            //    //-------------------------------------------------------------------------

            //    Vector const& originWS = viewport.GetViewOrigin() + viewport.GetViewForwardDirection() * OrientationGuide::g_worldRenderDistanceZ;
            //    Vector const& worldAxisX = ( Vector::UnitX );
            //    Vector const& worldAxisY = ( Vector::UnitY );
            //    Vector const& worldAxisZ = ( Vector::UnitZ );

            //    Vector const& worldAxisForwardPointX = ( originWS + worldAxisX );
            //    Vector const& worldAxisForwardPointY = ( originWS + worldAxisY );
            //    Vector const& worldAxisForwardPointZ = ( originWS + worldAxisZ );
            //    Vector const& worldAxisBackwardPointX = ( originWS - worldAxisX );
            //    Vector const& worldAxisBackwardPointY = ( originWS - worldAxisY );
            //    Vector const& worldAxisBackwardPointZ = ( originWS - worldAxisZ );

            //    Vector const axisStartPointX = Vector( viewport.WorldSpaceToScreenSpace( worldAxisBackwardPointX ) );
            //    Vector const axisStartPointY = Vector( viewport.WorldSpaceToScreenSpace( worldAxisBackwardPointY ) );
            //    Vector const axisStartPointZ = Vector( viewport.WorldSpaceToScreenSpace( worldAxisBackwardPointZ ) );
            //    Vector const axisEndPointX = Vector( viewport.WorldSpaceToScreenSpace( worldAxisForwardPointX ) );
            //    Vector const axisEndPointY = Vector( viewport.WorldSpaceToScreenSpace( worldAxisForwardPointY ) );
            //    Vector const axisEndPointZ = Vector( viewport.WorldSpaceToScreenSpace( worldAxisForwardPointZ ) );

            //    // Calculate screen space axis lengths
            //    //-------------------------------------------------------------------------

            //    float const axisLengthX = axisStartPointX.GetDistance2( axisEndPointX );
            //    float const axisLengthY = axisStartPointY.GetDistance2( axisEndPointY );
            //    float const axisLengthZ = axisStartPointZ.GetDistance2( axisEndPointZ );
            //    float const maxAxisLength = Math::Max( axisLengthX, Math::Max( axisLengthY, axisLengthZ ) );

            //    static float const axisHalfLengthSS = OrientationGuide::g_axisHalfLength;
            //    float const axisScaleX = ( axisLengthX / maxAxisLength ) * axisHalfLengthSS;
            //    float const axisScaleY = ( axisLengthY / maxAxisLength ) * axisHalfLengthSS;
            //    float const axisScaleZ = ( axisLengthZ / maxAxisLength ) * axisHalfLengthSS;

            //    // Calculate screen space axis directions
            //    Vector const origin = viewport.WorldSpaceToScreenSpace( originWS );
            //    Vector const axisDirX = ( axisEndPointX - origin ).GetNormalized2();
            //    Vector const axisDirY = ( axisEndPointY - origin ).GetNormalized2();
            //    Vector const axisDirZ = ( axisEndPointZ - origin ).GetNormalized2();

            //    // Sort SS axis and draw them in the correct order
            //    //-------------------------------------------------------------------------

            //    struct AxisDrawRequest { Axis m_axis; bool m_isInForwardDirection; float m_distance; };
            //    TInlineVector<AxisDrawRequest, 3> drawRequests;

            //    Plane const NearPlane = viewport.GetViewVolume().GetViewPlane( Math::ViewVolume::PlaneID::Near );

            //    drawRequests.push_back( { Axis::X, true, NearPlane.GetAbsoluteDistanceToPoint( worldAxisForwardPointX ) } );
            //    drawRequests.push_back( { Axis::Y, true, NearPlane.GetAbsoluteDistanceToPoint( worldAxisForwardPointY ) } );
            //    drawRequests.push_back( { Axis::Z, true, NearPlane.GetAbsoluteDistanceToPoint( worldAxisForwardPointZ ) } );
            //    drawRequests.push_back( { Axis::X, false, NearPlane.GetAbsoluteDistanceToPoint( worldAxisBackwardPointX ) } );
            //    drawRequests.push_back( { Axis::Y, false, NearPlane.GetAbsoluteDistanceToPoint( worldAxisBackwardPointY ) } );
            //    drawRequests.push_back( { Axis::Z, false, NearPlane.GetAbsoluteDistanceToPoint( worldAxisBackwardPointZ ) } );

            //    eastl::sort( drawRequests.begin(), drawRequests.end(), [] ( AxisDrawRequest const& lhs, AxisDrawRequest const& rhs ) { return lhs.m_distance > rhs.m_distance; } );

            //    //-------------------------------------------------------------------------

            //    auto pDrawList = ImGui::GetWindowDrawList();
            //    for ( auto const& request : drawRequests )
            //    {
            //        // X
            //        if ( request.m_axis == Axis::X && request.m_isInForwardDirection )
            //        {
            //            pDrawList->AddLine( windowCenter, windowCenter + axisDirX * ( axisScaleX - OrientationGuide::g_axisHeadRadius + 1.0f ), 0xBB0000FF, OrientationGuide::g_axisThickness );
            //            pDrawList->AddCircleFilled( windowCenter + axisDirX * axisScaleX, OrientationGuide::g_axisHeadRadius, 0xBB0000FF );
            //        }
            //        else if ( request.m_axis == Axis::X && !request.m_isInForwardDirection )
            //        {
            //            pDrawList->AddCircleFilled( windowCenter - axisDirX * axisScaleX, OrientationGuide::g_axisHeadRadius, 0x660000FF );
            //        }
            //        //Y
            //        else if ( request.m_axis == Axis::Y && request.m_isInForwardDirection )
            //        {
            //            pDrawList->AddLine( windowCenter, windowCenter + axisDirY * ( axisScaleY - OrientationGuide::g_axisHeadRadius + 1.0f ), 0xBB00FF00, OrientationGuide::g_axisThickness );
            //            pDrawList->AddCircleFilled( windowCenter + axisDirY * axisScaleY, OrientationGuide::g_axisHeadRadius, 0xBB00FF00 );
            //        }
            //        else if ( request.m_axis == Axis::Y && !request.m_isInForwardDirection )
            //        {
            //            pDrawList->AddCircleFilled( windowCenter - axisDirY * axisScaleY, OrientationGuide::g_axisHeadRadius, 0x6600FF00 );
            //        }
            //        // Z
            //        else if ( request.m_axis == Axis::Z && request.m_isInForwardDirection )
            //        {
            //            pDrawList->AddLine( windowCenter, windowCenter + axisDirZ * ( axisScaleZ - OrientationGuide::g_axisHeadRadius + 1.0f ), 0xBBFF0000, OrientationGuide::g_axisThickness );
            //            pDrawList->AddCircleFilled( windowCenter + axisDirZ * axisScaleZ, OrientationGuide::g_axisHeadRadius, 0xBBFF0000 );
            //        }
            //        else if ( request.m_axis == Axis::Z && !request.m_isInForwardDirection )
            //        {
            //            pDrawList->AddCircleFilled( windowCenter - axisDirZ * axisScaleZ, OrientationGuide::g_axisHeadRadius, 0x66FF0000 );
            //        }
            //    }

            //    ImGui::End();
            //}
            //ImGui::PopStyleVar( 3 );
        }

        void TempController::DrawHackToolbar( UpdateContext const& context, Math::Viewport const& viewport )
        {
            /*ImGuiIO& io = ImGui::GetIO();
            static bool alwaysOpen = true;
            static UUID newObjectID = UUID::GenerateID();

            ImVec2 const toolbarDimensions( 600, 0 );

            ImGui::SetNextWindowPos( ImVec2( io.DisplaySize.x - toolbarDimensions.x - g_windowMarginX, g_windowMarginY ) );
            ImGui::SetNextWindowBgAlpha( 1.0f );

            ImGui::PushStyleVar( ImGuiStyleVar_WindowMinSize, ImVec2( 0.0f, 0.0f ) );
            ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( g_windowPadding, g_windowPadding ) );
            ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, g_windowRounding );
            uint32 const flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings;
            if ( ImGui::Begin( "Editor_TopRightToolBar", &alwaysOpen, flags ) )
            {
                {
                    ImGuiX::ScopedFont scopedFont( ImGuiX::FontType::Small );
                    ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( g_windowFramePadding, g_windowFramePadding ) );

                    ImGui::InputText( "EntityID: ", m_hackBuffer, 256 );
                    ImGui::SameLine();
                    if ( ImGui::Button( "Select Entity" ) )
                    {
                        if ( m_pGizmo != nullptr )
                        {
                            KRG::Delete( m_pGizmo );
                            m_manipulationMode = ManipulationMode::None;
                        }

                        if ( m_pWorld->HasLoadedMaps() )
                        {
                            m_pObjectBeingManipulated = m_pWorld->FindObject( EntityID( m_hackBuffer ) );
                            if ( m_pObjectBeingManipulated != nullptr )
                            {
                                m_pGizmo = KRG::New<ImGuiX::RotationGizmo>( m_pObjectBeingManipulated->GetModifiableTransform() );
                                m_manipulationMode = ManipulationMode::Translate;
                            }
                        }
                    }

                    ImGui::PopStyleVar();
                }
                ImGui::End();
            }

            ImGui::PopStyleVar( 3 );*/
        }
    }
}