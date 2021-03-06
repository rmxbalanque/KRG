#pragma once

#include "AnimTask.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class TaskSystem
        {

        public:

            enum class RegistrationMode
            {
                PrePhys = 0,
                PostPhys = 1,
            };

        public:

            TaskSystem( Skeleton const* pSkeleton );

            void Reset();
            inline Pose const* GetFinalResult() const { return m_poseStack.GetFinalResult(); }

            inline bool HasPhysicsTasks() const { return !m_postPhysicsTasks.empty(); }

            void ExecutePrePhysicsTasks( float deltaTime );
            void ExecutePostPhysicsTasks( float deltaTime );

            inline void SwitchMode( RegistrationMode mode ) { m_mode = mode; }

            // Animation tasks
            template< typename T, typename ... ConstructorParams >
            void RegisterTask( ConstructorParams&&... params )
            {
                auto pTask = KRG::New<T>( std::forward<ConstructorParams>( params )... );
                m_tasks.emplace_back( pTask );

                if ( m_mode == RegistrationMode::PrePhys )
                {
                    KRG_ASSERT( !pTask->IsPostPhysicsTask() );
                    m_prePhysicsTasks.emplace_back( pTask );
                }
                else
                {
                    m_postPhysicsTasks.emplace_back( pTask );
                }
            }

        private:

            TVector<Task*>                  m_tasks;                // All the registered tasks
            TVector<Task*>                  m_prePhysicsTasks;      // Only pre-physics tasks
            TVector<Task*>                  m_postPhysicsTasks;     // Only post-physics tasks
            TaskPoseStack                   m_poseStack;            // The pose buffer stack
            RegistrationMode                m_mode;                 // What type of tasks are we registering
        };
    }
}