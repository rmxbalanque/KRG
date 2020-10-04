#pragma once

#include "ResourceID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        class KRG_SYSTEM_RESOURCE_API IResource
        {
        public:

            static ResourceTypeID GetStaticResourceTypeID() { return ResourceTypeID(); }

        public:

            virtual ~IResource() {}

            virtual bool IsValid() const = 0;
            virtual ResourceTypeID GetResourceType() const = 0;

            #if KRG_DEBUG_INSTRUMENTATION
            virtual char const* GetName() const { return "Resource Name"; }
            #endif

        protected:

            IResource() {}
        };
    }
}

//-------------------------------------------------------------------------

#define KRG_REGISTER_RESOURCE( type ) \
    public: \
        static ResourceTypeID GetStaticResourceTypeID() { return ResourceTypeID( type ); } \
        virtual ResourceTypeID GetResourceType() const override { return ResourceTypeID( type ); } \
    private: