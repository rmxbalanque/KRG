#include "Animation_Task.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    Task::Task( TaskSourceID sourceID, TaskUpdateStage updateRequirements, TaskDependencies const& dependencies )
        : m_sourceID( sourceID )
        , m_updateStage( updateRequirements )
        , m_dependencies( dependencies )
    {
        KRG_ASSERT( sourceID != InvalidIndex );
    }
}