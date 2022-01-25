#pragma once

#include "Engine/Animation/_Module/API.h"
#include "Engine/Animation/TaskSystem/Animation_TaskSystem.h"
#include "Engine/Core/Entity/EntityWorldDebugView.h"
#include "Engine/Core/Entity/EntityIDs.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Animation
{
    class AnimationWorldSystem;
    class AnimationGraphComponent;
    class TaskSystem;
    class RootMotionRecorder;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API AnimationDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( AnimationDebugView );

        struct ComponentDebugSettings
        {
            ComponentDebugSettings( ComponentID ID )
                : m_ID( ID )
            {}

            ComponentID             m_ID;
            bool                    m_drawControlParameters = false;
            bool                    m_drawActiveTasks = false;
            bool                    m_drawSampledEvents = false;
        };

    public:

        static void DrawGraphControlParameters( AnimationGraphComponent* pGraphComponent );
        static void DrawGraphActiveTasksDebugView( AnimationGraphComponent* pGraphComponent );
        static void DrawGraphSampledEventsView( AnimationGraphComponent* pGraphComponent );

    private:

        static void DrawTaskTreeRow( AnimationGraphComponent* pGraphComponent, TaskSystem* pTaskSystem, TaskIndex currentTaskIdx );
        static void DrawRootMotionRow( AnimationGraphComponent* pGraphComponent, RootMotionRecorder* pRootMotionRecorder, int16 currentActionIdx );

    public:

        AnimationDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;
        virtual void DrawOverlayElements( EntityWorldUpdateContext const& context ) override;

        ComponentDebugSettings* GetDebugSettings( ComponentID ID );
        void DestroyDebugSettings( ComponentID ID );

        void DrawMenu( EntityWorldUpdateContext const& context );

    private:

        EntityWorld const*                      m_pWorld = nullptr;
        AnimationWorldSystem*                   m_pAnimationWorldSystem = nullptr;
        TVector<ComponentDebugSettings>         m_componentDebugSettings;
    };
}
#endif