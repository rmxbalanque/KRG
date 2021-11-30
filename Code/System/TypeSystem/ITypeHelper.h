#pragma once

#include "_Module/API.h"
#include "System/Resource/ResourceTypeID.h"
#include "System/Core/Types/LoadingStatus.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

namespace KRG::Resource { class ResourceSystem; class ResourceRequesterID; }

//-------------------------------------------------------------------------
// Type Helper Interface
//-------------------------------------------------------------------------
// Every registered type will have a type helper associated with it
// This type helper provide an interface to assist with property loading/manipulation

namespace KRG
{
    class IRegisteredType;
}

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class KRG_SYSTEM_TYPESYSTEM_API ITypeHelper
    {
    public:

        virtual IRegisteredType* CreateType() const = 0;
        virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const = 0;
        virtual IRegisteredType const* GetDefaultTypeInstancePtr() const = 0;

        // Resource Helpers
        //-------------------------------------------------------------------------

        virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, Resource::ResourceRequesterID const& requesterID, IRegisteredType* pType ) const = 0;
        virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, Resource::ResourceRequesterID const& requesterID, IRegisteredType* pType ) const = 0;
        virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const = 0;
        virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const = 0;
        virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const = 0;

        // Array helpers
        //-------------------------------------------------------------------------

        virtual Byte* GetArrayElementDataPtr( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const = 0;
        virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const = 0;
        virtual size_t GetArrayElementSize( uint32 arrayID ) const = 0;
        virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const = 0;
        virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const = 0;
        virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const = 0;

        // Default value helpers
        //-------------------------------------------------------------------------

        virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const = 0;
        inline bool AreAllPropertiesSetToDefault( IRegisteredType const* pTypeInstance ) const { return AreAllPropertyValuesEqual( pTypeInstance, GetDefaultTypeInstancePtr() ); }
        virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const = 0;
        inline bool IsPropertyValueSetToDefault( IRegisteredType const* pTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const { return IsPropertyValueEqual( pTypeInstance, GetDefaultTypeInstancePtr(), propertyID, arrayIdx ); }
        virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) = 0;
    };

    //-------------------------------------------------------------------------

    // Concrete type helpers have their own namespace so as to not pollute the TypeSystem one
    namespace TypeHelpers
    {
        template<typename T>
        class KRG_SYSTEM_TYPESYSTEM_API TTypeHelper : public ITypeHelper
        {};
    }
}