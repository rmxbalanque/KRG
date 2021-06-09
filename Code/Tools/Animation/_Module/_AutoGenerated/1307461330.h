#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 1307461330

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Tools\Animation\Events\AnimationEventData.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::EventItem
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::EventItem& type )
    {
        archive( cereal::base_class<KRG::TimelineItem>( &type ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::EventItem> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::EventItem const* pActualDefaultTypeInstance = ( KRG::Animation::EventItem const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::EventItem> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::EventItem> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::EventItem ), alignof( KRG::Animation::EventItem ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::EventItem;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::EventItem" );
                    typeInfo.m_size = sizeof( KRG::Animation::EventItem );
                    typeInfo.m_alignment = alignof( KRG::Animation::EventItem );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::TimelineItem::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    KRG::Animation::EventItem::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::EventItem" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::EventItem*>( pDefaultTypeInstance )->~EventItem();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::EventItem>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::EventItem();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::EventItem*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::EventItem*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventItem*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventItem*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventItem*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventItem*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventItem const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Animation::EventItem*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventItem*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventItem*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::EventItem::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::EventItem const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::EventItem const*>( pOtherTypeInstance );

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::EventItem const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::EventItem const*>( pOtherTypeInstance );

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::EventItem const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::EventItem*>( pTypeInstance );

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::EventTrack
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::EventTrack& type )
    {
        archive( cereal::base_class<KRG::TimelineTrack>( &type ), KRG_NVP( m_eventType ), KRG_NVP( m_isSyncTrack ), KRG_NVP( m_eventTypeID ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::EventTrack> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::EventTrack const* pActualDefaultTypeInstance = ( KRG::Animation::EventTrack const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_eventType" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Animation::Event::EventType" );
            propertyInfo.m_parentTypeID = 1570778114;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_eventType;
            propertyInfo.m_offset = offsetof( KRG::Animation::EventTrack, m_eventType );
            propertyInfo.m_size = sizeof( KRG::Animation::Event::EventType );
            propertyInfo.m_flags.Set( 9 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_isSyncTrack" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "bool" );
            propertyInfo.m_parentTypeID = 1570778114;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_isSyncTrack;
            propertyInfo.m_offset = offsetof( KRG::Animation::EventTrack, m_isSyncTrack );
            propertyInfo.m_size = sizeof( bool );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_eventTypeID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TypeSystem::TypeID" );
            propertyInfo.m_parentTypeID = 1570778114;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_eventTypeID;
            propertyInfo.m_offset = offsetof( KRG::Animation::EventTrack, m_eventTypeID );
            propertyInfo.m_size = sizeof( KRG::TypeSystem::TypeID );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::EventTrack> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::EventTrack> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::EventTrack ), alignof( KRG::Animation::EventTrack ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::EventTrack;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::EventTrack" );
                    typeInfo.m_size = sizeof( KRG::Animation::EventTrack );
                    typeInfo.m_alignment = alignof( KRG::Animation::EventTrack );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::TimelineTrack::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::EventTrack> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::EventTrack::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::EventTrack" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::EventTrack*>( pDefaultTypeInstance )->~EventTrack();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::EventTrack>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::EventTrack();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::EventTrack*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::EventTrack*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventTrack*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventTrack*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventTrack*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventTrack*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventTrack const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Animation::EventTrack*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventTrack*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::EventTrack*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::EventTrack::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::EventTrack const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::EventTrack const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2339630235 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2992962708 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3788823540 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::EventTrack const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::EventTrack const*>( pOtherTypeInstance );

                    if ( propertyID == 2339630235 )
                    {
                        return pType->m_eventType == pOtherType->m_eventType;
                    }

                    if ( propertyID == 2992962708 )
                    {
                        return pType->m_isSyncTrack == pOtherType->m_isSyncTrack;
                    }

                    if ( propertyID == 3788823540 )
                    {
                        return pType->m_eventTypeID == pOtherType->m_eventTypeID;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::EventTrack const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::EventTrack*>( pTypeInstance );

                    if ( propertyID == 2339630235 )
                    {
                        pActualType->m_eventType = pDefaultType->m_eventType;
                        return;
                    }

                    if ( propertyID == 2992962708 )
                    {
                        pActualType->m_isSyncTrack = pDefaultType->m_isSyncTrack;
                        return;
                    }

                    if ( propertyID == 3788823540 )
                    {
                        pActualType->m_eventTypeID = pDefaultType->m_eventTypeID;
                        return;
                    }

                }

            };
        }
    }
}
