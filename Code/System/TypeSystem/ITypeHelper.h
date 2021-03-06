#pragma once

#include "_Module/API.h"
#include "System/Resource/ResourceTypeID.h"
#include "System/Core/Types/LoadingStatus.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource { class ResourceSystem; }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        class KRG_SYSTEM_TYPESYSTEM_API ITypeHelper
        {
        public:

            virtual void* CreateType() const = 0;

            virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& userID, void* pType ) const = 0;
            virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& userID, void* pType ) const = 0;
            virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const = 0;
            virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const = 0;

            virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const = 0;
            virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const = 0;
        };

        //-------------------------------------------------------------------------

        // Concrete type helpers have their own namespace so as to not pollute the TypeSystem one
        namespace TypeHelpers
        {
            template<typename T>
            class KRG_SYSTEM_TYPESYSTEM_API TTypeHelper : public ITypeHelper
            {
            };
        }
    }
}