#include "InputDebugViewController.h"
#include "System/Imgui/ImguiCore.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Input/InputSystem.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace Input
    {
        namespace
        {
            static U32 const g_controlOutlineColor = 0xFF666666;
            static U32 const g_controlFillColor = 0xFF888888;

            static F32 const g_buttonWidth = 20;
            static F32 const g_buttonBorderThickness = 2.0f;
            static F32 const g_buttonBorderRounding = 4.0f;

            static F32 const g_analogStickRangeRadius = 50;
            static F32 const g_analogStickPositionRadius = 2;

            static Float2 const g_buttonBorderOffset( g_buttonBorderThickness );
            static Float2 const g_buttonDimensions( g_buttonWidth, g_buttonWidth );
        }

        static void DrawButton( ImDrawList* pDrawList, Float2 const& position, Float2 const& dimensions, char const* const pLabel, bool IsHeldDown, U32 buttonColor = g_controlOutlineColor, U32 pressedColor = g_controlFillColor )
        {
            KRG_ASSERT( pDrawList != nullptr );
            Float2 const buttonTopLeft = position;
            Float2 const buttonBottomRight = buttonTopLeft + dimensions;
            pDrawList->AddRect( buttonTopLeft, buttonBottomRight, buttonColor, g_buttonBorderRounding, ~0, g_buttonBorderThickness );

            if ( IsHeldDown )
            {
                pDrawList->AddRectFilled( buttonTopLeft + g_buttonBorderOffset, buttonTopLeft + dimensions - g_buttonBorderOffset, pressedColor, g_buttonBorderRounding - 1, ~0 );
            }

            if ( pLabel != nullptr )
            {
                Float2 const textDimensions = ImGui::CalcTextSize( pLabel );
                Float2 const textPos = Float2( buttonTopLeft.x + ( dimensions.x / 2 ) - ( textDimensions.x / 2 ) + 1, buttonTopLeft.y + ( dimensions.y / 2 ) - ( textDimensions.y / 2 ) );
                pDrawList->AddText( textPos, 0xFFFFFFFF, pLabel );
            }
        }

        static void DrawTriggerButton( ImDrawList* pDrawList, Float2 const& position, Float2 const& dimensions, char const* const pLabel, ControllerInputState const& controllerState, bool isLeftTrigger )
        {
            KRG_ASSERT( pDrawList != nullptr );

            // Draw the label if set
            Float2 drawPosition = position;
            if ( pLabel != nullptr )
            {
                Float2 const textDimensions = ImGui::CalcTextSize( pLabel );
                Float2 const textPos = Float2( drawPosition.x + ( dimensions.x / 2 ) - ( textDimensions.x / 2 ) + 1, drawPosition.y + g_buttonBorderThickness );
                pDrawList->AddText( textPos, 0xFFFFFFFF, pLabel );
                drawPosition.y += textDimensions.y + 4;
            }

            // Draw the border
            Float2 const borderDimensions( dimensions.x, dimensions.y - ( drawPosition.y - position.y + 4 ) );
            Float2 const triggerTopLeft = drawPosition;
            Float2 const triggerBottomRight = triggerTopLeft + borderDimensions;
            pDrawList->AddRect( triggerTopLeft, triggerBottomRight, g_controlOutlineColor, 0.0f, ~0, g_buttonBorderThickness );

            // Draw the trigger values
            F32 const triggerValueRaw = isLeftTrigger ? controllerState.GetLeftTriggerRawValue() : controllerState.GetRightTriggerRawValue();
            if ( triggerValueRaw > 0 )
            {
                F32 triggerValue0;
                U32 triggerValue0Color;

                F32 triggerValue1;
                U32 triggerValue1Color;

                if ( isLeftTrigger )
                {
                    triggerValue0 = controllerState.GetLeftTriggerRawValue();
                    triggerValue1 = controllerState.GetLeftTriggerValue();
                    triggerValue0Color = 0xFF0000FF;
                    triggerValue1Color = 0xFF00FF00;
                }
                else
                {
                    triggerValue0 = controllerState.GetRightTriggerValue();
                    triggerValue1 = controllerState.GetRightTriggerRawValue();
                    triggerValue0Color = 0xFF00FF00;
                    triggerValue1Color = 0xFF0000FF;
                }

                F32 const valueMaxLength = borderDimensions.y - ( g_buttonBorderThickness * 2 );
                F32 const triggerValueWidth = ( borderDimensions.x - g_buttonBorderThickness * 2 ) / 2;
                F32 const triggerValue0TopLeftX = drawPosition.x + g_buttonBorderThickness;
                F32 const triggerValue1TopLeftX = triggerValue0TopLeftX + triggerValueWidth;
                F32 const triggerValue0TopLeftY = drawPosition.y + g_buttonBorderThickness + ( 1.0f - triggerValue0 ) * valueMaxLength;
                F32 const triggerValue1TopLeftY = drawPosition.y + g_buttonBorderThickness + ( 1.0f - triggerValue1 ) * valueMaxLength;

                Float2 const triggerValue0TopLeft( triggerValue0TopLeftX, triggerValue0TopLeftY );
                Float2 const triggerValue0BottomRight( triggerValue1TopLeftX, triggerBottomRight.y - g_buttonBorderThickness );
                pDrawList->AddRectFilled( triggerValue0TopLeft, triggerValue0BottomRight, triggerValue0Color );

                Float2 const triggerValue1TopLeft( triggerValue0TopLeftX + triggerValueWidth, triggerValue1TopLeftY );
                Float2 const triggerValue1BottomRight( triggerValue1TopLeftX + triggerValueWidth, triggerBottomRight.y - g_buttonBorderThickness );
                pDrawList->AddRectFilled( triggerValue1TopLeft, triggerValue1BottomRight, triggerValue1Color );
            }
        }

        static void DrawAnalogStick( ImDrawList* pDrawList, Float2 const position, ControllerInputDevice const& controller, bool isLeftStick )
        {
            KRG_ASSERT( pDrawList != nullptr );

            auto const& settings = controller.GetSettings();
            auto const& controllerState = controller.GetControllerState();

            Float2 rawValue = isLeftStick ? controllerState.GetLeftAnalogStickRawValue() : controllerState.GetRightAnalogStickRawValue();
            Float2 filteredValue = isLeftStick ? controllerState.GetLeftAnalogStickValue() : controllerState.GetRightAnalogStickValue();

            // Invert the y values to match screen space
            rawValue.y = -rawValue.y;
            filteredValue.y = -filteredValue.y;

            // Draw max stick range and dead zone range
            F32 const innerDeadZoneRadius = g_analogStickRangeRadius * ( isLeftStick ? settings.m_leftStickInnerDeadzone : settings.m_rightStickInnerDeadzone );
            F32 const outerDeadZoneRadius = g_analogStickRangeRadius * ( 1.0f - ( isLeftStick ? settings.m_leftStickOuterDeadzone : settings.m_rightStickOuterDeadzone ) );
            Float2 const analogStickCenter = position + Float2( g_analogStickRangeRadius );
            pDrawList->AddCircle( analogStickCenter, g_analogStickRangeRadius, g_controlFillColor, 20 );
            pDrawList->AddCircleFilled( analogStickCenter, outerDeadZoneRadius, g_controlFillColor, 20 );
            pDrawList->AddCircleFilled( analogStickCenter, innerDeadZoneRadius, 0xFF333333, 20 );

            // Draw raw stick position
            Float2 stickOffset = rawValue * g_analogStickRangeRadius;
            pDrawList->AddCircleFilled( analogStickCenter + stickOffset, g_analogStickPositionRadius, 0xFF0000FF, 6 );

            // Draw filtered stick position
            Vector vDirection = Vector( filteredValue ).GetNormalized2();
            stickOffset = ( filteredValue * ( outerDeadZoneRadius - innerDeadZoneRadius ) ) + ( vDirection * innerDeadZoneRadius ).ToFloat2();
            pDrawList->AddCircleFilled( analogStickCenter + stickOffset, g_analogStickPositionRadius, 0xFF00FF00, 6 );
        }

        //-------------------------------------------------------------------------

        void InputDebugViewController::DrawControllerState( ControllerInputDevice const& controller )
        {
            auto const& controllerState = controller.GetControllerState();

            ImDrawList* pDrawList = ImGui::GetWindowDrawList();
            Float2 const FirstRowTopLeft = ImGui::GetCursorScreenPos();
            Float2 const triggerButtonDimensions( g_buttonWidth, ( g_analogStickRangeRadius * 2 ) + ( g_buttonWidth * 2 ) + 8 );

            Float2 totalSize;
            Float2 drawPosition;

            // Left Shoulder and trigger buttons
            drawPosition = FirstRowTopLeft;
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "LB", controllerState.IsHeldDown( Input::ControllerButton::ShoulderLeft ) );
            drawPosition.y += g_buttonDimensions.y;
            DrawTriggerButton( pDrawList, drawPosition, triggerButtonDimensions, "LT", controllerState, true );

            // Left analog stick
            drawPosition = Float2( drawPosition.x + g_buttonDimensions.x + 9, FirstRowTopLeft.y );
            DrawAnalogStick( pDrawList, drawPosition, controller, true );

            // Right analog stick
            drawPosition = Float2( drawPosition.x + 26 + g_analogStickRangeRadius * 2, FirstRowTopLeft.y );
            DrawAnalogStick( pDrawList, drawPosition, controller, false );

            // Right Shoulder and trigger buttons
            drawPosition = Float2( drawPosition.x + g_analogStickRangeRadius * 2 + 9, FirstRowTopLeft.y );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "RB", controllerState.IsHeldDown( Input::ControllerButton::ShoulderRight ) );
            drawPosition.y += g_buttonDimensions.y;
            DrawTriggerButton( pDrawList, drawPosition, triggerButtonDimensions, "RT", controllerState, false );

            totalSize.x = ( drawPosition.x + g_buttonWidth ) - FirstRowTopLeft.x;
            totalSize.y = ( g_analogStickRangeRadius * 2 ) + 8;

            //-------------------------------------------------------------------------

            Float2 const SecondRowTopLeft = Float2( FirstRowTopLeft.x, FirstRowTopLeft.y + totalSize.y );

            // D-Pad
            F32 const upButtonTopLeft = SecondRowTopLeft.x + ( g_buttonWidth + 9 + g_analogStickRangeRadius ) - g_buttonWidth / 2;

            drawPosition = Float2( upButtonTopLeft, SecondRowTopLeft.y );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "Up", controllerState.IsHeldDown( Input::ControllerButton::DPadUp ) );
            drawPosition = Float2( upButtonTopLeft - g_buttonWidth, SecondRowTopLeft.y + g_buttonWidth );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "Lt", controllerState.IsHeldDown( Input::ControllerButton::DPadLeft ) );
            drawPosition = Float2( upButtonTopLeft + g_buttonWidth, SecondRowTopLeft.y + g_buttonWidth );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "Rt", controllerState.IsHeldDown( Input::ControllerButton::DPadRight ) );
            drawPosition = Float2( upButtonTopLeft, SecondRowTopLeft.y + g_buttonWidth + g_buttonWidth );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "Dn", controllerState.IsHeldDown( Input::ControllerButton::DPadDown ) );

            // Face Buttons
            F32 const topFaceButtonTopLeft = SecondRowTopLeft.x + ( ( g_buttonWidth + g_analogStickRangeRadius ) - g_buttonWidth / 2 ) * 2 + 34 + ( g_buttonWidth * 2 );

            drawPosition = Float2( topFaceButtonTopLeft, SecondRowTopLeft.y );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "Y", controllerState.IsHeldDown( Input::ControllerButton::FaceButtonUp ), 0xFF00FFFF );
            drawPosition = Float2( topFaceButtonTopLeft - g_buttonWidth, SecondRowTopLeft.y + g_buttonWidth );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "X", controllerState.IsHeldDown( Input::ControllerButton::FaceButtonLeft ), 0xFFFF0000 );
            drawPosition = Float2( topFaceButtonTopLeft + g_buttonWidth, SecondRowTopLeft.y + g_buttonWidth );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "B", controllerState.IsHeldDown( Input::ControllerButton::FaceButtonRight ), 0xFF0000FF );
            drawPosition = Float2( topFaceButtonTopLeft, SecondRowTopLeft.y + g_buttonWidth + g_buttonWidth );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "A", controllerState.IsHeldDown( Input::ControllerButton::FaceButtonDown ), 0xFF00FF00 );

            // System Buttons
            drawPosition = Float2( SecondRowTopLeft.x + g_buttonWidth + g_analogStickRangeRadius * 2, SecondRowTopLeft.y + 10 );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "S0", controllerState.IsHeldDown( Input::ControllerButton::System0 ) );
            drawPosition = Float2( drawPosition.x + g_buttonWidth + 4, drawPosition.y );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "S1", controllerState.IsHeldDown( Input::ControllerButton::System1 ) );

            // Stick Buttons
            drawPosition = Float2( SecondRowTopLeft.x + g_buttonWidth + g_analogStickRangeRadius * 2, drawPosition.y + g_buttonWidth + 4 );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "LS", controllerState.IsHeldDown( Input::ControllerButton::ThumbstickLeft ) );
            drawPosition = Float2( drawPosition.x + g_buttonWidth + 4, drawPosition.y );
            DrawButton( pDrawList, drawPosition, g_buttonDimensions, "RS", controllerState.IsHeldDown( Input::ControllerButton::ThumbstickRight ) );

            totalSize.x = ( drawPosition.x + g_buttonWidth ) - FirstRowTopLeft.x;
            totalSize.y = triggerButtonDimensions.y + g_buttonWidth + 4;

            //-------------------------------------------------------------------------
            ImGui::Dummy( totalSize );
        }

        //-------------------------------------------------------------------------

        InputDebugViewController::InputDebugViewController()
        {
            auto drawSystemLogMenuOptions = [this] ( UpdateContext const& context )
            {

                if ( ImGui::BeginMenu( "Controllers" ) )
                {
                    auto pInputSystem = context.GetSystem<InputSystem>();
                    KRG_ASSERT( pInputSystem != nullptr );

                    U32 const numControllers = pInputSystem->GetNumConnectedControllers();
                    if ( numControllers > 0 )
                    {
                        char buffer[256];
                        bool noControllersConnected = true;
                        for ( U32 i = 0u; i < numControllers; i++ )
                        {

                            Printf( buffer, 256, "Show Controller State: %d", i );
                            if ( ImGui::Button( buffer ) )
                            {
                                auto pController = pInputSystem->GetControllerDevice( i );
                                bool isWindowAlreadyOpen = VectorContains( m_openControllerWindows, pController );
                                if ( !isWindowAlreadyOpen )
                                {
                                    m_openControllerWindows.emplace_back( pController );
                                }
                            }
                        }
                    }
                    else
                    {
                        ImGui::Text( "No Controllers Connected" );
                    }

                    ImGui::EndMenu();
                }
            };

            m_menuCallbacks.emplace_back( Debug::DebugMenuCallback( "Controller", "Input", drawSystemLogMenuOptions ) );
        }

        void InputDebugViewController::DrawWindows( UpdateContext const& context )
        {
            auto pInputSystem = context.GetSystem<InputSystem>();
            KRG_ASSERT( pInputSystem != nullptr );

            char buffer[256];

            // Draw the open controller windows
            S32 const numControllers = (S32) m_openControllerWindows.size();
            for ( S32 i = numControllers - 1; i >= 0; i-- )
            {
                // Draw the window
                Printf( buffer, 256, "Controller State: Controller %d", i );

                bool isWindowOpen = true;
                ImGui::SetNextWindowSize( ImVec2( 300, 210 ) );
                if ( ImGui::Begin( buffer, &isWindowOpen, ImGuiWindowFlags_NoResize ) )
                {
                    DrawControllerState( *m_openControllerWindows[i] );
                    ImGui::End();
                }

                // Should we close the window?
                if ( !isWindowOpen )
                {
                    VectorEraseUnsorted( m_openControllerWindows, i );
                }
            }
        }
    }
}
#endif