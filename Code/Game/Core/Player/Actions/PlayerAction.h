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
    namespace Input { class InputSystem; }
}

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class PlayerPhysicsComponent;
    class MainPlayerComponent;

    //-------------------------------------------------------------------------

    struct ActionContext
    {
        ~ActionContext()
        {
            KRG_ASSERT( m_pEntityUpdateContext == nullptr && m_pPlayerComponent == nullptr && m_pRootComponent == nullptr && m_pCapsuleComponent == nullptr && m_pCameraComponent == nullptr && m_pPhysicsComponent == nullptr );
        }

        inline bool IsValid() const
        {
            return m_pEntityUpdateContext != nullptr && m_pInputSystem != nullptr && m_pPlayerComponent != nullptr && m_pRootComponent != nullptr && m_pCapsuleComponent != nullptr && m_pCameraComponent != nullptr && m_pPhysicsComponent != nullptr;
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
        Input::InputSystem*                     m_pInputSystem = nullptr;
        MainPlayerComponent*                    m_pPlayerComponent = nullptr;
        SpatialEntityComponent*                 m_pRootComponent = nullptr;
        Physics::CapsuleComponent*              m_pCapsuleComponent = nullptr;
        OrbitCameraComponent*                   m_pCameraComponent = nullptr;
        PlayerPhysicsComponent*                 m_pPhysicsComponent = nullptr;
        TInlineVector<EntityComponent*, 10>     m_components;
    };

    //-------------------------------------------------------------------------

    class Action
    {
    public:

        enum class Status : uint8
        {
            Running,
            Completed
        };

        enum class StopReason : uint8
        {
            ActionCompleted,
            TransitionFired
        };

    public:

        virtual ~Action() = default;

        // Get the ID for this action
        virtual uint32 GetActionID() const = 0;

        // Is this action active
        inline bool IsActive() const { return m_isActive; }

        // Try to start this action - this is where you check all the start preconditions
        inline bool TryStart( ActionContext const& ctx ) 
        {
            KRG_ASSERT( !m_isActive );
            if ( TryStartInternal( ctx ) )
            {
                m_isActive = true;
                return true;
            }

            return false;
        }

        // Called to update this action, this will be called directly after the try start if it succeeds
        inline Status Update( ActionContext const& ctx )
        {
            KRG_ASSERT( m_isActive );
            return UpdateInternal( ctx );
        }

        // Called to stop this action
        inline void Stop( ActionContext const& ctx, StopReason reason )
        {
            KRG_ASSERT( m_isActive );
            StopInternal( ctx, reason );
            m_isActive = false;
        }

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetName() const = 0;
        #endif

    protected:

        // Try to start this action - this is where you check all the start preconditions
        virtual bool TryStartInternal( ActionContext const& ctx ) { return true; }

        // Called to update this action, this will be called directly after the try start if it succeeds
        virtual Status UpdateInternal( ActionContext const& ctx ) = 0;

        // Called to stop this action
        virtual void StopInternal( ActionContext const& ctx, StopReason reason ) = 0;

    private:

        bool    m_isActive = false;
    };

    //-------------------------------------------------------------------------

    // Needed for type safety
    class OverlayAction : public Action
    {

    };
}

//-------------------------------------------------------------------------

#define KRG_PLAYER_GAMEPLAY_ACTION_ID( TypeName ) \
constexpr static uint32 const s_gameplayStateID = Hash::FNV1a::GetHash32( #TypeName ); \
virtual uint32 GetActionID() const override final { return TypeName::s_gameplayStateID; }\
KRG_DEVELOPMENT_TOOLS_LINE_IN_MACRO( char const* GetName() const override final { return #TypeName; } )