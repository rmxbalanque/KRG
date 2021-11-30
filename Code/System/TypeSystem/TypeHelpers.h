#pragma once

#include "System/TypeSystem/ITypeHelper.h"
#include "TypeRegistry.h"
#include "TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::TypeHelpers
{
    template<>
    class TTypeHelper<IRegisteredType> final : public ITypeHelper
    {
        static TTypeHelper<IRegisteredType> StaticTypeHelper;

    public:

        virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override
        {
            return nullptr;
        }

        static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
        {
            TypeSystem::TypeInfo typeInfo;
            typeInfo.m_ID = TypeSystem::TypeID( "KRG::IRegisteredType" );
            typeInfo.m_size = sizeof( IRegisteredType );
            typeInfo.m_alignment = alignof( IRegisteredType );
            typeInfo.m_pTypeHelper = &StaticTypeHelper;

            // Register Type
            IRegisteredType::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
        }

        static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
        {
            auto const ID = TypeSystem::TypeID( "KRG::IRegisteredType" );
            typeRegistry.UnregisterType( ID );
        }

        virtual IRegisteredType* CreateType() const override
        {
            KRG_HALT(); // Error! Trying to instantiate an abstract entity system!
            return nullptr;
        }

        virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override
        {
            KRG_HALT();
        }

        virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, Resource::ResourceRequesterID const& requesterID, IRegisteredType* pType ) const override
        {}

        virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, Resource::ResourceRequesterID const& requesterID, IRegisteredType* pType ) const override
        {}

        virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override
        {
            return LoadingStatus::Loaded;
        }

        virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override
        {
            return LoadingStatus::Unloaded;
        }

        virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override
        {
            KRG_UNREACHABLE_CODE();
            return nullptr;
        }

        virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override
        {
            KRG_UNREACHABLE_CODE();
            return 0;
        }

        virtual size_t GetArrayElementSize( uint32 arrayID ) const override
        {
            KRG_UNREACHABLE_CODE();
            return 0;
        }

        virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override { KRG_UNREACHABLE_CODE(); }
        virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override { KRG_UNREACHABLE_CODE(); }
        virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override { KRG_UNREACHABLE_CODE(); }

        virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override
        {
            KRG_UNREACHABLE_CODE();
            return ResourceTypeID();
        }

        virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override { return false; }
        virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override { return false; }
        virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override {}
    };
}

//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    void serialize( Archive& archive, KRG::IRegisteredType& type )
    {
        KRG_HALT(); // Not allowed to be directly serialized
    }
}