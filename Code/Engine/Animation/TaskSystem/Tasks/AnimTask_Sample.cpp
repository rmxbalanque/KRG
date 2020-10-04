#include "AnimTask_Sample.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        namespace Tasks
        {
            Sample::Sample( S32 sourceID, AnimationData const* pAnimSequence, Percentage time )
                : Task( sourceID )
                , m_pAnimation( pAnimSequence )
                , m_time( time )
            {
                KRG_ASSERT( m_pAnimation != nullptr );
            }

            void Sample::Execute( TaskPoseStack& poseStack ) const
            {
                KRG_ASSERT( m_pAnimation != nullptr );

                auto pPose = poseStack.PushPose();
                m_pAnimation->GetPose( m_time, pPose );

                #if KRG_DEBUG_INSTRUMENTATION
                poseStack.RecordDebugPose( pPose );
                #endif
            }

            #if KRG_DEBUG_INSTRUMENTATION
            String Sample::GetDebugText() const
            {
                if ( m_pAnimation->IsAdditive() )
                {
                    return String().sprintf( "Sample Task (Additive): %s at %.2f", m_pAnimation->GetName(), (F32) m_time );
                }
                else
                {
                    return String().sprintf( "Sample Task: %s at %.2f", m_pAnimation->GetName(), (F32) m_time );
                }
            }
            #endif
        }
    }

}