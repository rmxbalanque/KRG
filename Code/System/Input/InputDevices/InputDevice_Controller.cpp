#include "InputDevice_Controller.h"
#include "System\Core\Math\Vector.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Input
    {
        void ControllerInputDevice::SetTriggerValues( float leftRawValue, float rightRawValue )
        {
            auto calculateFilteredTriggerValue = [this] ( float rawValue, float threshold )
            {
                KRG_ASSERT( threshold >= 0 && threshold <= 1.0f );

                float filteredValue = 0.0f;
                if ( rawValue > threshold )
                {
                    filteredValue = ( rawValue - threshold ) / ( 1.0f - threshold );
                }

                return filteredValue;
            };

            //-------------------------------------------------------------------------

            m_controllerState.m_triggerRaw[ControllerInputState::Direction::Left] = leftRawValue;
            m_controllerState.m_triggerRaw[ControllerInputState::Direction::Right] = rightRawValue;
            m_controllerState.m_triggerFiltered[ControllerInputState::Direction::Left] = calculateFilteredTriggerValue( leftRawValue, m_settings.m_leftTriggerThreshold );
            m_controllerState.m_triggerFiltered[ControllerInputState::Direction::Right] = calculateFilteredTriggerValue( rightRawValue, m_settings.m_rightTriggerThreshold );
        }

        void ControllerInputDevice::SetAnalogStickValues( Float2 const& leftRawValue, Float2 const& rightRawValue )
        {
            auto calculateRawValue = [this] ( Float2 const rawValue, bool bInvertY )
            {
                float const normalizedX = Math::Clamp( rawValue.x, -1.0f, 1.0f );
                float const normalizedY = Math::Clamp( rawValue.y, -1.0f, 1.0f );
                return Float2( normalizedX, bInvertY ? -normalizedY : normalizedY );
            };

            m_controllerState.m_analogInputRaw[ControllerInputState::Direction::Left] = calculateRawValue( leftRawValue, m_settings.m_leftStickInvertY );
            m_controllerState.m_analogInputRaw[ControllerInputState::Direction::Right] = calculateRawValue( rightRawValue, m_settings.m_rightStickInvertY );

            auto calculateFilteredValue = [this] ( Float2 const rawValue, float const innerDeadzoneRange, float const outerDeadzoneRange )
            {
                KRG_ASSERT( innerDeadzoneRange >= 0 && innerDeadzoneRange <= 1.0f && outerDeadzoneRange >= 0 && outerDeadzoneRange <= 1.0f );

                Float2 filteredValue;

                // Get the direction and magnitude
                Vector vDirection, vMagnitude;
                Vector( rawValue ).ToDirectionAndLength2( vDirection, vMagnitude );

                // Apply dead zones
                float const magnitude = vMagnitude.ToFloat();
                if ( magnitude > innerDeadzoneRange )
                {
                    float const remainingRange = ( 1.0f - outerDeadzoneRange - innerDeadzoneRange );
                    float const newMagnitude = Math::Min( 1.0f, ( magnitude - innerDeadzoneRange ) / remainingRange );
                    filteredValue = ( vDirection * newMagnitude ).ToFloat2();
                }
                else // Set the value to zero
                {
                    filteredValue = Float2::Zero;
                }

                return filteredValue;
            };

            m_controllerState.m_analogInputFiltered[ControllerInputState::Direction::Left] = calculateFilteredValue( m_controllerState.m_analogInputRaw[ControllerInputState::Direction::Left], m_settings.m_leftStickInnerDeadzone, m_settings.m_leftStickOuterDeadzone );
            m_controllerState.m_analogInputFiltered[ControllerInputState::Direction::Right] = calculateFilteredValue( m_controllerState.m_analogInputRaw[ControllerInputState::Direction::Right], m_settings.m_rightStickInnerDeadzone, m_settings.m_rightStickOuterDeadzone );
        }
    }
}