#include "AnimGraphTask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    Task::Task( int16 sourceNodeIdx, UpdateStage updateRequirements, TaskDependencies const& dependencies )
        : m_sourceNodeIdx( sourceNodeIdx )
        , m_updateStage( updateRequirements )
        , m_dependencies( m_dependencies )
    {
        KRG_ASSERT( sourceNodeIdx != InvalidIndex );
    }
}