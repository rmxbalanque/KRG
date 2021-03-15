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
// TypeHelper: KRG::AnotherTestStruct
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_GAME_CORE_API void serialize( Archive& archive, KRG::AnotherTestStruct& type )
    {
        archive( KRG_NVP( m_dynamicArray ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::AnotherTestStruct> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::AnotherTestStruct const* pActualDefaultTypeInstance = ( KRG::AnotherTestStruct const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_dynamicArray" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 2593117954;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_dynamicArray;
            propertyInfo.m_offset = offsetof( KRG::AnotherTestStruct, m_dynamicArray );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_dynamicArray.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_dynamicArray.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( float );
            propertyInfo.m_size = sizeof( TVector<float> );
            propertyInfo.m_flags.SetFlags( 2 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_GAME_CORE_API TTypeHelper<KRG::AnotherTestStruct> : public ITypeHelper
            {
                static TTypeHelper<KRG::AnotherTestStruct> StaticTypeHelper;

                static void const* DefaultTypeInstancePtr;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::AnotherTestStruct ), alignof( KRG::AnotherTestStruct ) );
                    new ( pDefaultTypeInstance ) KRG::AnotherTestStruct;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::AnotherTestStruct" );
                    typeInfo.m_size = sizeof( KRG::AnotherTestStruct );
                    typeInfo.m_alignment = alignof( KRG::AnotherTestStruct );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::AnotherTestStruct> >( DefaultTypeInstancePtr );
                    KRG::AnotherTestStruct::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::AnotherTestStruct" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    reinterpret_cast<KRG::AnotherTestStruct*>( pDefaultTypeInstance )->~AnotherTestStruct();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::AnotherTestStruct>();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::AnotherTestStruct*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::AnotherTestStruct*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::AnotherTestStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::AnotherTestStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::AnotherTestStruct*>( pType );
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
                    auto pActualType = reinterpret_cast<KRG::AnotherTestStruct*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::TestStruct
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_GAME_CORE_API void serialize( Archive& archive, KRG::TestStruct& type )
    {
        archive( KRG_NVP( m_U8 ), KRG_NVP( m_uint16 ), KRG_NVP( m_U32 ), KRG_NVP( m_U64 ), KRG_NVP( m_UUID ), KRG_NVP( m_eulerAngles ), KRG_NVP( m_dynamicArray ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::TestStruct> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::TestStruct const* pActualDefaultTypeInstance = ( KRG::TestStruct const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_U8" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint8" );
            propertyInfo.m_parentTypeID = 3498272077;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_U8;
            propertyInfo.m_offset = offsetof( KRG::TestStruct, m_U8 );
            propertyInfo.m_size = sizeof( KRG::uint8 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_uint16" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint16" );
            propertyInfo.m_parentTypeID = 3498272077;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_uint16;
            propertyInfo.m_offset = offsetof( KRG::TestStruct, m_uint16 );
            propertyInfo.m_size = sizeof( KRG::uint16 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_U32" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint32" );
            propertyInfo.m_parentTypeID = 3498272077;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_U32;
            propertyInfo.m_offset = offsetof( KRG::TestStruct, m_U32 );
            propertyInfo.m_size = sizeof( KRG::uint32 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_U64" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint64" );
            propertyInfo.m_parentTypeID = 3498272077;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_U64;
            propertyInfo.m_offset = offsetof( KRG::TestStruct, m_U64 );
            propertyInfo.m_size = sizeof( KRG::uint64 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_UUID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 3498272077;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_UUID;
            propertyInfo.m_offset = offsetof( KRG::TestStruct, m_UUID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_eulerAngles" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::EulerAngles" );
            propertyInfo.m_parentTypeID = 3498272077;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_eulerAngles;
            propertyInfo.m_offset = offsetof( KRG::TestStruct, m_eulerAngles );
            propertyInfo.m_size = sizeof( KRG::EulerAngles );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_dynamicArray" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::AnotherTestStruct" );
            propertyInfo.m_parentTypeID = 3498272077;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_dynamicArray;
            propertyInfo.m_offset = offsetof( KRG::TestStruct, m_dynamicArray );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_dynamicArray.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_dynamicArray.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::AnotherTestStruct );
            propertyInfo.m_size = sizeof( TVector<KRG::AnotherTestStruct> );
            propertyInfo.m_flags.SetFlags( 2 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_GAME_CORE_API TTypeHelper<KRG::TestStruct> : public ITypeHelper
            {
                static TTypeHelper<KRG::TestStruct> StaticTypeHelper;

                static void const* DefaultTypeInstancePtr;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::TestStruct ), alignof( KRG::TestStruct ) );
                    new ( pDefaultTypeInstance ) KRG::TestStruct;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::TestStruct" );
                    typeInfo.m_size = sizeof( KRG::TestStruct );
                    typeInfo.m_alignment = alignof( KRG::TestStruct );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::TestStruct> >( DefaultTypeInstancePtr );
                    KRG::TestStruct::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::TestStruct" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    reinterpret_cast<KRG::TestStruct*>( pDefaultTypeInstance )->~TestStruct();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::TestStruct>();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::TestStruct*>( pType );

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        KRG::AnotherTestStruct::StaticTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::TestStruct*>( pType );

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        KRG::AnotherTestStruct::StaticTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        status = KRG::AnotherTestStruct::StaticTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &propertyValue );
                        if ( status == LoadingStatus::Loading )
                        {
                            return LoadingStatus::Loading;
                        }
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestStruct*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    for ( auto& propertyValue : pActualType->m_dynamicArray )
                    {
                        status = KRG::AnotherTestStruct::StaticTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &propertyValue );
                        if ( status != LoadingStatus::Unloaded )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestStruct*>( pType );
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
                    auto pActualType = reinterpret_cast<KRG::TestStruct*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// Enum Helper: KRG::SomeFlags
//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace EnumHelpers
        {
            class EnumHelper_KRG_SomeFlags
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
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_resourceID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceID" );
            propertyInfo.m_parentTypeID = 748499872;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_resourceID;
            propertyInfo.m_offset = offsetof( KRG::TestComponent::InternalStruct, m_resourceID );
            propertyInfo.m_size = sizeof( KRG::ResourceID );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_GAME_CORE_API TTypeHelper<KRG::TestComponent::InternalStruct> : public ITypeHelper
            {
                static TTypeHelper<KRG::TestComponent::InternalStruct> StaticTypeHelper;

                static void const* DefaultTypeInstancePtr;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::TestComponent::InternalStruct ), alignof( KRG::TestComponent::InternalStruct ) );
                    new ( pDefaultTypeInstance ) KRG::TestComponent::InternalStruct;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::TestComponent::InternalStruct" );
                    typeInfo.m_size = sizeof( KRG::TestComponent::InternalStruct );
                    typeInfo.m_alignment = alignof( KRG::TestComponent::InternalStruct );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent::InternalStruct> >( DefaultTypeInstancePtr );
                    KRG::TestComponent::InternalStruct::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::TestComponent::InternalStruct" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    reinterpret_cast<KRG::TestComponent::InternalStruct*>( pDefaultTypeInstance )->~InternalStruct();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::TestComponent::InternalStruct>();
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
// Enum Helper: KRG::TestComponent::Test::InternalEnum
//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace EnumHelpers
        {
            class EnumHelper_KRG_TestComponent_Test_InternalEnum
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
        archive( cereal::base_class<KRG::EntityComponent>( &type ), KRG_NVP( m_bool ), KRG_NVP( m_U8 ), KRG_NVP( m_uint16 ), KRG_NVP( m_U32 ), KRG_NVP( m_U64 ), KRG_NVP( m_S8 ), KRG_NVP( m_S16 ), KRG_NVP( m_S32 ), KRG_NVP( m_S64 ), KRG_NVP( m_F32 ), KRG_NVP( m_F64 ), KRG_NVP( m_Color ), KRG_NVP( m_UUID ), KRG_NVP( m_String ), KRG_NVP( m_StringID ), KRG_NVP( m_DataPath ), KRG_NVP( m_Float2 ), KRG_NVP( m_Float3 ), KRG_NVP( m_Float4 ), KRG_NVP( m_Quaternion ), KRG_NVP( m_Matrix ), KRG_NVP( m_AffineTransform ), KRG_NVP( m_internalEnum ), KRG_NVP( m_testIinternalEnum ), KRG_NVP( m_eulerAngles ), KRG_NVP( m_resourceID ), KRG_NVP( m_struct ), KRG_NVP( m_staticArray ), KRG_NVP( m_dynamicArray ), KRG_NVP( m_staticArrayOfIDs ), KRG_NVP( m_dynamicArrayOfStructs ), KRG_NVP( m_staticArrayOfStructs ), KRG_NVP( m_staticArrayOfEnums ), KRG_NVP( m_genericFlags ), KRG_NVP( m_specificFlags ) );
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
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_U8" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint8" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_U8;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_U8 );
            propertyInfo.m_size = sizeof( KRG::uint8 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_uint16" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint16" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_uint16;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_uint16 );
            propertyInfo.m_size = sizeof( KRG::uint16 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_U32" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint32" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_U32;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_U32 );
            propertyInfo.m_size = sizeof( KRG::uint32 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_U64" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::uint64" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_U64;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_U64 );
            propertyInfo.m_size = sizeof( KRG::uint64 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_S8" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::int8" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_S8;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_S8 );
            propertyInfo.m_size = sizeof( KRG::int8 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_S16" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::int16" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_S16;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_S16 );
            propertyInfo.m_size = sizeof( KRG::int16 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_S32" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::int32" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_S32;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_S32 );
            propertyInfo.m_size = sizeof( KRG::int32 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_S64" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::int64" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_S64;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_S64 );
            propertyInfo.m_size = sizeof( KRG::int64 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_F32" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_F32;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_F32 );
            propertyInfo.m_size = sizeof( float );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_F64" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "double" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_F64;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_F64 );
            propertyInfo.m_size = sizeof( double );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_Color" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Color" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_Color;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_Color );
            propertyInfo.m_size = sizeof( KRG::Color );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_UUID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_UUID;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_UUID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_String" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_String;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_String );
            propertyInfo.m_size = sizeof( KRG::String );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_StringID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_StringID;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_StringID );
            propertyInfo.m_size = sizeof( KRG::StringID );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_DataPath" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::DataPath" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_DataPath;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_DataPath );
            propertyInfo.m_size = sizeof( KRG::DataPath );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_Float2" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float2" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_Float2;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_Float2 );
            propertyInfo.m_size = sizeof( KRG::Float2 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_Float3" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float3" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_Float3;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_Float3 );
            propertyInfo.m_size = sizeof( KRG::Float3 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_Float4" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float4" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_Float4;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_Float4 );
            propertyInfo.m_size = sizeof( KRG::Float4 );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_Quaternion" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Quaternion" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_Quaternion;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_Quaternion );
            propertyInfo.m_size = sizeof( KRG::Quaternion );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_Matrix" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Matrix" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_Matrix;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_Matrix );
            propertyInfo.m_size = sizeof( KRG::Matrix );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_AffineTransform" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Transform" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_AffineTransform;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_AffineTransform );
            propertyInfo.m_size = sizeof( KRG::Transform );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_internalEnum" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TestComponent::InternalEnum" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_internalEnum;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_internalEnum );
            propertyInfo.m_size = sizeof( KRG::TestComponent::InternalEnum );
            propertyInfo.m_flags.SetFlags( 4 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_testIinternalEnum" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TestComponent::Test::InternalEnum" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_testIinternalEnum;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_testIinternalEnum );
            propertyInfo.m_size = sizeof( KRG::TestComponent::Test::InternalEnum );
            propertyInfo.m_flags.SetFlags( 4 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_eulerAngles" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::EulerAngles" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_eulerAngles;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_eulerAngles );
            propertyInfo.m_size = sizeof( KRG::EulerAngles );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_resourceID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceID" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_resourceID;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_resourceID );
            propertyInfo.m_size = sizeof( KRG::ResourceID );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_struct" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TestStruct" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_struct;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_struct );
            propertyInfo.m_size = sizeof( KRG::TestStruct );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

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
            propertyInfo.m_flags.SetFlags( 1 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

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
            propertyInfo.m_flags.SetFlags( 2 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_staticArrayOfIDs" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_staticArrayOfIDs;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_staticArrayOfIDs );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_staticArrayOfIDs;
            propertyInfo.m_arraySize = 4;
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::StringID );
            propertyInfo.m_size = sizeof( KRG::StringID ) * 4;
            propertyInfo.m_flags.SetFlags( 1 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_dynamicArrayOfStructs" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TestStruct" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_dynamicArrayOfStructs;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_dynamicArrayOfStructs );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_dynamicArrayOfStructs.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_dynamicArrayOfStructs.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::TestStruct );
            propertyInfo.m_size = sizeof( TVector<KRG::TestStruct> );
            propertyInfo.m_flags.SetFlags( 2 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

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
            propertyInfo.m_flags.SetFlags( 1 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_staticArrayOfEnums" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TestComponent::Test::InternalEnum" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_staticArrayOfEnums;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_staticArrayOfEnums );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_staticArrayOfEnums;
            propertyInfo.m_arraySize = 6;
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::TestComponent::Test::InternalEnum );
            propertyInfo.m_size = sizeof( KRG::TestComponent::Test::InternalEnum ) * 6;
            propertyInfo.m_flags.SetFlags( 5 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_genericFlags" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::BitFlags" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_genericFlags;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_genericFlags );
            propertyInfo.m_size = sizeof( KRG::BitFlags );
            propertyInfo.m_flags.SetFlags( 8 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_specificFlags" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TBitFlags" );
            propertyInfo.m_parentTypeID = 3100775830;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::SomeFlags" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_specificFlags;
            propertyInfo.m_offset = offsetof( KRG::TestComponent, m_specificFlags );
            propertyInfo.m_size = sizeof( KRG::TBitFlags<KRG::SomeFlags> );
            propertyInfo.m_flags.SetFlags( 8 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_GAME_CORE_API TTypeHelper<KRG::TestComponent> : public ITypeHelper
            {
                static TTypeHelper<KRG::TestComponent> StaticTypeHelper;

                static void const* DefaultTypeInstancePtr;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
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

                    pParentType = KRG::EntityComponent::StaticTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::TestComponent> >( DefaultTypeInstancePtr );
                    KRG::TestComponent::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::TestComponent" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    reinterpret_cast<KRG::TestComponent*>( pDefaultTypeInstance )->~TestComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::TestComponent>();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pType );

                    KRG::TestStruct::StaticTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &pActualType->m_struct );

                    for ( auto& propertyValue : pActualType->m_dynamicArrayOfStructs )
                    {
                        KRG::TestStruct::StaticTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                    KRG::TestComponent::InternalStruct::StaticTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &pActualType->m_staticArrayOfStructs[0] );

                    KRG::TestComponent::InternalStruct::StaticTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &pActualType->m_staticArrayOfStructs[1] );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pType );

                    KRG::TestStruct::StaticTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &pActualType->m_struct );

                    for ( auto& propertyValue : pActualType->m_dynamicArrayOfStructs )
                    {
                        KRG::TestStruct::StaticTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                    KRG::TestComponent::InternalStruct::StaticTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &pActualType->m_staticArrayOfStructs[0] );

                    KRG::TestComponent::InternalStruct::StaticTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &pActualType->m_staticArrayOfStructs[1] );

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    status = KRG::TestStruct::StaticTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &pActualType->m_struct );
                    if ( status == LoadingStatus::Loading )
                    {
                        return LoadingStatus::Loading;
                    }

                    for ( auto& propertyValue : pActualType->m_dynamicArrayOfStructs )
                    {
                        status = KRG::TestStruct::StaticTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &propertyValue );
                        if ( status == LoadingStatus::Loading )
                        {
                            return LoadingStatus::Loading;
                        }
                    }

                    status = KRG::TestComponent::InternalStruct::StaticTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &pActualType->m_staticArrayOfStructs[0] ); 
                    if ( status == LoadingStatus::Loading )
                    {
                        return LoadingStatus::Loading;
                    }

                    status = KRG::TestComponent::InternalStruct::StaticTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &pActualType->m_staticArrayOfStructs[1] ); 
                    if ( status == LoadingStatus::Loading )
                    {
                        return LoadingStatus::Loading;
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::TestComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    status = KRG::TestStruct::StaticTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &pActualType->m_struct );
                    if ( status != LoadingStatus::Unloaded )
                    {
                        return LoadingStatus::Unloading;
                    }

                    for ( auto& propertyValue : pActualType->m_dynamicArrayOfStructs )
                    {
                        status = KRG::TestStruct::StaticTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &propertyValue );
                        if ( status != LoadingStatus::Unloaded )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    status = KRG::TestComponent::InternalStruct::StaticTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &pActualType->m_staticArrayOfStructs[0] ); 
                    if ( status != LoadingStatus::Unloaded )
                    {
                        return LoadingStatus::Unloading;
                    }

                    status = KRG::TestComponent::InternalStruct::StaticTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &pActualType->m_staticArrayOfStructs[1] ); 
                    if ( status != LoadingStatus::Unloaded )
                    {
                        return LoadingStatus::Unloading;
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
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

            };
        }
    }
}
