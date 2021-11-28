#include "AnimationGraphTask_Sample.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph::Tasks
{
    SampleTask::SampleTask( NodeIndex sourceNodeIdx, AnimationClip const* pAnimation, Percentage time )
        : Task( sourceNodeIdx )
        , m_pAnimation( pAnimation )
        , m_time( time )
    {
        KRG_ASSERT( m_pAnimation != nullptr );
    }

    void SampleTask::Execute( TaskContext const& context )
    {
        KRG_ASSERT( m_pAnimation != nullptr );

        auto pResultBuffer = GetNewPoseBuffer( context );
        m_pAnimation->GetPose( m_time, &pResultBuffer->m_pose );
        MarkTaskComplete( context );
    }

    #if KRG_DEVELOPMENT_TOOLS
    String SampleTask::GetDebugText() const
    {
        if ( m_pAnimation->IsAdditive() )
        {
            return String( String::CtorSprintf(), "Sample Task (Additive): %s at %.2f", m_pAnimation->GetResourceID().c_str(), (float) m_time );
        }
        else
        {
            return String( String::CtorSprintf(), "Sample Task: %s at %.2f", m_pAnimation->GetResourceID().c_str(), (float) m_time );
        }
    }
    #endif
}