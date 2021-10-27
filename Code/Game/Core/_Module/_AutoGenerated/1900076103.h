#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 1900076103

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "E:\Git\KRG\Code\Game\Core\TestComponent.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::ExternalTestSubSubStruct
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_GAME_CORE_API void serialize( Archive& archive, KRG::ExternalTestSubSubStruct& type )
    {
        archive( cereal::base_class<KRG::IRegisteredType>( &type ), KRG_NVP( m_dynamicArray ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestSubSubStruct> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::ExternalTestSubSubStruct const* pActualDefaultTypeInstance = ( KRG::ExternalTestSubSubStruct const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_dynamicArray" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 2649621769;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_dynamicArray;
            propertyInfo.m_offset = offsetof( KRG::ExternalTestSubSubStruct, m_dynamicArray );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_dynamicArray.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_dynamicArray.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( float );
            propertyInfo.m_size = sizeof( TVector<float> );
            propertyInfo.m_flags.Set( 5 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_GAME_CORE_API TTypeHelper<KRG::ExternalTestSubSubStruct> final : public ITypeHelper
            {
                static TTypeHelper<KRG::ExternalTestSubSubStruct> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::ExternalTestSubSubStruct ), alignof( KRG::ExternalTestSubSubStruct ) );
                    new ( pDefaultTypeInstance ) KRG::ExternalTestSubSubStruct;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::ExternalTestSubSubStruct" );
                    typeInfo.m_size = sizeof( KRG::ExternalTestSubSubStruct );
                    typeInfo.m_alignment = alignof( KRG::ExternalTestSubSubStruct );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::IRegisteredType::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestSubSubStruct> >( s_pDefaultTypeInstancePtr );
                    KRG::ExternalTestSubSubStruct::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::ExternalTestSubSubStruct" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::ExternalTestSubSubStruct*>( pDefaultTypeInstance )->~ExternalTestSubSubStruct();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::ExternalTestSubSubStruct>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::ExternalTestSubSubStruct();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubSubStruct*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubSubStruct*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubSubStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubSubStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubSubStruct*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubSubStruct*>( pType );

                    if ( arrayID == 1528863423 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_dynamicArray.size() )
                        {
                            pActualType->m_dynamicArray.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_dynamicArray[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubSubStruct const*>( pTypeInstance );

                    if ( arrayID == 1528863423 )
                    {
                        return pActualType->m_dynamicArray.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 1528863423 )
                    {
                        return sizeof( float );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubSubStruct*>( pTypeInstance );

                    if ( arrayID == 1528863423 )
                    {
                        pActualType->m_dynamicArray.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubSubStruct*>( pTypeInstance );

                    if ( arrayID == 1528863423 )
                    {
                        pActualType->m_dynamicArray.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubSubStruct*>( pTypeInstance );

                    if ( arrayID == 1528863423 )
                    {
                        pActualType->m_dynamicArray.erase( pActualType->m_dynamicArray.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::ExternalTestSubSubStruct::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::ExternalTestSubSubStruct const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::ExternalTestSubSubStruct const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1528863423 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::ExternalTestSubSubStruct const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::ExternalTestSubSubStruct const*>( pOtherTypeInstance );

                    if ( propertyID == 1528863423 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_dynamicArray.size() )
                            {
                                return false;
                            }

                            return pType->m_dynamicArray[arrayIdx] == pOtherType->m_dynamicArray[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_dynamicArray.size() != pOtherType->m_dynamicArray.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_dynamicArray.size(); i++ )
                            {
                                if( pType->m_dynamicArray[i] != pOtherType->m_dynamicArray[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::ExternalTestSubSubStruct const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubSubStruct*>( pTypeInstance );

                    if ( propertyID == 1528863423 )
                    {
                        pActualType->m_dynamicArray = pDefaultType->m_dynamicArray;
                        return;
                    }

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::ExternalTestSubStruct
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_GAME_CORE_API void serialize( Archive& archive, KRG::ExternalTestSubStruct& type )
    {
        archive( cereal::base_class<KRG::IRegisteredType>( &type ), KRG_NVP( m_floats ), KRG_NVP( m_dynamicArray ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestSubStruct> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::ExternalTestSubStruct const* pActualDefaultTypeInstance = ( KRG::ExternalTestSubStruct const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_floats" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 916469573;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_floats;
            propertyInfo.m_offset = offsetof( KRG::ExternalTestSubStruct, m_floats );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_floats.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_floats.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( float );
            propertyInfo.m_size = sizeof( TVector<float> );
            propertyInfo.m_flags.Set( 5 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_dynamicArray" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ExternalTestSubSubStruct" );
            propertyInfo.m_parentTypeID = 916469573;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_dynamicArray;
            propertyInfo.m_offset = offsetof( KRG::ExternalTestSubStruct, m_dynamicArray );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_dynamicArray.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_dynamicArray.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::ExternalTestSubSubStruct );
            propertyInfo.m_size = sizeof( TVector<KRG::ExternalTestSubSubStruct> );
            propertyInfo.m_flags.Set( 37 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_GAME_CORE_API TTypeHelper<KRG::ExternalTestSubStruct> final : public ITypeHelper
            {
                static TTypeHelper<KRG::ExternalTestSubStruct> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::ExternalTestSubStruct ), alignof( KRG::ExternalTestSubStruct ) );
                    new ( pDefaultTypeInstance ) KRG::ExternalTestSubStruct;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::ExternalTestSubStruct" );
                    typeInfo.m_size = sizeof( KRG::ExternalTestSubStruct );
                    typeInfo.m_alignment = alignof( KRG::ExternalTestSubStruct );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::IRegisteredType::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestSubStruct> >( s_pDefaultTypeInstancePtr );
                    KRG::ExternalTestSubStruct::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::ExternalTestSubStruct" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::ExternalTestSubStruct*>( pDefaultTypeInstance )->~ExternalTestSubStruct();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::ExternalTestSubStruct>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::ExternalTestSubStruct();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pType );

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        KRG::ExternalTestSubSubStruct::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pType );

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        KRG::ExternalTestSubSubStruct::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        status = KRG::ExternalTestSubSubStruct::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &propertyValue );
                        if ( status == LoadingStatus::Loading )
                        {
                            return LoadingStatus::Loading;
                        }
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        status = KRG::ExternalTestSubSubStruct::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &propertyValue );
                        if ( status != LoadingStatus::Unloaded )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pType );

                    if ( arrayID == 2448071977 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_floats.size() )
                        {
                            pActualType->m_floats.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_floats[arrayIdx];
                    }

                    if ( arrayID == 1528863423 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_dynamicArray.size() )
                        {
                            pActualType->m_dynamicArray.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_dynamicArray[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct const*>( pTypeInstance );

                    if ( arrayID == 2448071977 )
                    {
                        return pActualType->m_floats.size();
                    }

                    if ( arrayID == 1528863423 )
                    {
                        return pActualType->m_dynamicArray.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 2448071977 )
                    {
                        return sizeof( float );
                    }

                    if ( arrayID == 1528863423 )
                    {
                        return sizeof( KRG::ExternalTestSubSubStruct );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pTypeInstance );

                    if ( arrayID == 2448071977 )
                    {
                        pActualType->m_floats.clear();
                        return;
                    }

                    if ( arrayID == 1528863423 )
                    {
                        pActualType->m_dynamicArray.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pTypeInstance );

                    if ( arrayID == 2448071977 )
                    {
                        pActualType->m_floats.emplace_back();
                        return;
                    }

                    if ( arrayID == 1528863423 )
                    {
                        pActualType->m_dynamicArray.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pTypeInstance );

                    if ( arrayID == 2448071977 )
                    {
                        pActualType->m_floats.erase( pActualType->m_floats.begin() + arrayIdx );
                        return;
                    }

                    if ( arrayID == 1528863423 )
                    {
                        pActualType->m_dynamicArray.erase( pActualType->m_dynamicArray.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::ExternalTestSubStruct::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::ExternalTestSubStruct const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::ExternalTestSubStruct const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2448071977 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1528863423 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::ExternalTestSubStruct const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::ExternalTestSubStruct const*>( pOtherTypeInstance );

                    if ( propertyID == 2448071977 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_floats.size() )
                            {
                                return false;
                            }

                            return pType->m_floats[arrayIdx] == pOtherType->m_floats[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_floats.size() != pOtherType->m_floats.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_floats.size(); i++ )
                            {
                                if( pType->m_floats[i] != pOtherType->m_floats[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 1528863423 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_dynamicArray.size() )
                            {
                                return false;
                            }

                            return KRG::ExternalTestSubSubStruct::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_dynamicArray[arrayIdx], &pOtherType->m_dynamicArray[arrayIdx] );
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_dynamicArray.size() != pOtherType->m_dynamicArray.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_dynamicArray.size(); i++ )
                            {
                                if( !KRG::ExternalTestSubSubStruct::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_dynamicArray[i], &pOtherType->m_dynamicArray[i] ) )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::ExternalTestSubStruct const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pTypeInstance );

                    if ( propertyID == 2448071977 )
                    {
                        pActualType->m_floats = pDefaultType->m_floats;
                        return;
                    }

                    if ( propertyID == 1528863423 )
                    {
                        pActualType->m_dynamicArray = pDefaultType->m_dynamicArray;
                        return;
                    }

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::ExternalTestStruct
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_GAME_CORE_API void serialize( Archive& archive, KRG::ExternalTestStruct& type )
    {
        archive( cereal::base_class<KRG::IRegisteredType>( &type ), KRG_NVP( m_uint8 ), KRG_NVP( m_uint16 ), KRG_NVP( m_uint32 ), KRG_NVP( m_U64 ), KRG_NVP( m_UUID ), KRG_NVP( m_eulerAngles ), KRG_NVP( m_dynamicArray ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestStruct> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::ExternalTestStruct const* pActualDefaultTypeInstance = ( KRG::ExternalTestStruct const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_uint8" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint8" );
            propertyInfo.m_parentTypeID = 252259265;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_uint8;
            propertyInfo.m_offset = offsetof( KRG::ExternalTestStruct, m_uint8 );
            propertyInfo.m_size = sizeof( KRG::uint8 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_uint16" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint16" );
            propertyInfo.m_parentTypeID = 252259265;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_uint16;
            propertyInfo.m_offset = offsetof( KRG::ExternalTestStruct, m_uint16 );
            propertyInfo.m_size = sizeof( KRG::uint16 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_uint32" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint32" );
            propertyInfo.m_parentTypeID = 252259265;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_uint32;
            propertyInfo.m_offset = offsetof( KRG::ExternalTestStruct, m_uint32 );
            propertyInfo.m_size = sizeof( KRG::uint32 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_U64" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint64" );
            propertyInfo.m_parentTypeID = 252259265;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_U64;
            propertyInfo.m_offset = offsetof( KRG::ExternalTestStruct, m_U64 );
            propertyInfo.m_size = sizeof( KRG::uint64 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_UUID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 252259265;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_UUID;
            propertyInfo.m_offset = offsetof( KRG::ExternalTestStruct, m_UUID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_eulerAngles" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::EulerAngles" );
            propertyInfo.m_parentTypeID = 252259265;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_eulerAngles;
            propertyInfo.m_offset = offsetof( KRG::ExternalTestStruct, m_eulerAngles );
            propertyInfo.m_size = sizeof( KRG::EulerAngles );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_dynamicArray" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ExternalTestSubStruct" );
            propertyInfo.m_parentTypeID = 252259265;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_dynamicArray;
            propertyInfo.m_offset = offsetof( KRG::ExternalTestStruct, m_dynamicArray );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_dynamicArray.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_dynamicArray.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::ExternalTestSubStruct );
            propertyInfo.m_size = sizeof( TVector<KRG::ExternalTestSubStruct> );
            propertyInfo.m_flags.Set( 37 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_GAME_CORE_API TTypeHelper<KRG::ExternalTestStruct> final : public ITypeHelper
            {
                static TTypeHelper<KRG::ExternalTestStruct> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::ExternalTestStruct ), alignof( KRG::ExternalTestStruct ) );
                    new ( pDefaultTypeInstance ) KRG::ExternalTestStruct;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::ExternalTestStruct" );
                    typeInfo.m_size = sizeof( KRG::ExternalTestStruct );
                    typeInfo.m_alignment = alignof( KRG::ExternalTestStruct );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::IRegisteredType::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestStruct> >( s_pDefaultTypeInstancePtr );
                    KRG::ExternalTestStruct::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::ExternalTestStruct" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::ExternalTestStruct*>( pDefaultTypeInstance )->~ExternalTestStruct();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::ExternalTestStruct>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::ExternalTestStruct();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pType );

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        KRG::ExternalTestSubStruct::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pType );

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        KRG::ExternalTestSubStruct::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        status = KRG::ExternalTestSubStruct::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &propertyValue );
                        if ( status == LoadingStatus::Loading )
                        {
                            return LoadingStatus::Loading;
                        }
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        status = KRG::ExternalTestSubStruct::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &propertyValue );
                        if ( status != LoadingStatus::Unloaded )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pType );

                    if ( arrayID == 1528863423 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_dynamicArray.size() )
                        {
                            pActualType->m_dynamicArray.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_dynamicArray[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct const*>( pTypeInstance );

                    if ( arrayID == 1528863423 )
                    {
                        return pActualType->m_dynamicArray.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 1528863423 )
                    {
                        return sizeof( KRG::ExternalTestSubStruct );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pTypeInstance );

                    if ( arrayID == 1528863423 )
                    {
                        pActualType->m_dynamicArray.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pTypeInstance );

                    if ( arrayID == 1528863423 )
                    {
                        pActualType->m_dynamicArray.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pTypeInstance );

                    if ( arrayID == 1528863423 )
                    {
                        pActualType->m_dynamicArray.erase( pActualType->m_dynamicArray.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::ExternalTestStruct::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::ExternalTestStruct const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::ExternalTestStruct const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1639544406 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2692276328 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 705045617 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1099158272 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1271504002 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1455094642 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1528863423 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::ExternalTestStruct const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::ExternalTestStruct const*>( pOtherTypeInstance );

                    if ( propertyID == 1639544406 )
                    {
                        return pType->m_uint8 == pOtherType->m_uint8;
                    }

                    if ( propertyID == 2692276328 )
                    {
                        return pType->m_uint16 == pOtherType->m_uint16;
                    }

                    if ( propertyID == 705045617 )
                    {
                        return pType->m_uint32 == pOtherType->m_uint32;
                    }

                    if ( propertyID == 1099158272 )
                    {
                        return pType->m_U64 == pOtherType->m_U64;
                    }

                    if ( propertyID == 1271504002 )
                    {
                        return pType->m_UUID == pOtherType->m_UUID;
                    }

                    if ( propertyID == 1455094642 )
                    {
                        return pType->m_eulerAngles == pOtherType->m_eulerAngles;
                    }

                    if ( propertyID == 1528863423 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_dynamicArray.size() )
                            {
                                return false;
                            }

                            return KRG::ExternalTestSubStruct::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_dynamicArray[arrayIdx], &pOtherType->m_dynamicArray[arrayIdx] );
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_dynamicArray.size() != pOtherType->m_dynamicArray.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_dynamicArray.size(); i++ )
                            {
                                if( !KRG::ExternalTestSubStruct::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_dynamicArray[i], &pOtherType->m_dynamicArray[i] ) )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::ExternalTestStruct const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pTypeInstance );

                    if ( propertyID == 1639544406 )
                    {
                        pActualType->m_uint8 = pDefaultType->m_uint8;
                        return;
                    }

                    if ( propertyID == 2692276328 )
                    {
                        pActualType->m_uint16 = pDefaultType->m_uint16;
                        return;
                    }

                    if ( propertyID == 705045617 )
                    {
                        pActualType->m_uint32 = pDefaultType->m_uint32;
                        return;
                    }

                    if ( propertyID == 1099158272 )
                    {
                        pActualType->m_U64 = pDefaultType->m_U64;
                        return;
                    }

                    if ( propertyID == 1271504002 )
                    {
                        pActualType->m_UUID = pDefaultType->m_UUID;
                        return;
                    }

                    if ( propertyID == 1455094642 )
                    {
                        pActualType->m_eulerAngles = pDefaultType->m_eulerAngles;
                        return;
                    }

                    if ( propertyID == 1528863423 )
                    {
                        pActualType->m_dynamicArray = pDefaultType->m_dynamicArray;
                        return;
                    }

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// Enum Helper: KRG::TestFlags
//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace EnumHelpers
        {
            class EnumHelper_KRG_TestFlags
            {
            public:

                static void RegisterEnum( TypeSystem::TypeRegistry& typeRegistry );
                static void UnregisterEnum( TypeSystem::TypeRegistry& typeRegistry );
            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::TestComponent::InternalStruct
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_GAME_CORE_API void serialize( Archive& archive, KRG::TestComponent::InternalStruct& type )
    {
        archive( cereal::base_class<KRG::IRegisteredType>( &type ), KRG_NVP( m_eulerAngles ), KRG_NVP( m_resourceID ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent::InternalStruct> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::TestComponent::InternalStruct const* pActualDefaultTypeInstance = ( KRG::TestComponent::InternalStruct const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_eulerAngles" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::EulerAngles" );
            propertyInfo.m_parentTypeID = 748499872;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_eulerAngles;
            propertyInfo.m_offset = offsetof( KRG::TestComponent::InternalStruct, m_eulerAngles );
            propertyInfo.m_size = sizeof( KRG::EulerAngles );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_resourceID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceID" );
            propertyInfo.m_parentTypeID = 748499872;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_resourceID;
            propertyInfo.m_offset = offsetof( KRG::TestComponent::InternalStruct, m_resourceID );
            propertyInfo.m_size = sizeof( KRG::ResourceID );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_GAME_CORE_API TTypeHelper<KRG::TestComponent::InternalStruct> final : public ITypeHelper
            {
                static TTypeHelper<KRG::TestComponent::InternalStruct> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::TestComponent::InternalStruct ), alignof( KRG::TestComponent::InternalStruct ) );
                    new ( pDefaultTypeInstance ) KRG::TestComponent::InternalStruct;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::TestComponent::InternalStruct" );
                    typeInfo.m_size = sizeof( KRG::TestComponent::InternalStruct );
                    typeInfo.m_alignment = alignof( KRG::TestComponent::InternalStruct );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::IRegisteredType::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent::InternalStruct> >( s_pDefaultTypeInstancePtr );
                    KRG::TestComponent::InternalStruct::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::TestComponent::InternalStruct" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::TestComponent::InternalStruct*>( pDefaultTypeInstance )->~InternalStruct();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::TestComponent::InternalStruct>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::TestComponent::InternalStruct();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::TestComponent::InternalStruct const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::TestComponent::InternalStruct const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1455094642 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 159853016 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::TestComponent::InternalStruct const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::TestComponent::InternalStruct const*>( pOtherTypeInstance );

                    if ( propertyID == 1455094642 )
                    {
                        return pType->m_eulerAngles == pOtherType->m_eulerAngles;
                    }

                    if ( propertyID == 159853016 )
                    {
                        return pType->m_resourceID == pOtherType->m_resourceID;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::TestComponent::InternalStruct const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pTypeInstance );

                    if ( propertyID == 1455094642 )
                    {
                        pActualType->m_eulerAngles = pDefaultType->m_eulerAngles;
                        return;
                    }

                    if ( propertyID == 159853016 )
                    {
                        pActualType->m_resourceID = pDefaultType->m_resourceID;
                        return;
                    }

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// Enum Helper: KRG::TestComponent::InternalEnum
//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace EnumHelpers
        {
            class EnumHelper_KRG_TestComponent_InternalEnum
            {
            public:

                static void RegisterEnum( TypeSystem::TypeRegistry& typeRegistry );
                static void UnregisterEnum( TypeSystem::TypeRegistry& typeRegistry );
            };
        }
    }
}

//-------------------------------------------------------------------------
// Enum Helper: KRG::TestComponent::InternalTest::InternalEnum
//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace EnumHelpers
        {
            class EnumHelper_KRG_TestComponent_InternalTest_InternalEnum
            {
            public:

                static void RegisterEnum( TypeSystem::TypeRegistry& typeRegistry );
                static void UnregisterEnum( TypeSystem::TypeRegistry& typeRegistry );
            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::TestComponent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_GAME_CORE_API void serialize( Archive& archive, KRG::TestComponent& type )
    {
        archive( cereal::base_class<KRG::EntityComponent>( &type ), KRG_NVP( m_bool ), KRG_NVP( m_U8 ), KRG_NVP( m_U16 ), KRG_NVP( m_U32 ), KRG_NVP( m_U64 ), KRG_NVP( m_S8 ), KRG_NVP( m_S16 ), KRG_NVP( m_S32 ), KRG_NVP( m_S64 ), KRG_NVP( m_F32 ), KRG_NVP( m_F64 ), KRG_NVP( m_UUID ), KRG_NVP( m_StringID ), KRG_NVP( m_String ), KRG_NVP( m_Color ), KRG_NVP( m_Float2 ), KRG_NVP( m_Float3 ), KRG_NVP( m_Float4 ), KRG_NVP( m_vector ), KRG_NVP( m_quaternion ), KRG_NVP( m_matrix ), KRG_NVP( m_affineTransform ), KRG_NVP( m_us ), KRG_NVP( m_ms ), KRG_NVP( m_s ), KRG_NVP( m_percentage ), KRG_NVP( m_degrees ), KRG_NVP( m_radians ), KRG_NVP( m_eulerAngles ), KRG_NVP( m_resourcePath ), KRG_NVP( m_genericFlags ), KRG_NVP( m_specificFlags ), KRG_NVP( m_resourceTypeID ), KRG_NVP( m_resourceID ), KRG_NVP( m_specificResourcePtr ), KRG_NVP( m_intRange ), KRG_NVP( m_floatRange ), KRG_NVP( m_internalEnum ), KRG_NVP( m_testInternalEnum ), KRG_NVP( m_struct0 ), KRG_NVP( m_struct1 ), KRG_NVP( m_staticArray ), KRG_NVP( m_staticArrayOfStringIDs ), KRG_NVP( m_staticArrayOfStructs ), KRG_NVP( m_staticArrayOfEnums ), KRG_NVP( m_dynamicArray ), KRG_NVP( m_dynamicArrayOfStructs ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::TestComponent const* pActualDefaultTypeInstance = ( KRG::TestComponent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_bool" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "bool" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_bool;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_bool );
            propertyInfo.m_size = sizeof( bool );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_U8" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint8" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_U8;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_U8 );
            propertyInfo.m_size = sizeof( KRG::uint8 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_U16" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint16" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_U16;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_U16 );
            propertyInfo.m_size = sizeof( KRG::uint16 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_U32" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint32" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_U32;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_U32 );
            propertyInfo.m_size = sizeof( KRG::uint32 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_U64" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint64" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_U64;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_U64 );
            propertyInfo.m_size = sizeof( KRG::uint64 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_S8" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::int8" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_S8;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_S8 );
            propertyInfo.m_size = sizeof( KRG::int8 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_S16" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::int16" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_S16;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_S16 );
            propertyInfo.m_size = sizeof( KRG::int16 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_S32" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::int32" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_S32;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_S32 );
            propertyInfo.m_size = sizeof( KRG::int32 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_S64" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::int64" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_S64;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_S64 );
            propertyInfo.m_size = sizeof( KRG::int64 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_F32" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_F32;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_F32 );
            propertyInfo.m_size = sizeof( float );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_F64" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "double" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_F64;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_F64 );
            propertyInfo.m_size = sizeof( double );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_UUID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_UUID;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_UUID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_StringID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_StringID;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_StringID );
            propertyInfo.m_size = sizeof( KRG::StringID );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_String" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_String;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_String );
            propertyInfo.m_size = sizeof( KRG::String );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_Color" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Color" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_Color;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_Color );
            propertyInfo.m_size = sizeof( KRG::Color );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_Float2" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float2" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_Float2;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_Float2 );
            propertyInfo.m_size = sizeof( KRG::Float2 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_Float3" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float3" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_Float3;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_Float3 );
            propertyInfo.m_size = sizeof( KRG::Float3 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_Float4" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float4" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_Float4;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_Float4 );
            propertyInfo.m_size = sizeof( KRG::Float4 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_vector" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Vector" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_vector;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_vector );
            propertyInfo.m_size = sizeof( KRG::Vector );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_quaternion" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Quaternion" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_quaternion;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_quaternion );
            propertyInfo.m_size = sizeof( KRG::Quaternion );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_matrix" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Matrix" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_matrix;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_matrix );
            propertyInfo.m_size = sizeof( KRG::Matrix );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_affineTransform" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Transform" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_affineTransform;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_affineTransform );
            propertyInfo.m_size = sizeof( KRG::Transform );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_us" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Microseconds" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_us;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_us );
            propertyInfo.m_size = sizeof( KRG::Microseconds );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ms" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Milliseconds" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_ms;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_ms );
            propertyInfo.m_size = sizeof( KRG::Milliseconds );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_s" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Seconds" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_s;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_s );
            propertyInfo.m_size = sizeof( KRG::Seconds );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_percentage" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Percentage" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_percentage;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_percentage );
            propertyInfo.m_size = sizeof( KRG::Percentage );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_degrees" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Degrees" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_degrees;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_degrees );
            propertyInfo.m_size = sizeof( KRG::Degrees );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_radians" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Radians" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_radians;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_radians );
            propertyInfo.m_size = sizeof( KRG::Radians );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_eulerAngles" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::EulerAngles" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_eulerAngles;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_eulerAngles );
            propertyInfo.m_size = sizeof( KRG::EulerAngles );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_resourcePath" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourcePath" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_resourcePath;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_resourcePath );
            propertyInfo.m_size = sizeof( KRG::ResourcePath );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_genericFlags" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::BitFlags" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_genericFlags;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_genericFlags );
            propertyInfo.m_size = sizeof( KRG::BitFlags );
            propertyInfo.m_flags.Set( 17 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_specificFlags" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TBitFlags" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::TestFlags" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_specificFlags;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_specificFlags );
            propertyInfo.m_size = sizeof( KRG::TBitFlags<KRG::TestFlags> );
            propertyInfo.m_flags.Set( 17 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_resourceTypeID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceTypeID" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_resourceTypeID;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_resourceTypeID );
            propertyInfo.m_size = sizeof( KRG::ResourceTypeID );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_resourceID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceID" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_resourceID;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_resourceID );
            propertyInfo.m_size = sizeof( KRG::ResourceID );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_specificResourcePtr" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Render::SkeletalMesh" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_specificResourcePtr;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_specificResourcePtr );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Render::SkeletalMesh> );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_intRange" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::IntRange" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_intRange;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_intRange );
            propertyInfo.m_size = sizeof( KRG::IntRange );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_floatRange" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::FloatRange" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_floatRange;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_floatRange );
            propertyInfo.m_size = sizeof( KRG::FloatRange );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_internalEnum" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TestComponent::InternalEnum" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_internalEnum;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_internalEnum );
            propertyInfo.m_size = sizeof( KRG::TestComponent::InternalEnum );
            propertyInfo.m_flags.Set( 9 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_testInternalEnum" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TestComponent::InternalTest::InternalEnum" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_testInternalEnum;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_testInternalEnum );
            propertyInfo.m_size = sizeof( KRG::TestComponent::InternalTest::InternalEnum );
            propertyInfo.m_flags.Set( 9 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_struct0" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ExternalTestStruct" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_struct0;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_struct0 );
            propertyInfo.m_size = sizeof( KRG::ExternalTestStruct );
            propertyInfo.m_flags.Set( 33 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_struct1" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TestComponent::InternalStruct" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_struct1;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_struct1 );
            propertyInfo.m_size = sizeof( KRG::TestComponent::InternalStruct );
            propertyInfo.m_flags.Set( 33 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_staticArray" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_staticArray;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_staticArray );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_staticArray;
            propertyInfo.m_arraySize = 4;
            propertyInfo.m_arrayElementSize = (int32) sizeof( float );
            propertyInfo.m_size = sizeof( float ) * 4;
            propertyInfo.m_flags.Set( 3 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_staticArrayOfStringIDs" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_staticArrayOfStringIDs;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_staticArrayOfStringIDs );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_staticArrayOfStringIDs;
            propertyInfo.m_arraySize = 4;
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::StringID );
            propertyInfo.m_size = sizeof( KRG::StringID ) * 4;
            propertyInfo.m_flags.Set( 3 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_staticArrayOfStructs" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TestComponent::InternalStruct" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_staticArrayOfStructs;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_staticArrayOfStructs );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_staticArrayOfStructs;
            propertyInfo.m_arraySize = 2;
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::TestComponent::InternalStruct );
            propertyInfo.m_size = sizeof( KRG::TestComponent::InternalStruct ) * 2;
            propertyInfo.m_flags.Set( 35 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_staticArrayOfEnums" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TestComponent::InternalTest::InternalEnum" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_staticArrayOfEnums;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_staticArrayOfEnums );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_staticArrayOfEnums;
            propertyInfo.m_arraySize = 6;
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::TestComponent::InternalTest::InternalEnum );
            propertyInfo.m_size = sizeof( KRG::TestComponent::InternalTest::InternalEnum ) * 6;
            propertyInfo.m_flags.Set( 11 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_dynamicArray" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_dynamicArray;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_dynamicArray );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_dynamicArray.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_dynamicArray.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( float );
            propertyInfo.m_size = sizeof( TVector<float> );
            propertyInfo.m_flags.Set( 5 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_dynamicArrayOfStructs" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ExternalTestStruct" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_dynamicArrayOfStructs;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_dynamicArrayOfStructs );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_dynamicArrayOfStructs.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_dynamicArrayOfStructs.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::ExternalTestStruct );
            propertyInfo.m_size = sizeof( TVector<KRG::ExternalTestStruct> );
            propertyInfo.m_flags.Set( 37 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_GAME_CORE_API TTypeHelper<KRG::TestComponent> final : public ITypeHelper
            {
                static TTypeHelper<KRG::TestComponent> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::TestComponent ), alignof( KRG::TestComponent ) );
                    new ( pDefaultTypeInstance ) KRG::TestComponent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::TestComponent" );
                    typeInfo.m_size = sizeof( KRG::TestComponent );
                    typeInfo.m_alignment = alignof( KRG::TestComponent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntityComponent );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::EntityComponent::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent> >( s_pDefaultTypeInstancePtr );
                    KRG::TestComponent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::TestComponent" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::TestComponent*>( pDefaultTypeInstance )->~TestComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::TestComponent>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::TestComponent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pType );

                    if ( pActualType->m_specificResourcePtr.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_specificResourcePtr, requesterID );
                    }

                    KRG::ExternalTestStruct::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &pActualType->m_struct0 );

                    KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &pActualType->m_struct1 );

                    KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &pActualType->m_staticArrayOfStructs[0] );

                    KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &pActualType->m_staticArrayOfStructs[1] );

                    for ( auto& propertyValue : pActualType->m_dynamicArrayOfStructs )
                    {
                        KRG::ExternalTestStruct::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pType );

                    if ( pActualType->m_specificResourcePtr.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_specificResourcePtr, requesterID );
                    }

                    KRG::ExternalTestStruct::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &pActualType->m_struct0 );

                    KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &pActualType->m_struct1 );

                    KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &pActualType->m_staticArrayOfStructs[0] );

                    KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &pActualType->m_staticArrayOfStructs[1] );

                    for ( auto& propertyValue : pActualType->m_dynamicArrayOfStructs )
                    {
                        KRG::ExternalTestStruct::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    if ( !pActualType->m_specificResourcePtr.IsValid() || pActualType->m_specificResourcePtr.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( !pActualType->m_specificResourcePtr.IsLoaded() )
                    {
                        return LoadingStatus::Loading;
                    }

                    status = KRG::ExternalTestStruct::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &pActualType->m_struct0 );
                    if ( status == LoadingStatus::Loading )
                    {
                        return LoadingStatus::Loading;
                    }

                    status = KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &pActualType->m_struct1 );
                    if ( status == LoadingStatus::Loading )
                    {
                        return LoadingStatus::Loading;
                    }

                    status = KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &pActualType->m_staticArrayOfStructs[0] ); 
                    if ( status == LoadingStatus::Loading )
                    {
                        return LoadingStatus::Loading;
                    }

                    status = KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &pActualType->m_staticArrayOfStructs[1] ); 
                    if ( status == LoadingStatus::Loading )
                    {
                        return LoadingStatus::Loading;
                    }

                    for ( auto& propertyValue : pActualType->m_dynamicArrayOfStructs )
                    {
                        status = KRG::ExternalTestStruct::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &propertyValue );
                        if ( status == LoadingStatus::Loading )
                        {
                            return LoadingStatus::Loading;
                        }
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    KRG_ASSERT( !pActualType->m_specificResourcePtr.IsLoading() );
                    if ( !pActualType->m_specificResourcePtr.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    status = KRG::ExternalTestStruct::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &pActualType->m_struct0 );
                    if ( status != LoadingStatus::Unloaded )
                    {
                        return LoadingStatus::Unloading;
                    }

                    status = KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &pActualType->m_struct1 );
                    if ( status != LoadingStatus::Unloaded )
                    {
                        return LoadingStatus::Unloading;
                    }

                    status = KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &pActualType->m_staticArrayOfStructs[0] ); 
                    if ( status != LoadingStatus::Unloaded )
                    {
                        return LoadingStatus::Unloading;
                    }

                    status = KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &pActualType->m_staticArrayOfStructs[1] ); 
                    if ( status != LoadingStatus::Unloaded )
                    {
                        return LoadingStatus::Unloading;
                    }

                    for ( auto& propertyValue : pActualType->m_dynamicArrayOfStructs )
                    {
                        status = KRG::ExternalTestStruct::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &propertyValue );
                        if ( status != LoadingStatus::Unloaded )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pType );
                    if ( propertyID == 3104453760 )
                    {
                        return KRG::Render::SkeletalMesh::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pType );

                    if ( arrayID == 2004964571 )
                    {
                        return (Byte*) &pActualType->m_staticArray[arrayIdx];
                    }

                    if ( arrayID == 2137117432 )
                    {
                        return (Byte*) &pActualType->m_staticArrayOfStringIDs[arrayIdx];
                    }

                    if ( arrayID == 3178258553 )
                    {
                        return (Byte*) &pActualType->m_staticArrayOfStructs[arrayIdx];
                    }

                    if ( arrayID == 1833056808 )
                    {
                        return (Byte*) &pActualType->m_staticArrayOfEnums[arrayIdx];
                    }

                    if ( arrayID == 1528863423 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_dynamicArray.size() )
                        {
                            pActualType->m_dynamicArray.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_dynamicArray[arrayIdx];
                    }

                    if ( arrayID == 3640519683 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_dynamicArrayOfStructs.size() )
                        {
                            pActualType->m_dynamicArrayOfStructs.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_dynamicArrayOfStructs[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent const*>( pTypeInstance );

                    if ( arrayID == 2004964571 )
                    {
                        return 4;
                    }

                    if ( arrayID == 2137117432 )
                    {
                        return 4;
                    }

                    if ( arrayID == 3178258553 )
                    {
                        return 2;
                    }

                    if ( arrayID == 1833056808 )
                    {
                        return 6;
                    }

                    if ( arrayID == 1528863423 )
                    {
                        return pActualType->m_dynamicArray.size();
                    }

                    if ( arrayID == 3640519683 )
                    {
                        return pActualType->m_dynamicArrayOfStructs.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 2004964571 )
                    {
                        return sizeof( float );
                    }

                    if ( arrayID == 2137117432 )
                    {
                        return sizeof( KRG::StringID );
                    }

                    if ( arrayID == 3178258553 )
                    {
                        return sizeof( KRG::TestComponent::InternalStruct );
                    }

                    if ( arrayID == 1833056808 )
                    {
                        return sizeof( KRG::TestComponent::InternalTest::InternalEnum );
                    }

                    if ( arrayID == 1528863423 )
                    {
                        return sizeof( float );
                    }

                    if ( arrayID == 3640519683 )
                    {
                        return sizeof( KRG::ExternalTestStruct );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pTypeInstance );

                    if ( arrayID == 1528863423 )
                    {
                        pActualType->m_dynamicArray.clear();
                        return;
                    }

                    if ( arrayID == 3640519683 )
                    {
                        pActualType->m_dynamicArrayOfStructs.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pTypeInstance );

                    if ( arrayID == 1528863423 )
                    {
                        pActualType->m_dynamicArray.emplace_back();
                        return;
                    }

                    if ( arrayID == 3640519683 )
                    {
                        pActualType->m_dynamicArrayOfStructs.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pTypeInstance );

                    if ( arrayID == 1528863423 )
                    {
                        pActualType->m_dynamicArray.erase( pActualType->m_dynamicArray.begin() + arrayIdx );
                        return;
                    }

                    if ( arrayID == 3640519683 )
                    {
                        pActualType->m_dynamicArrayOfStructs.erase( pActualType->m_dynamicArrayOfStructs.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::TestComponent::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::TestComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::TestComponent const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1949576814 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2477887483 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1290600528 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 349389720 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1099158272 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3492112075 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2238197494 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 590949444 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1658055241 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2779447547 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 82971377 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1271504002 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2799478371 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1137073394 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3492891095 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2909786051 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 705098768 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3165776718 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 938393238 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3789540716 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2322373324 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3833515037 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2633518375 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3313859714 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1868635034 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1669746482 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4027095241 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1083696271 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1455094642 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1580011022 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1147727897 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 887644177 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3111215471 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 159853016 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3104453760 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1475840709 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2711007718 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3763518750 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4189160738 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2468056801 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1934151689 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2004964571 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2137117432 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3178258553 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1833056808 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1528863423 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3640519683 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::TestComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::TestComponent const*>( pOtherTypeInstance );

                    if ( propertyID == 1949576814 )
                    {
                        return pType->m_bool == pOtherType->m_bool;
                    }

                    if ( propertyID == 2477887483 )
                    {
                        return pType->m_U8 == pOtherType->m_U8;
                    }

                    if ( propertyID == 1290600528 )
                    {
                        return pType->m_U16 == pOtherType->m_U16;
                    }

                    if ( propertyID == 349389720 )
                    {
                        return pType->m_U32 == pOtherType->m_U32;
                    }

                    if ( propertyID == 1099158272 )
                    {
                        return pType->m_U64 == pOtherType->m_U64;
                    }

                    if ( propertyID == 3492112075 )
                    {
                        return pType->m_S8 == pOtherType->m_S8;
                    }

                    if ( propertyID == 2238197494 )
                    {
                        return pType->m_S16 == pOtherType->m_S16;
                    }

                    if ( propertyID == 590949444 )
                    {
                        return pType->m_S32 == pOtherType->m_S32;
                    }

                    if ( propertyID == 1658055241 )
                    {
                        return pType->m_S64 == pOtherType->m_S64;
                    }

                    if ( propertyID == 2779447547 )
                    {
                        return pType->m_F32 == pOtherType->m_F32;
                    }

                    if ( propertyID == 82971377 )
                    {
                        return pType->m_F64 == pOtherType->m_F64;
                    }

                    if ( propertyID == 1271504002 )
                    {
                        return pType->m_UUID == pOtherType->m_UUID;
                    }

                    if ( propertyID == 2799478371 )
                    {
                        return pType->m_StringID == pOtherType->m_StringID;
                    }

                    if ( propertyID == 1137073394 )
                    {
                        return pType->m_String == pOtherType->m_String;
                    }

                    if ( propertyID == 3492891095 )
                    {
                        return pType->m_Color == pOtherType->m_Color;
                    }

                    if ( propertyID == 2909786051 )
                    {
                        return pType->m_Float2 == pOtherType->m_Float2;
                    }

                    if ( propertyID == 705098768 )
                    {
                        return pType->m_Float3 == pOtherType->m_Float3;
                    }

                    if ( propertyID == 3165776718 )
                    {
                        return pType->m_Float4 == pOtherType->m_Float4;
                    }

                    if ( propertyID == 938393238 )
                    {
                        return pType->m_vector == pOtherType->m_vector;
                    }

                    if ( propertyID == 3789540716 )
                    {
                        return pType->m_quaternion == pOtherType->m_quaternion;
                    }

                    if ( propertyID == 2322373324 )
                    {
                        return pType->m_matrix == pOtherType->m_matrix;
                    }

                    if ( propertyID == 3833515037 )
                    {
                        return pType->m_affineTransform == pOtherType->m_affineTransform;
                    }

                    if ( propertyID == 2633518375 )
                    {
                        return pType->m_us == pOtherType->m_us;
                    }

                    if ( propertyID == 3313859714 )
                    {
                        return pType->m_ms == pOtherType->m_ms;
                    }

                    if ( propertyID == 1868635034 )
                    {
                        return pType->m_s == pOtherType->m_s;
                    }

                    if ( propertyID == 1669746482 )
                    {
                        return pType->m_percentage == pOtherType->m_percentage;
                    }

                    if ( propertyID == 4027095241 )
                    {
                        return pType->m_degrees == pOtherType->m_degrees;
                    }

                    if ( propertyID == 1083696271 )
                    {
                        return pType->m_radians == pOtherType->m_radians;
                    }

                    if ( propertyID == 1455094642 )
                    {
                        return pType->m_eulerAngles == pOtherType->m_eulerAngles;
                    }

                    if ( propertyID == 1580011022 )
                    {
                        return pType->m_resourcePath == pOtherType->m_resourcePath;
                    }

                    if ( propertyID == 1147727897 )
                    {
                        return pType->m_genericFlags == pOtherType->m_genericFlags;
                    }

                    if ( propertyID == 887644177 )
                    {
                        return pType->m_specificFlags == pOtherType->m_specificFlags;
                    }

                    if ( propertyID == 3111215471 )
                    {
                        return pType->m_resourceTypeID == pOtherType->m_resourceTypeID;
                    }

                    if ( propertyID == 159853016 )
                    {
                        return pType->m_resourceID == pOtherType->m_resourceID;
                    }

                    if ( propertyID == 3104453760 )
                    {
                        return pType->m_specificResourcePtr == pOtherType->m_specificResourcePtr;
                    }

                    if ( propertyID == 1475840709 )
                    {
                        return pType->m_intRange == pOtherType->m_intRange;
                    }

                    if ( propertyID == 2711007718 )
                    {
                        return pType->m_floatRange == pOtherType->m_floatRange;
                    }

                    if ( propertyID == 3763518750 )
                    {
                        return pType->m_internalEnum == pOtherType->m_internalEnum;
                    }

                    if ( propertyID == 4189160738 )
                    {
                        return pType->m_testInternalEnum == pOtherType->m_testInternalEnum;
                    }

                    if ( propertyID == 2468056801 )
                    {
                        return KRG::ExternalTestStruct::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_struct0, &pOtherType->m_struct0 );
                    }

                    if ( propertyID == 1934151689 )
                    {
                        return KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_struct1, &pOtherType->m_struct1 );
                    }

                    if ( propertyID == 2004964571 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            return pType->m_staticArray[arrayIdx] == pOtherType->m_staticArray[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            for ( size_t i = 0; i < 4; i++ )
                            {
                                if( pType->m_staticArray[i] != pOtherType->m_staticArray[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 2137117432 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            return pType->m_staticArrayOfStringIDs[arrayIdx] == pOtherType->m_staticArrayOfStringIDs[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            for ( size_t i = 0; i < 4; i++ )
                            {
                                if( pType->m_staticArrayOfStringIDs[i] != pOtherType->m_staticArrayOfStringIDs[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 3178258553 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            return KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_staticArrayOfStructs[arrayIdx], &pOtherType->m_staticArrayOfStructs[arrayIdx] );
                        }
                        else // Compare entire array contents
                        {
                            for ( size_t i = 0; i < 2; i++ )
                            {
                                if( !KRG::TestComponent::InternalStruct::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_staticArrayOfStructs[i], &pOtherType->m_staticArrayOfStructs[i] ) )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 1833056808 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            return pType->m_staticArrayOfEnums[arrayIdx] == pOtherType->m_staticArrayOfEnums[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            for ( size_t i = 0; i < 6; i++ )
                            {
                                if( pType->m_staticArrayOfEnums[i] != pOtherType->m_staticArrayOfEnums[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 1528863423 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_dynamicArray.size() )
                            {
                                return false;
                            }

                            return pType->m_dynamicArray[arrayIdx] == pOtherType->m_dynamicArray[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_dynamicArray.size() != pOtherType->m_dynamicArray.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_dynamicArray.size(); i++ )
                            {
                                if( pType->m_dynamicArray[i] != pOtherType->m_dynamicArray[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 3640519683 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_dynamicArrayOfStructs.size() )
                            {
                                return false;
                            }

                            return KRG::ExternalTestStruct::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_dynamicArrayOfStructs[arrayIdx], &pOtherType->m_dynamicArrayOfStructs[arrayIdx] );
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_dynamicArrayOfStructs.size() != pOtherType->m_dynamicArrayOfStructs.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_dynamicArrayOfStructs.size(); i++ )
                            {
                                if( !KRG::ExternalTestStruct::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_dynamicArrayOfStructs[i], &pOtherType->m_dynamicArrayOfStructs[i] ) )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::TestComponent const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pTypeInstance );

                    if ( propertyID == 1949576814 )
                    {
                        pActualType->m_bool = pDefaultType->m_bool;
                        return;
                    }

                    if ( propertyID == 2477887483 )
                    {
                        pActualType->m_U8 = pDefaultType->m_U8;
                        return;
                    }

                    if ( propertyID == 1290600528 )
                    {
                        pActualType->m_U16 = pDefaultType->m_U16;
                        return;
                    }

                    if ( propertyID == 349389720 )
                    {
                        pActualType->m_U32 = pDefaultType->m_U32;
                        return;
                    }

                    if ( propertyID == 1099158272 )
                    {
                        pActualType->m_U64 = pDefaultType->m_U64;
                        return;
                    }

                    if ( propertyID == 3492112075 )
                    {
                        pActualType->m_S8 = pDefaultType->m_S8;
                        return;
                    }

                    if ( propertyID == 2238197494 )
                    {
                        pActualType->m_S16 = pDefaultType->m_S16;
                        return;
                    }

                    if ( propertyID == 590949444 )
                    {
                        pActualType->m_S32 = pDefaultType->m_S32;
                        return;
                    }

                    if ( propertyID == 1658055241 )
                    {
                        pActualType->m_S64 = pDefaultType->m_S64;
                        return;
                    }

                    if ( propertyID == 2779447547 )
                    {
                        pActualType->m_F32 = pDefaultType->m_F32;
                        return;
                    }

                    if ( propertyID == 82971377 )
                    {
                        pActualType->m_F64 = pDefaultType->m_F64;
                        return;
                    }

                    if ( propertyID == 1271504002 )
                    {
                        pActualType->m_UUID = pDefaultType->m_UUID;
                        return;
                    }

                    if ( propertyID == 2799478371 )
                    {
                        pActualType->m_StringID = pDefaultType->m_StringID;
                        return;
                    }

                    if ( propertyID == 1137073394 )
                    {
                        pActualType->m_String = pDefaultType->m_String;
                        return;
                    }

                    if ( propertyID == 3492891095 )
                    {
                        pActualType->m_Color = pDefaultType->m_Color;
                        return;
                    }

                    if ( propertyID == 2909786051 )
                    {
                        pActualType->m_Float2 = pDefaultType->m_Float2;
                        return;
                    }

                    if ( propertyID == 705098768 )
                    {
                        pActualType->m_Float3 = pDefaultType->m_Float3;
                        return;
                    }

                    if ( propertyID == 3165776718 )
                    {
                        pActualType->m_Float4 = pDefaultType->m_Float4;
                        return;
                    }

                    if ( propertyID == 938393238 )
                    {
                        pActualType->m_vector = pDefaultType->m_vector;
                        return;
                    }

                    if ( propertyID == 3789540716 )
                    {
                        pActualType->m_quaternion = pDefaultType->m_quaternion;
                        return;
                    }

                    if ( propertyID == 2322373324 )
                    {
                        pActualType->m_matrix = pDefaultType->m_matrix;
                        return;
                    }

                    if ( propertyID == 3833515037 )
                    {
                        pActualType->m_affineTransform = pDefaultType->m_affineTransform;
                        return;
                    }

                    if ( propertyID == 2633518375 )
                    {
                        pActualType->m_us = pDefaultType->m_us;
                        return;
                    }

                    if ( propertyID == 3313859714 )
                    {
                        pActualType->m_ms = pDefaultType->m_ms;
                        return;
                    }

                    if ( propertyID == 1868635034 )
                    {
                        pActualType->m_s = pDefaultType->m_s;
                        return;
                    }

                    if ( propertyID == 1669746482 )
                    {
                        pActualType->m_percentage = pDefaultType->m_percentage;
                        return;
                    }

                    if ( propertyID == 4027095241 )
                    {
                        pActualType->m_degrees = pDefaultType->m_degrees;
                        return;
                    }

                    if ( propertyID == 1083696271 )
                    {
                        pActualType->m_radians = pDefaultType->m_radians;
                        return;
                    }

                    if ( propertyID == 1455094642 )
                    {
                        pActualType->m_eulerAngles = pDefaultType->m_eulerAngles;
                        return;
                    }

                    if ( propertyID == 1580011022 )
                    {
                        pActualType->m_resourcePath = pDefaultType->m_resourcePath;
                        return;
                    }

                    if ( propertyID == 1147727897 )
                    {
                        pActualType->m_genericFlags = pDefaultType->m_genericFlags;
                        return;
                    }

                    if ( propertyID == 887644177 )
                    {
                        pActualType->m_specificFlags = pDefaultType->m_specificFlags;
                        return;
                    }

                    if ( propertyID == 3111215471 )
                    {
                        pActualType->m_resourceTypeID = pDefaultType->m_resourceTypeID;
                        return;
                    }

                    if ( propertyID == 159853016 )
                    {
                        pActualType->m_resourceID = pDefaultType->m_resourceID;
                        return;
                    }

                    if ( propertyID == 3104453760 )
                    {
                        pActualType->m_specificResourcePtr = pDefaultType->m_specificResourcePtr;
                        return;
                    }

                    if ( propertyID == 1475840709 )
                    {
                        pActualType->m_intRange = pDefaultType->m_intRange;
                        return;
                    }

                    if ( propertyID == 2711007718 )
                    {
                        pActualType->m_floatRange = pDefaultType->m_floatRange;
                        return;
                    }

                    if ( propertyID == 3763518750 )
                    {
                        pActualType->m_internalEnum = pDefaultType->m_internalEnum;
                        return;
                    }

                    if ( propertyID == 4189160738 )
                    {
                        pActualType->m_testInternalEnum = pDefaultType->m_testInternalEnum;
                        return;
                    }

                    if ( propertyID == 2468056801 )
                    {
                        pActualType->m_struct0 = pDefaultType->m_struct0;
                        return;
                    }

                    if ( propertyID == 1934151689 )
                    {
                        pActualType->m_struct1 = pDefaultType->m_struct1;
                        return;
                    }

                    if ( propertyID == 2004964571 )
                    {
                        pActualType->m_staticArray[0] = pDefaultType->m_staticArray[0];
                        pActualType->m_staticArray[1] = pDefaultType->m_staticArray[1];
                        pActualType->m_staticArray[2] = pDefaultType->m_staticArray[2];
                        pActualType->m_staticArray[3] = pDefaultType->m_staticArray[3];
                        return;
                    }

                    if ( propertyID == 2137117432 )
                    {
                        pActualType->m_staticArrayOfStringIDs[0] = pDefaultType->m_staticArrayOfStringIDs[0];
                        pActualType->m_staticArrayOfStringIDs[1] = pDefaultType->m_staticArrayOfStringIDs[1];
                        pActualType->m_staticArrayOfStringIDs[2] = pDefaultType->m_staticArrayOfStringIDs[2];
                        pActualType->m_staticArrayOfStringIDs[3] = pDefaultType->m_staticArrayOfStringIDs[3];
                        return;
                    }

                    if ( propertyID == 3178258553 )
                    {
                        pActualType->m_staticArrayOfStructs[0] = pDefaultType->m_staticArrayOfStructs[0];
                        pActualType->m_staticArrayOfStructs[1] = pDefaultType->m_staticArrayOfStructs[1];
                        return;
                    }

                    if ( propertyID == 1833056808 )
                    {
                        pActualType->m_staticArrayOfEnums[0] = pDefaultType->m_staticArrayOfEnums[0];
                        pActualType->m_staticArrayOfEnums[1] = pDefaultType->m_staticArrayOfEnums[1];
                        pActualType->m_staticArrayOfEnums[2] = pDefaultType->m_staticArrayOfEnums[2];
                        pActualType->m_staticArrayOfEnums[3] = pDefaultType->m_staticArrayOfEnums[3];
                        pActualType->m_staticArrayOfEnums[4] = pDefaultType->m_staticArrayOfEnums[4];
                        pActualType->m_staticArrayOfEnums[5] = pDefaultType->m_staticArrayOfEnums[5];
                        return;
                    }

                    if ( propertyID == 1528863423 )
                    {
                        pActualType->m_dynamicArray = pDefaultType->m_dynamicArray;
                        return;
                    }

                    if ( propertyID == 3640519683 )
                    {
                        pActualType->m_dynamicArrayOfStructs = pDefaultType->m_dynamicArrayOfStructs;
                        return;
                    }

                }

            };
        }
    }
}
