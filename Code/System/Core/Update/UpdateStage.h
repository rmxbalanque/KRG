#pragma once

#include "System/Core/Memory/Memory.h"

//-------------------------------------------------------------------------
// Update Stages
//-------------------------------------------------------------------------
// We only allow 254 levels of priority
// The lower the number the earlier it runs, i.e. priority 1 will run before priority 127
// A priority of 255 (0xFF) means that stage is disabled

namespace KRG
{
    // Pre-defined engine stages
    //-------------------------------------------------------------------------

    enum class UpdateStage : uint8
    {
        FrameStart = 0,
        PrePhysics,
        Physics,
        PostPhysics,
        FrameEnd,

        NumStages,
    };

    // Some pre-defined priority levels
    //-------------------------------------------------------------------------

    enum class UpdatePriority : uint8
    {
        Highest = 0x00,
        High = 0x40,
        Medium = 0x80,
        Low = 0xC0,
        Disabled = 0xFF,
        Default = Medium,
    };

    // Stage & Priority pair
    //-------------------------------------------------------------------------

    struct UpdateStagePriority
    {
        inline UpdateStagePriority( UpdateStage stage ) : m_stage( stage ) {}
        inline UpdateStagePriority( UpdateStage stage, uint8 priority ) : m_stage( stage ), m_priority( priority ) {}
        inline UpdateStagePriority( UpdateStage stage, UpdatePriority priority ) : m_stage( stage ), m_priority( (uint8) priority ) {}

    public:

        UpdateStage     m_stage;
        uint8           m_priority = (uint8) UpdatePriority::Default;
    };

    // Syntactic sugar for use in macro declarations
    using RequiresUpdate = UpdateStagePriority;

    // List of priorities per update stage
    //-------------------------------------------------------------------------

    struct UpdatePriorityList
    {
        UpdatePriorityList()
        {
            Reset();
        }

        template<typename... Args>
        UpdatePriorityList( Args&&... args )
        {
            Reset();
            ( ( *this << std::forward<Args>( args ) ), ... );
        }

        inline void Reset()
        {
            memset( m_priorities, (uint8) UpdatePriority::Disabled, sizeof( m_priorities ) );
        }

        inline bool IsStageEnabled( UpdateStage stage ) const
        {
            KRG_ASSERT( stage < UpdateStage::NumStages );
            return m_priorities[(uint8) stage] != (uint8) UpdatePriority::Disabled;
        }

        inline uint8 GetPriorityForStage( UpdateStage stage ) const
        {
            KRG_ASSERT( stage < UpdateStage::NumStages );
            return m_priorities[(uint8) stage];
        }

        inline UpdatePriorityList& SetStagePriority( UpdateStagePriority&& stagePriority )
        {
            m_priorities[(uint8) stagePriority.m_stage] = stagePriority.m_priority;
            return *this;
        }

        // Set a priority for a given stage
        inline UpdatePriorityList& operator<<( UpdateStagePriority&& stagePriority )
        {
            m_priorities[(uint8) stagePriority.m_stage] = stagePriority.m_priority;
            return *this;
        }

        inline bool AreAllStagesDisabled() const
        {
            static uint8 const disabledStages[(int32) UpdateStage::NumStages] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
            static_assert( sizeof( disabledStages ) == sizeof( m_priorities ), "disabled stages must be the same size as the priorities list" );
            return memcmp( m_priorities, disabledStages, sizeof( m_priorities ) ) == 0;
        }

    private:

        uint8           m_priorities[(int32) UpdateStage::NumStages];
    };
}