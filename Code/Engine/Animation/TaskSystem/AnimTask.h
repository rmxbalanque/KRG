#pragma once

#include "AnimTaskPoseStack.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class Task
        {

        public:

            Task( S32 sourceID = InvalidIndex ) : m_taskSourceID( sourceID ) {}
            virtual ~Task() {}
            virtual void Execute( TaskPoseStack& poseStack ) const = 0;
            virtual bool IsPostPhysicsTask() const { return false; }

            #if KRG_DEBUG_INSTRUMENTATION
            virtual S32 GetDependencyCount() const { return 0; }
            virtual Color GetDebugColor() const { return Colors::Yellow; }
            virtual String GetDebugText() const { return String(); }
            #endif

        protected:

            S32 m_taskSourceID;
        };
    }
}
