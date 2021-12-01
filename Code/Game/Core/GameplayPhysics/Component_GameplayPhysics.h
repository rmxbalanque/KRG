#pragma once

#include "Game/Core/_Module/API.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"

//-------------------------------------------------------------------------

namespace KRG::GameplayPhysics
{
    class PhysicsState;

    //-------------------------------------------------------------------------

    class KRG_GAME_CORE_API GameplayPhysicsComponent : public EntityComponent
    {
        KRG_REGISTER_SINGLETON_ENTITY_COMPONENT( GameplayPhysicsComponent );

    public:

        enum SetStateOption
        {
            CrashIfAlreadyActive = 0,
            DoNothingIfAlreadyActive,
            ResetIfAlreadyActive
        };

    public:

        inline bool HasCreatedPhysicsStates() const { return !m_registeredStates.empty(); }
        void CreatePhysicsStates();
        void DestroyPhysicsStates();

        // Get the currently active physics state
        inline GameplayPhysics::PhysicsState* GetActivePhysicsState() const { return m_pActiveState; }

        // Get the currently active physics state
        template<typename T>
        inline T* GetActivePhysicsState() const
        {
            KRG_ASSERT( m_pActiveState != nullptr );
            KRG_ASSERT( m_pActiveState->GetPhysicsStateID() == T::s_physicsStateID );
            return static_cast<T*>( m_pActiveState );
        }

        // Set new physics state
        template<typename T>
        inline T* SetPhysicsState( SetStateOption const option = SetStateOption::CrashIfAlreadyActive ) const
        {
            static_assert( std::is_base_of<KRG::GameplayPhysics::PhysicsState, T>::value, "T is not derived from GameplayPhysics::State" );
            KRG_ASSERT( m_pActiveState != nullptr );

            // Find the new state
            //-------------------------------------------------------------------------

            GameplayPhysics::PhysicsState* pNewState = nullptr;
            for ( GameplayPhysics::PhysicsState* pState : m_registeredStates )
            {
                if ( pState->GetPhysicsStateID() == T::s_physicsStateID )
                {
                    pNewState = pState;
                    break;
                }
            }

            KRG_ASSERT( pNewState != nullptr );

            // Handle requesting the same state
            //-------------------------------------------------------------------------

            if ( m_pActiveState == pNewState )
            {
                switch ( option )
                {
                    case CrashIfAlreadyActive :
                    {
                        KRG_HALT();
                    }
                    break;

                    case DoNothingIfAlreadyActive:
                    {
                        return m_pActiveState;
                    }
                    break;

                    case ResetIfAlreadyActive:
                    {
                        // Do nothing, since the next code will handle this case correctly
                    }
                    break;
                }
            }

            // Switch state
            //-------------------------------------------------------------------------

            m_pActiveState->Deactivate();
            m_pActiveState = static_cast<T*>( pNewState );
            m_pActiveState->Activate();

            return m_pActiveState;
        }

    protected:

        virtual void CreatePhysicsStatesInternal() = 0;

    protected:

        GameplayPhysics::PhysicsState*                      m_pActiveState = nullptr;
        TInlineVector<GameplayPhysics::PhysicsState*, 6>    m_registeredStates;
    };
}