#pragma once
#include "System/Core/Algorithm/Hash.h"
#include "Engine/Core/Entity/EntityUpdateContext.h"

#if KRG_DEVELOPMENT_TOOLS
#include "imgui.h"
#endif

//-------------------------------------------------------------------------

namespace KRG
{
    class EntityComponent;
    class OrbitCameraComponent;
    namespace Physics { class CharacterComponent; class PhysicsStateController; class PhysicsWorldSystem; }
    namespace Input { class InputSystem; }
    namespace Animation { class GraphController; }
}

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class MainPlayerComponent;

    //-------------------------------------------------------------------------
    // The context for all player actions
    //-------------------------------------------------------------------------
    // Provides the common set of systems and components needed for player actions

    struct ActionContext
    {
        ~ActionContext();

        bool IsValid() const;

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
        template<typename T> inline T* GetAnimSubGraphController() const { return m_pAnimationController->GetSubGraphController<T>(); }

    public:

        EntityUpdateContext const*                  m_pEntityUpdateContext = nullptr;
        Input::InputSystem*                         m_pInputSystem = nullptr;
        Physics::PhysicsWorldSystem*                m_pPhysicsWorld = nullptr;

        MainPlayerComponent*                        m_pPlayerComponent = nullptr;
        Physics::PhysicsStateController*            m_pPhysicsController = nullptr;
        Physics::CharacterComponent*                m_pCharacterPhysicsComponent = nullptr;
        Animation::GraphController*                 m_pAnimationController = nullptr;
        OrbitCameraComponent*                       m_pCameraComponent = nullptr;
        TInlineVector<EntityComponent*, 10>         m_components;
    };

    //-------------------------------------------------------------------------
    // The player action state
    //-------------------------------------------------------------------------
    // This defines a discrete action the player is currently undertaking i.e. moving, shooting, reloading, etc...

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
        // Returns a friendly name for the action
        virtual char const* GetName() const = 0;

        // Override this function to draw custom imgui controls in the action debugger UI
        virtual void DrawDebugUI() { ImGui::Text( "No Debug" ); }
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

#define KRG_PLAYER_ACTION_ID( TypeName ) \
constexpr static uint32 const s_gameplayStateID = Hash::FNV1a::GetHash32( #TypeName ); \
virtual uint32 GetActionID() const override final { return TypeName::s_gameplayStateID; }\
KRG_DEVELOPMENT_TOOLS_LINE_IN_MACRO( char const* GetName() const override final { return #TypeName; } )