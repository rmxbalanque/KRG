#pragma once

#include "_Module/API.h"
#include "ResourcePtr.h"
#include "IResource.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Serialization { class BinaryArchive; }

    //-------------------------------------------------------------------------

    namespace Resource
    {
        using InstallDependencyList = TInlineVector<ResourcePtr, 4>;

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_RESOURCE_API ResourceLoader
        {
        protected:

            inline static ResourcePtr GetInstallDependency( InstallDependencyList const& installDependencies, ResourceID const& resourceID )
            {
                for ( auto const& pInstallDependency : installDependencies )
                {
                    if ( pInstallDependency.GetResourceID() == resourceID )
                    {
                        return pInstallDependency;
                    }
                }

                KRG_HALT();
                return nullptr;
            }

        public:

            TVector<ResourceTypeID> const& GetLoadableTypes() const { return m_loadableTypes; }

            // This function loads is responsible to deserialize the compiled resource data, read the resource header for install dependencies and to create the new runtime resource object
            bool Load( ResourceID const& resourceID, TVector<Byte>& rawData, ResourceRecord* pResourceRecord ) const;

            // This function will destroy the created resource object
            void Unload( ResourceID const& resourceID, ResourceRecord* pResourceRecord ) const;

            // This function is called once all the install dependencies have been loaded, it allows us to update any internal resource ptrs the resource might hold
            virtual bool Install( ResourceID const& resourceID, ResourceRecord* pResourceRecord, InstallDependencyList const& installDependencies ) const { return true; }

            // This function is called as a first step when we are about to unload a resource, it allows us to clean up anything that might require an install dependency to be available
            virtual void Uninstall( ResourceID const& resourceID, ResourceRecord* pResourceRecord ) const {}

        protected:

            // (Required) Override this function to implement you custom deserialization and creation logic, resource header has already been read at this point
            virtual bool LoadInternal( ResourceID const& resourceID, ResourceRecord* pResourceRecord, Serialization::BinaryArchive& archive ) const = 0;

            // (Optional) Override this function to implement any custom object destruction logic if needed, by default this will just delete the created resource
            virtual void UnloadInternal( ResourceID const& resourceID, ResourceRecord* pResourceRecord ) const;

        protected:

            TVector<ResourceTypeID>          m_loadableTypes;
        };
    }
}