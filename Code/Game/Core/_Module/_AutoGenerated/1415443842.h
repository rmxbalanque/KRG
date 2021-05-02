#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 1415443842

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Game\Core\TestComponent.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::ExternalTestSubStruct
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_GAME_CORE_API void serialize( Archive& archive, KRG::ExternalTestSubStruct& type )
    {
        archive( KRG_NVP( m_dynamicArray ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestSubStruct> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::ExternalTestSubStruct const* pActualDefaultTypeInstance = ( KRG::ExternalTestSubStruct const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_dynamicArray" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 916469573;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_dynamicArray;
            propertyInfo.m_offset = offsetof( KRG::ExternalTestSubStruct, m_dynamicArray );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_dynamicArray.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_dynamicArray.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( float );
            propertyInfo.m_size = sizeof( TVector<float> );
            propertyInfo.m_flags.Set( 2 );
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

                static void const* s_pDefaultTypeInstancePtr;

            public:

                virtual void const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::ExternalTestSubStruct ), alignof( KRG::ExternalTestSubStruct ) );
                    new ( pDefaultTypeInstance ) KRG::ExternalTestSubStruct;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::ExternalTestSubStruct" );
                    typeInfo.m_size = sizeof( KRG::ExternalTestSubStruct );
                    typeInfo.m_alignment = alignof( KRG::ExternalTestSubStruct );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestSubStruct> >( s_pDefaultTypeInstancePtr );
                    KRG::ExternalTestSubStruct::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::ExternalTestSubStruct" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::ExternalTestSubStruct*>( pDefaultTypeInstance )->~ExternalTestSubStruct();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::ExternalTestSubStruct>();
                }

                virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::ExternalTestSubStruct();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pType );
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

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestSubStruct*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
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
        archive( KRG_NVP( m_uint8 ), KRG_NVP( m_uint16 ), KRG_NVP( m_uint32 ), KRG_NVP( m_U64 ), KRG_NVP( m_UUID ), KRG_NVP( m_eulerAngles ), KRG_NVP( m_dynamicArray ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestStruct> >( void const* pDefaultTypeInstance )
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 2 );
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

                static void const* s_pDefaultTypeInstancePtr;

            public:

                virtual void const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::ExternalTestStruct ), alignof( KRG::ExternalTestStruct ) );
                    new ( pDefaultTypeInstance ) KRG::ExternalTestStruct;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::ExternalTestStruct" );
                    typeInfo.m_size = sizeof( KRG::ExternalTestStruct );
                    typeInfo.m_alignment = alignof( KRG::ExternalTestStruct );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::ExternalTestStruct> >( s_pDefaultTypeInstancePtr );
                    KRG::ExternalTestStruct::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::ExternalTestStruct" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::ExternalTestStruct*>( pDefaultTypeInstance )->~ExternalTestStruct();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::ExternalTestStruct>();
                }

                virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::ExternalTestStruct();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pType );

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        KRG::ExternalTestSubStruct::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pType );

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        KRG::ExternalTestSubStruct::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
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

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
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

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
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

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::ExternalTestStruct*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
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
        archive( KRG_NVP( m_eulerAngles ), KRG_NVP( m_resourceID ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent::InternalStruct> >( void const* pDefaultTypeInstance )
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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

                static void const* s_pDefaultTypeInstancePtr;

            public:

                virtual void const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::TestComponent::InternalStruct ), alignof( KRG::TestComponent::InternalStruct ) );
                    new ( pDefaultTypeInstance ) KRG::TestComponent::InternalStruct;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::TestComponent::InternalStruct" );
                    typeInfo.m_size = sizeof( KRG::TestComponent::InternalStruct );
                    typeInfo.m_alignment = alignof( KRG::TestComponent::InternalStruct );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent::InternalStruct> >( s_pDefaultTypeInstancePtr );
                    KRG::TestComponent::InternalStruct::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::TestComponent::InternalStruct" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::TestComponent::InternalStruct*>( pDefaultTypeInstance )->~InternalStruct();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::TestComponent::InternalStruct>();
                }

                virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::TestComponent::InternalStruct();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pType );
                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent::InternalStruct*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
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
        archive( cereal::base_class<KRG::EntityComponent>( &type ), KRG_NVP( m_bool ), KRG_NVP( m_U8 ), KRG_NVP( m_U16 ), KRG_NVP( m_U32 ), KRG_NVP( m_U64 ), KRG_NVP( m_S8 ), KRG_NVP( m_S16 ), KRG_NVP( m_S32 ), KRG_NVP( m_S64 ), KRG_NVP( m_F32 ), KRG_NVP( m_F64 ), KRG_NVP( m_UUID ), KRG_NVP( m_StringID ), KRG_NVP( m_String ), KRG_NVP( m_Color ), KRG_NVP( m_Float2 ), KRG_NVP( m_Float3 ), KRG_NVP( m_Float4 ), KRG_NVP( m_vector ), KRG_NVP( m_quaternion ), KRG_NVP( m_matrix ), KRG_NVP( m_affineTransform ), KRG_NVP( m_us ), KRG_NVP( m_ms ), KRG_NVP( m_s ), KRG_NVP( m_percentage ), KRG_NVP( m_degrees ), KRG_NVP( m_radians ), KRG_NVP( m_eulerAngles ), KRG_NVP( m_dataPath ), KRG_NVP( m_genericFlags ), KRG_NVP( m_specificFlags ), KRG_NVP( m_resourceTypeID ), KRG_NVP( m_resourceID ), KRG_NVP( m_specificResourcePtr ), KRG_NVP( m_internalEnum ), KRG_NVP( m_testInternalEnum ), KRG_NVP( m_struct0 ), KRG_NVP( m_struct1 ), KRG_NVP( m_staticArray ), KRG_NVP( m_staticArrayOfStringIDs ), KRG_NVP( m_staticArrayOfStructs ), KRG_NVP( m_staticArrayOfEnums ), KRG_NVP( m_dynamicArray ), KRG_NVP( m_dynamicArrayOfStructs ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent> >( void const* pDefaultTypeInstance )
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_dataPath" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::DataPath" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_dataPath;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_dataPath );
            propertyInfo.m_size = sizeof( KRG::DataPath );
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 8 );
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
            propertyInfo.m_flags.Set( 8 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 4 );
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
            propertyInfo.m_flags.Set( 4 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 1 );
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
            propertyInfo.m_flags.Set( 1 );
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
            propertyInfo.m_flags.Set( 1 );
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
            propertyInfo.m_flags.Set( 5 );
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
            propertyInfo.m_flags.Set( 2 );
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
            propertyInfo.m_flags.Set( 2 );
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

                static void const* s_pDefaultTypeInstancePtr;

            public:

                virtual void const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::TestComponent ), alignof( KRG::TestComponent ) );
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

                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::TestComponent*>( pDefaultTypeInstance )->~TestComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::TestComponent>();
                }

                virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::TestComponent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
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

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
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

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    if ( !pActualType->m_specificResourcePtr.IsValid() || pActualType->m_specificResourcePtr.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( pActualType->m_specificResourcePtr.IsUnloaded() || pActualType->m_specificResourcePtr.IsLoading() )
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

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
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

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pType );
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

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
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

            };
        }
    }
}
