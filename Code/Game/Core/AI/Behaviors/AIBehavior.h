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
    namespace Physics { class CharacterComponent; class PhysicsWorldSystem; }
    namespace Navmesh { class NavmeshWorldSystem; }
    namespace Animation { class GraphController; }
}

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class AIComponent;
    class CharacterPhysicsController;

    //-------------------------------------------------------------------------
    // The context for all AI behaviors
    //-------------------------------------------------------------------------
    // Provides the common set of systems and components needed for AI behaviors/actions

    struct BehaviorContext
    {
        ~BehaviorContext();

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
        Physics::PhysicsWorldSystem*                m_pPhysicsWorld = nullptr;
        Navmesh::NavmeshWorldSystem*                m_pNavmeshSystem = nullptr;

        AIComponent*                                m_pAIComponent = nullptr;
        Physics::CharacterComponent*                m_pCharacter = nullptr;
        CharacterPhysicsController*                 m_pCharacterController = nullptr;
        Animation::GraphController*                 m_pAnimationController = nullptr;
        TInlineVector<EntityComponent*, 10>         m_components;
    };

    //-------------------------------------------------------------------------
    // An AI Action
    //-------------------------------------------------------------------------
    // A specific actuation task (move, play anim, etc...) that a behavior requests to help achieve its goal
    // Each derived action needs to define a StartInternal( ActionContext const& ctx, ARGS... ) function that will start the AI action

    class Action
    {
    public:

        enum class Status : uint8
        {
            Running,
            Completed,
            Failed
        };

    public:

    };


    //-------------------------------------------------------------------------
    // An AI behavior
    //-------------------------------------------------------------------------
    // This defines a behavior i.e. a sequence of actions to achieve a specified goal

    class Behavior
    {
    public:

        enum class Status : uint8
        {
            Running,
            Completed,
            Failed
        };

        enum class StopReason : uint8
        {
            Completed,
            Interrupted
        };

    public:

        virtual ~Behavior() = default;

        // Get the ID for this action
        virtual uint32 GetActionID() const = 0;

        // Is this action active
        inline bool IsActive() const { return m_isActive; }

        // Try to start this action - this is where you check all the start preconditions
        inline void Start( BehaviorContext const& ctx )
        {
            KRG_ASSERT( !m_isActive );
            StartInternal( ctx );
            m_isActive = true;
        }

        // Called to update this action, this will be called directly after the try start if it succeeds
        inline Status Update( BehaviorContext const& ctx )
        {
            KRG_ASSERT( m_isActive );
            return UpdateInternal( ctx );
        }

        // Called to stop this action
        inline void Stop( BehaviorContext const& ctx, StopReason reason )
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

        // Called to start this action
        virtual void StartInternal( BehaviorContext const& ctx ) = 0;

        // Called to update this action, this will be called directly after the try start if it succeeds
        virtual Status UpdateInternal( BehaviorContext const& ctx ) = 0;

        // Called to stop this action
        virtual void StopInternal( BehaviorContext const& ctx, StopReason reason ) = 0;

    private:

        bool    m_isActive = false;
    };
}

//-------------------------------------------------------------------------

#define KRG_AI_BEHAVIOR_ID( TypeName ) \
constexpr static uint32 const s_gameplayStateID = Hash::FNV1a::GetHash32( #TypeName ); \
virtual uint32 GetActionID() const override final { return TypeName::s_gameplayStateID; }\
KRG_DEVELOPMENT_TOOLS_LINE_IN_MACRO( char const* GetName() const override final { return #TypeName; } )