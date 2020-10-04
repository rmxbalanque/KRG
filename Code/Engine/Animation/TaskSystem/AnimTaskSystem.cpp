#include "AnimTaskSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        TaskSystem::TaskSystem( Skeleton const* pSkeleton )
            : m_poseStack( pSkeleton )
            , m_mode( RegistrationMode::PrePhys )
        {
            KRG_ASSERT( pSkeleton != nullptr );
        }

        void TaskSystem::Reset()
        {
            for ( auto pTask : m_tasks )
            {
                KRG::Delete( pTask );
            }

            m_tasks.clear();
            m_prePhysicsTasks.clear();
            m_postPhysicsTasks.clear();
            m_poseStack.Reset();
            m_mode = RegistrationMode::PrePhys;
        }

        void TaskSystem::ExecutePrePhysicsTasks( F32 deltaTime )
        {
            // Execute tasks
            for ( auto pTask : m_prePhysicsTasks )
            {
                pTask->Execute( m_poseStack );
            }
        }

        void TaskSystem::ExecutePostPhysicsTasks( F32 deltaTime )
        {
            for ( auto pTask : m_postPhysicsTasks )
            {
                pTask->Execute( m_poseStack );
            }
        }
    }
}