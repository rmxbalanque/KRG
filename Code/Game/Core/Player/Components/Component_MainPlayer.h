#pragma once

#include "Game/Core/_Module/API.h"
#include "Engine/Core/Components/Component_Player.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class EnergyController;
}

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class KRG_GAME_CORE_API MainPlayerComponent : public PlayerComponent
    {
        friend EnergyController;
        KRG_REGISTER_ENTITY_COMPONENT( MainPlayerComponent );

    public:

        float GetEnergyLevel() const { return m_energyLevel; }
        bool HasEnoughEnergy( float requiredEnergy ) const { return m_energyLevel >= requiredEnergy; }
        void ConsumeEnergy( float ConsumedEnergy ) { KRG_ASSERT( m_energyLevel >= ConsumedEnergy ); m_energyLevel -= ConsumedEnergy; }

        void UpdateState( float deltaTime );

    public:

        bool m_sprintFlag = false;
        bool m_CrouchFlag = false;
        KRG_EXPOSE FloatCurve jumpCurve;

    private:

        float m_energyLevel = 3.0f;
    };
}