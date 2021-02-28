#pragma once

#include "System/Core/Memory/Memory.h"

//-------------------------------------------------------------------------

namespace KRG
{
    enum class UpdateStage : S8
    {
        None = -1,

        FrameStart = 0,
        PrePhysics,
        Physics,
        PostPhysics,
        FrameEnd,

        NumStages,
    };

    // List of priorities per update stage
    //-------------------------------------------------------------------------
    // We only allow 127 levels of priority - The lower the number the earlier it runs, i.e. priority 1 will run before priority 127
    // Values < 0 means that the update for that update stage is disabled

    struct UpdatePriorityList
    {
        UpdatePriorityList()
        {
            DisableAllStages();
        }

        inline void DisableAllStages()
        {
            for ( S32 stageIdx = 0; stageIdx < (S32) UpdateStage::NumStages; stageIdx++ )
            {
                m_priorities[stageIdx] = -1;
            }
        }

        inline bool AreAllStagesDisabled() const
        {
            for( S32 stageIdx = 0; stageIdx < (S32) UpdateStage::NumStages; stageIdx++ )
            {
                if ( m_priorities[stageIdx] >= 0 )
                {
                    return false;
                }
            }

            return true;
        }

        inline S8& operator[] ( UpdateStage stage )
        {
            KRG_ASSERT( stage != UpdateStage::None && stage != UpdateStage::NumStages );
            return m_priorities[(S8) stage];
        }

        inline S8 const& operator[] ( UpdateStage stage ) const
        {
            KRG_ASSERT( stage != UpdateStage::None && stage != UpdateStage::NumStages );
            return m_priorities[(S8) stage];
        }

        inline bool IsUpdateStageEnabled( UpdateStage stage ) const
        {
            KRG_ASSERT( stage != UpdateStage::None && stage != UpdateStage::NumStages );
            return m_priorities[(S8) stage] >= 0;
        }

        inline S8 GetPriorityForStage( UpdateStage stage ) const 
        {
            KRG_ASSERT( stage != UpdateStage::None && stage != UpdateStage::NumStages );
            return m_priorities[(S8) stage];
        }

        inline UpdatePriorityList& DisableUpdateStage( UpdateStage stage )
        {
            KRG_ASSERT( stage != UpdateStage::None && stage != UpdateStage::NumStages );
            m_priorities[(S8) stage] = 0;
            return *this;
        }

        inline UpdatePriorityList& EnableUpdateStage( UpdateStage stage, U8 priority = 64 )
        {
            KRG_ASSERT( stage != UpdateStage::None && stage != UpdateStage::NumStages );
            KRG_ASSERT( priority != 0 ); // Call DisableUpdate instead
            m_priorities[(S8) stage] = priority;
            return *this;
        }

    private:

        S8 m_priorities[(S32) UpdateStage::NumStages];
    };
}