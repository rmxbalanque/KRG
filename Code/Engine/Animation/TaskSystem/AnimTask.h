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

            Task( int32 sourceID = InvalidIndex ) : m_taskSourceID( sourceID ) {}
            virtual ~Task() {}
            virtual void Execute( TaskPoseStack& poseStack ) const = 0;
            virtual bool IsPostPhysicsTask() const { return false; }

            #if KRG_DEVELOPMENT_TOOLS
            virtual int32 GetDependencyCount() const { return 0; }
            virtual Color GetDebugColor() const { return Colors::Yellow; }
            virtual String GetDebugText() const { return String(); }
            #endif

        protected:

            int32 m_taskSourceID;
        };
    }
}
