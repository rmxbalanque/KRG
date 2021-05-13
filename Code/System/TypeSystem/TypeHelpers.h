#pragma once

#include "_Module/API.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG ::TypeSystem
{
    namespace TypeHelpers
    {
        template<>
        class KRG_SYSTEM_TYPESYSTEM_API TTypeHelper<IRegisteredType> final : public ITypeHelper
        {
            static TTypeHelper<IRegisteredType> StaticTypeHelper;

        public:

            virtual void const* GetDefaultTypeInstancePtr() const override
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

            virtual void* CreateType() const override
            {
                KRG_HALT(); // Error! Trying to instantiate an abstract entity system!
                return nullptr;
            }

            virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override
            {
                KRG_HALT();
            }

            virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override
            {}

            virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override
            {}

            virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override
            {
                return LoadingStatus::Loaded;
            }

            virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override
            {
                return LoadingStatus::Unloaded;
            }

            virtual Byte* GetArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override
            {
                KRG_UNREACHABLE_CODE();
                return nullptr;
            }

            virtual size_t GetArraySize( void const* pTypeInstance, uint32 arrayID ) const override
            {
                KRG_UNREACHABLE_CODE();
                return 0;
            }

            virtual size_t GetArrayElementSize( uint32 arrayID ) const override
            {
                KRG_UNREACHABLE_CODE();
                return 0;
            }

            virtual void ClearArray( void* pTypeInstance, uint32 arrayID ) const override { KRG_UNREACHABLE_CODE(); }
            virtual void AddArrayElement( void* pTypeInstance, uint32 arrayID ) const override { KRG_UNREACHABLE_CODE(); }
            virtual void RemoveArrayElement( void* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override { KRG_UNREACHABLE_CODE(); }

            virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override
            {
                KRG_UNREACHABLE_CODE();
                return ResourceTypeID();
            }

            virtual bool AreAllPropertyValuesEqual( void const* pTypeInstance, void const* pOtherTypeInstance ) const override { return false; }
            virtual bool IsPropertyValueEqual( void const* pTypeInstance, void const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override { return false; }
            virtual void ResetToDefault( void* pTypeInstance, uint32 propertyID ) override {}
        };
    }

    //-------------------------------------------------------------------------

    KRG_SYSTEM_TYPESYSTEM_API void RegisterCoreTypeSystemTypes( TypeRegistry& typeRegistry );
    KRG_SYSTEM_TYPESYSTEM_API void UnregisterCoreTypeSystemTypes( TypeRegistry& typeRegistry );
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