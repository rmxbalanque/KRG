#pragma once
#include "_Module/API.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Algorithm/Hash.h"
#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class CharacterComponent;
    class PhysicsWorldSystem;
}

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_ENGINE_PHYSICS_API PhysicsState
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
        virtual bool TryMoveCapsule( Physics::PhysicsWorldSystem* pPhysicsWorld, Physics::CharacterComponent* pCharacterComponent, Seconds const deltaTime, Vector const& deltaTranslation, Quaternion const& deltaRotation ) = 0;

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        // Returns a friendly name for the action
        virtual char const* GetName() const = 0;

        // Override this function to draw custom imgui controls in the physics state debugger UI
        virtual void DrawDebugUI();
        #endif
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsStateController
    {
        friend class PhysicsDebugView;

    public:

        enum SetStateOption
        {
            CrashIfAlreadyActive = 0,
            DoNothingIfAlreadyActive,
            ResetIfAlreadyActive
        };

    public:

        virtual ~PhysicsStateController();

        inline bool HasCreatedPhysicsStates() const { return !m_registeredStates.empty(); }

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
        inline T* SetPhysicsState( SetStateOption const option = SetStateOption::CrashIfAlreadyActive )
        {
            static_assert( std::is_base_of<PhysicsState, T>::value, "T is not derived from GameplayPhysics::State" );

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
                        KRG_ASSERT( m_pActiveState != nullptr );
                        return static_cast<T*>(m_pActiveState);
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
            if( m_pActiveState != nullptr ) // Can happen on the first frame
            {
                m_pActiveState->Deactivate();
            }
            m_pActiveState = static_cast<T*>( pNewState );
            m_pActiveState->Activate();

            KRG_ASSERT( m_pActiveState != nullptr );
            return static_cast<T*>( m_pActiveState );
        }

    protected:

        PhysicsState*                      m_pActiveState = nullptr;
        TInlineVector<PhysicsState*, 6>    m_registeredStates;
    };
}

//-------------------------------------------------------------------------

#define KRG_PHYSICS_STATE_ID( TypeName ) \
constexpr static uint32 const s_physicsStateID = Hash::FNV1a::GetHash32( #TypeName ); \
virtual uint32 GetPhysicsStateID() const override final { return TypeName::s_physicsStateID; }\
KRG_DEVELOPMENT_TOOLS_LINE_IN_MACRO( char const* GetName() const override final { return #TypeName; } )