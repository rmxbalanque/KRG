#pragma once
#include "System/Core/Algorithm/Hash.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class EntityComponent;
    class SpatialEntityComponent;
    class OrbitCameraComponent;
    namespace Physics { class CapsuleComponent; }
}

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class PlayerGameplayPhysicsComponent;
    class PlayerComponent;
    class PlayerGameplayState;

    //-------------------------------------------------------------------------

    struct GameplayStateContext
    {
        ~GameplayStateContext()
        {
            KRG_ASSERT( m_pEntityUpdateContext == nullptr && m_pPlayerComponent == nullptr && m_pRootComponent == nullptr && m_pCapsuleComponent == nullptr && m_pCameraComponent == nullptr && m_pGameplayPhysicsComponent == nullptr );
        }

        inline bool IsValid() const
        {
            return m_pEntityUpdateContext != nullptr && m_pPlayerComponent != nullptr && m_pRootComponent != nullptr && m_pCapsuleComponent != nullptr && m_pCameraComponent != nullptr && m_pGameplayPhysicsComponent != nullptr;
        }

        template<typename T>
        T* GetComponentByType() const
        {
            static_assert( std::is_base_of<EntityComponent, T>::value, "T must be a component type" );
            for ( auto pComponent : m_components )
            {
                if ( auto pCastComponent = TryCast<T>( pComponent ) )
                {
                    return pCastComponent;
                }
            }

            return nullptr;
        }

        // Forwarding helper functions
        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE Seconds GetDeltaTime() const { return m_pEntityUpdateContext->GetDeltaTime(); }
        template<typename T> inline T* GetWorldSystem() const { return m_pEntityUpdateContext->GetWorldSystem<T>(); }
        template<typename T> inline T* GetSystem() const { return m_pEntityUpdateContext->GetSystem<T>(); }

    public:

        EntityUpdateContext const*              m_pEntityUpdateContext = nullptr;
        PlayerComponent*                        m_pPlayerComponent = nullptr;
        SpatialEntityComponent*                 m_pRootComponent = nullptr;
        Physics::CapsuleComponent*              m_pCapsuleComponent = nullptr;
        OrbitCameraComponent*                   m_pCameraComponent = nullptr;
        PlayerGameplayPhysicsComponent*         m_pGameplayPhysicsComponent = nullptr;
        TInlineVector<EntityComponent*, 10>     m_components;
    };

    //-------------------------------------------------------------------------

    class GameplayState
    {
    public:

        enum class Status : uint8
        {
            Running,
            Completed
        };

        enum class StopReason : uint8
        {
            StateCompleted,
            TransitionFired
        };

    public:

        virtual ~GameplayState() = default;

        // Get the ID for this gameplay state
        virtual uint32 GetGameplayStateID() const = 0;

        // Try to start this state - this is where you handle all transition conditions
        virtual bool TryStart( GameplayStateContext const& ctx ) { return true; }

        // Called to update this state, this will be called directly after the try start if it succeeds
        virtual Status Update( GameplayStateContext const& ctx ) = 0;

        // Called to stop this state
        virtual void Stop( GameplayStateContext const& ctx, StopReason reason ) = 0;
    };
}

//-------------------------------------------------------------------------

#define KRG_PLAYER_GAMEPLAY_STATE_ID( TypeName ) \
constexpr static uint32 const s_gameplayStateID = Hash::FNV1a::GetHash32( #TypeName ); \
virtual uint32 GetGameplayStateID() const override final { return TypeName::s_gameplayStateID; }