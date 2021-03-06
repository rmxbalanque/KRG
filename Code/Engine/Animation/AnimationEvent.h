#pragma once

#include "System/Core/Types/Percentage.h"
#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class Event
        {
        public:

            enum class Type
            {
                Invalid = 0,
                SyncEvent,
                MarkupEvent,
            };

            union Data
            {
                void* m_pData;
                uint32   m_syncID;
            };

        public:

            inline Seconds GetStartTime() const { return m_startTime; }
            inline Seconds GetDuration() const { return m_duration; }
            Type GetType() const { return m_type; }

            inline bool IsSyncEvent() const { return m_type == Type::SyncEvent; }
            inline uint32 GetSyncID() const { KRG_ASSERT( IsSyncEvent() ); return m_data.m_syncID; }

            inline bool IsMarkupEvent() const { return m_type == Type::MarkupEvent; }
            inline void* GetMarkupData() const { KRG_ASSERT( IsMarkupEvent() ); return m_data.m_pData; }

        private:

            Data            m_data;
            Seconds         m_startTime;
            Seconds         m_duration;
            Type            m_type;
        };
    }
}