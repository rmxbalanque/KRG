#pragma once

#include "_Module/API.h"
#include "System/Resource/ResourceTypeID.h"
#include "System/Core/Types/LoadingStatus.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

namespace KRG::Resource { class ResourceSystem; }

//-------------------------------------------------------------------------
// Type Helper Interface
//-------------------------------------------------------------------------
// Every registered type will have a type helper associated with it
// This type helper provide an interface to assist with property loading/manipulation

namespace KRG::TypeSystem
{
    class KRG_SYSTEM_TYPESYSTEM_API ITypeHelper
    {
    public:

        virtual void* CreateType() const = 0;
        virtual void CreateTypeInPlace( void* pAllocatedMemory ) const = 0;
        virtual void const* GetDefaultTypeInstancePtr() const = 0;

        // Resource Helpers
        //-------------------------------------------------------------------------

        virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& userID, void* pType ) const = 0;
        virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& userID, void* pType ) const = 0;
        virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const = 0;
        virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const = 0;
        virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const = 0;

        // Array helpers
        //-------------------------------------------------------------------------

        virtual Byte* GetArrayElementDataPtr( void* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const = 0;
        virtual size_t GetArraySize( void const* pTypeInstance, uint32 arrayID ) const = 0;
        virtual size_t GetArrayElementSize( uint32 arrayID ) const = 0;
        virtual void ClearArray( void* pTypeInstance, uint32 arrayID ) const = 0;
        virtual void AddArrayElement( void* pTypeInstance, uint32 arrayID ) const = 0;
        virtual void RemoveArrayElement( void* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const = 0;

        // Default value helpers
        //-------------------------------------------------------------------------

        virtual bool AreAllPropertyValuesEqual( void const* pTypeInstance, void const* pOtherTypeInstance ) const = 0;
        inline bool AreAllPropertiesSetToDefault( void const* pTypeInstance ) const { return AreAllPropertyValuesEqual( pTypeInstance, GetDefaultTypeInstancePtr() ); }
        virtual bool IsPropertyValueEqual( void const* pTypeInstance, void const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const = 0;
        inline bool IsPropertyValueSetToDefault( void const* pTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const { return IsPropertyValueEqual( pTypeInstance, GetDefaultTypeInstancePtr(), propertyID, arrayIdx ); }
        virtual void ResetToDefault( void* pTypeInstance, uint32 propertyID ) = 0;
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