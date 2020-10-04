#pragma once

#include "EventConsumerContext.h"
#include "System\Animation\AnimTypes\AnimPose.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        class Event;

        //-------------------------------------------------------------------------

        class IEventConsumer
        {

        public:

            struct Context
            {
                Context()
                    : pCurrentPose( nullptr )
                {}

                Pose const*     pCurrentPose;
            };


        public:

            IEventConsumer() {}
            virtual ~IEventConsumer() {}

            virtual void ForwardEvent( Event& event ) = 0;
            virtual void Consume( Context const& Ctx ) = 0;
        };
    }
}