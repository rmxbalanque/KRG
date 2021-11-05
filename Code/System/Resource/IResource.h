#pragma once

#include "ResourceID.h"

//-------------------------------------------------------------------------
// Base for all KRG resources
//-------------------------------------------------------------------------
// Virtual resources are resources which are produced as side-effects of compiling other resources
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

        inline ResourceID const& GetResourceID() const { return m_resourceID; }
        inline ResourcePath const& GetResourcePath() const { return m_resourceID.GetResourcePath(); }

        virtual bool IsValid() const = 0;
        virtual ResourceTypeID GetResourceType() const = 0;
        virtual bool IsVirtualResourceType() const = 0;

        #if KRG_DEVELOPMENT_TOOLS
        virtual char const* GetFriendlyName() const = 0;
        #endif

    protected:

        IResource() {}

    private:

        ResourceID      m_resourceID;
    };
}

//-------------------------------------------------------------------------

#define KRG_REGISTER_RESOURCE( typeFourCC, friendlyName ) \
    public: \
        static bool const IsVirtualResource = false;\
        static ResourceTypeID GetStaticResourceTypeID() { return ResourceTypeID( typeFourCC ); } \
        virtual ResourceTypeID GetResourceType() const override { return ResourceTypeID( typeFourCC ); } \
        virtual bool IsVirtualResourceType() const override { return false; }\
        KRG_DEVELOPMENT_TOOLS_LINE_IN_MACRO( virtual char const* GetFriendlyName() const override { return friendlyName; } )\
    private:

#define KRG_REGISTER_VIRTUAL_RESOURCE( typeFourCC, friendlyName ) \
    public: \
        static bool const IsVirtualResource = true;\
        static ResourceTypeID GetStaticResourceTypeID() { return ResourceTypeID( typeFourCC ); } \
        virtual ResourceTypeID GetResourceType() const override { return ResourceTypeID( typeFourCC ); } \
        virtual bool IsVirtualResourceType() const override { return true; }\
        KRG_DEVELOPMENT_TOOLS_LINE_IN_MACRO( virtual char const* GetFriendlyName() const override { return friendlyName; } )\
    private: