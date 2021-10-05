#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 1784434565

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "d:\Kruger\Code\Game\Core\PlayerController\DefaultPlayerController.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::DefaultPlayerController
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_GAME_CORE_API void serialize( Archive& archive, KRG::DefaultPlayerController& type )
    {
        archive( cereal::base_class<KRG::IEntitySystem>( &type ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::DefaultPlayerController> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::DefaultPlayerController const* pActualDefaultTypeInstance = ( KRG::DefaultPlayerController const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_GAME_CORE_API TTypeHelper<KRG::DefaultPlayerController> final : public ITypeHelper
            {
                static TTypeHelper<KRG::DefaultPlayerController> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::DefaultPlayerController ), alignof( KRG::DefaultPlayerController ) );
                    new ( pDefaultTypeInstance ) KRG::DefaultPlayerController;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::DefaultPlayerController" );
                    typeInfo.m_size = sizeof( KRG::DefaultPlayerController );
                    typeInfo.m_alignment = alignof( KRG::DefaultPlayerController );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntitySystem );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::IEntitySystem::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    KRG::DefaultPlayerController::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::DefaultPlayerController" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::DefaultPlayerController*>( pDefaultTypeInstance )->~DefaultPlayerController();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::DefaultPlayerController>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::DefaultPlayerController();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::DefaultPlayerController*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::DefaultPlayerController*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::DefaultPlayerController*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::DefaultPlayerController*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::DefaultPlayerController*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::DefaultPlayerController*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::DefaultPlayerController const*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::DefaultPlayerController*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::DefaultPlayerController*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::DefaultPlayerController*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::DefaultPlayerController::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::DefaultPlayerController const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::DefaultPlayerController const*>( pOtherTypeInstance );

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::DefaultPlayerController const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::DefaultPlayerController const*>( pOtherTypeInstance );

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::DefaultPlayerController const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::DefaultPlayerController*>( pTypeInstance );

                }

            };
        }
    }
}
