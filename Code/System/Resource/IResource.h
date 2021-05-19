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
        friend class ResourceLoader;

    public:

        static ResourceTypeID GetStaticResourceTypeID() { return ResourceTypeID(); }

    public:

        virtual ~IResource() {}

        virtual bool IsValid() const = 0;
        virtual ResourceTypeID GetResourceType() const = 0;
        virtual bool IsVirtualResourceType() const = 0;

        inline ResourceID const& GetResourceID() const { return m_resourceID; }

    protected:

        IResource() {}

    private:

        ResourceID      m_resourceID;
    };
}

//-------------------------------------------------------------------------

#define KRG_REGISTER_RESOURCE( typeFourCC ) \
    public: \
        static bool const IsVirtualResource = false;\
        static ResourceTypeID GetStaticResourceTypeID() { return ResourceTypeID( typeFourCC ); } \
        virtual ResourceTypeID GetResourceType() const override { return ResourceTypeID( typeFourCC ); } \
        virtual bool IsVirtualResourceType() const override { return false; }\
    private:

#define KRG_REGISTER_VIRTUAL_RESOURCE( typeFourCC ) \
    public: \
        static bool const IsVirtualResource = true;\
        static ResourceTypeID GetStaticResourceTypeID() { return ResourceTypeID( typeFourCC ); } \
        virtual ResourceTypeID GetResourceType() const override { return ResourceTypeID( typeFourCC ); } \
        virtual bool IsVirtualResourceType() const override { return true; }\
    private: