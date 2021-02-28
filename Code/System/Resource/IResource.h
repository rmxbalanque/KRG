#pragma once

#include "ResourceID.h"

//-------------------------------------------------------------------------
// Base for all KRG resources
//-------------------------------------------------------------------------
// Virtual resources are resources produces as side-effects of other resources
// e.g. navmesh being generated when compiling a map

namespace KRG::Resource
{
    class KRG_SYSTEM_RESOURCE_API IResource
    {
    public:

        static ResourceTypeID GetStaticResourceTypeID() { return ResourceTypeID(); }

    public:

        virtual ~IResource() {}

        virtual bool IsValid() const = 0;
        virtual ResourceTypeID GetResourceType() const = 0;
        virtual bool IsVirtualResourceType() const = 0;

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetName() const { return "Resource Name"; }
        #endif

    protected:

        IResource() {}
    };
}

//-------------------------------------------------------------------------

#define KRG_REGISTER_RESOURCE( type ) \
    public: \
        static bool const IsVirtualResource = false;\
        static ResourceTypeID GetStaticResourceTypeID() { return ResourceTypeID( type ); } \
        virtual ResourceTypeID GetResourceType() const override { return ResourceTypeID( type ); } \
        virtual bool IsVirtualResourceType() const override { return false; }\
    private:

#define KRG_REGISTER_VIRTUAL_RESOURCE( type ) \
    public: \
        static bool const IsVirtualResource = true;\
        static ResourceTypeID GetStaticResourceTypeID() { return ResourceTypeID( type ); } \
        virtual ResourceTypeID GetResourceType() const override { return ResourceTypeID( type ); } \
        virtual bool IsVirtualResourceType() const override { return true; }\
    private: