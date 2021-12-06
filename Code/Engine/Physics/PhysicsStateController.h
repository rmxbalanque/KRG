#pragma once
#include "_Module/API.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Algorithm/Hash.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class CharacterComponent;
    class PhysicsWorldSystem;
}

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsState
    {
    public:

        virtual ~PhysicsState() = default;

        // Get the ID for this physics state
        virtual uint32 GetPhysicsStateID() const = 0;

        // Called whenever we switch to this state
        virtual void Activate() {}

        // Called whenever we leave this state
        virtual void Deactivate() {}

        // Try to move the capsule, delta rotation/translation are in world space. Returns true if the capsule was able to be moved
        virtual bool TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsWorld, Physics::CharacterComponent* pCharacterComponent, float const deltaTime, Transform const& deltaTransform ) = 0;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsStateController
    {

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
        inline PhysicsState* GetActivePhysicsState() const { return m_pActiveState; }

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
            static_assert( std::is_base_of<KRG::PhysicsState, T>::value, "T is not derived from GameplayPhysics::State" );
            KRG_ASSERT( m_pActiveState != nullptr );

            // Find the new state
            //-------------------------------------------------------------------------

            PhysicsState* pNewState = nullptr;
            for ( PhysicsState* pState : m_registeredStates )
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
                    case CrashIfAlreadyActive:
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

        PhysicsState*                      m_pActiveState = nullptr;
        TInlineVector<PhysicsState*, 6>    m_registeredStates;
    };
}

//-------------------------------------------------------------------------

#define KRG_PHYSICS_STATE_ID( TypeName ) \
constexpr static uint32 const s_physicsStateID = Hash::FNV1a::GetHash32( #TypeName ); \
virtual uint32 GetPhysicsStateID() const override final { return TypeName::s_physicsStateID; }