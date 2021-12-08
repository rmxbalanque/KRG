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
    namespace Physics { class CharacterComponent; class PhysicsStateController; class PhysicsWorldSystem; }
    namespace Navmesh { class NavmeshWorldSystem; }
    namespace Animation { class GraphController; }
}

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class AIComponent;

    //-------------------------------------------------------------------------
    // The context for all AI actions
    //-------------------------------------------------------------------------
    // Provides the common set of systems and components needed for AI actions

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
        Physics::PhysicsWorldSystem*                m_pPhysicsWorld = nullptr;
        Navmesh::NavmeshWorldSystem*                m_pNavmeshSystem = nullptr;

        AIComponent*                                m_pAIComponent = nullptr;
        Physics::PhysicsStateController*            m_pPhysicsController = nullptr;
        Physics::CharacterComponent*                m_pCharacterPhysicsComponent = nullptr;
        Animation::GraphController*                 m_pAnimationController = nullptr;
        TInlineVector<EntityComponent*, 10>         m_components;
    };

    //-------------------------------------------------------------------------
    // The AI action state
    //-------------------------------------------------------------------------
    // This defines a discrete action the AI is currently undertaking i.e. moving, shooting, reloading, etc...

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
            Interrupted
        };

    public:

        virtual ~Action() = default;

        // Get the ID for this action
        virtual uint32 GetActionID() const = 0;

        // Is this action active
        inline bool IsActive() const { return m_isActive; }

        // Try to start this action - this is where you check all the start preconditions
        template<typename... ConstructorParams>
        inline void Start( ActionContext const& ctx, ConstructorParams&&... params )
        {
            KRG_ASSERT( !m_isActive );
            StartInternal( ctx, std::forward<ConstructorParams>( params )... );
            m_isActive = true;
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

        // Each action needs to define a StartInternal( ActionContext const& ctx, ARGS... ) function that will start the AI action

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

#define KRG_AI_ACTION_ID( TypeName ) \
constexpr static uint32 const s_gameplayStateID = Hash::FNV1a::GetHash32( #TypeName ); \
virtual uint32 GetActionID() const override final { return TypeName::s_gameplayStateID; }\
KRG_DEVELOPMENT_TOOLS_LINE_IN_MACRO( char const* GetName() const override final { return #TypeName; } )