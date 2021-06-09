#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 732180760

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Engine\Animation\Events\AnimationEvent_ID.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::Events::IDEvent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_ENGINE_ANIMATION_API void serialize( Archive& archive, KRG::Animation::Events::IDEvent& type )
    {
        archive( cereal::base_class<KRG::Animation::Event>( &type ), KRG_NVP( m_startTime ), KRG_NVP( m_duration ), KRG_NVP( m_ID ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Events::IDEvent> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::Events::IDEvent const* pActualDefaultTypeInstance = ( KRG::Animation::Events::IDEvent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_startTime" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Seconds" );
            propertyInfo.m_parentTypeID = 4142154412;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_startTime;
            propertyInfo.m_offset = offsetof( KRG::Animation::Events::IDEvent, m_startTime );
            propertyInfo.m_size = sizeof( KRG::Seconds );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_duration" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Seconds" );
            propertyInfo.m_parentTypeID = 4142154412;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_duration;
            propertyInfo.m_offset = offsetof( KRG::Animation::Events::IDEvent, m_duration );
            propertyInfo.m_size = sizeof( KRG::Seconds );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 4142154412;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_ID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Events::IDEvent, m_ID );
            propertyInfo.m_size = sizeof( KRG::StringID );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_ANIMATION_API TTypeHelper<KRG::Animation::Events::IDEvent> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::Events::IDEvent> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::Events::IDEvent ), alignof( KRG::Animation::Events::IDEvent ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::Events::IDEvent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::Events::IDEvent" );
                    typeInfo.m_size = sizeof( KRG::Animation::Events::IDEvent );
                    typeInfo.m_alignment = alignof( KRG::Animation::Events::IDEvent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Animation::Event::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Events::IDEvent> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::Events::IDEvent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::Events::IDEvent" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::Events::IDEvent*>( pDefaultTypeInstance )->~IDEvent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::Events::IDEvent>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::Events::IDEvent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Events::IDEvent*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Events::IDEvent*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Events::IDEvent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Events::IDEvent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Events::IDEvent*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Events::IDEvent*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Events::IDEvent const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Animation::Events::IDEvent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Events::IDEvent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Events::IDEvent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::Events::IDEvent::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::Events::IDEvent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Events::IDEvent const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 642514472 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4243392298 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4230898639 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::Events::IDEvent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Events::IDEvent const*>( pOtherTypeInstance );

                    if ( propertyID == 642514472 )
                    {
                        return pType->m_startTime == pOtherType->m_startTime;
                    }

                    if ( propertyID == 4243392298 )
                    {
                        return pType->m_duration == pOtherType->m_duration;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        return pType->m_ID == pOtherType->m_ID;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::Events::IDEvent const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::Events::IDEvent*>( pTypeInstance );

                    if ( propertyID == 642514472 )
                    {
                        pActualType->m_startTime = pDefaultType->m_startTime;
                        return;
                    }

                    if ( propertyID == 4243392298 )
                    {
                        pActualType->m_duration = pDefaultType->m_duration;
                        return;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        pActualType->m_ID = pDefaultType->m_ID;
                        return;
                    }

                }

            };
        }
    }
}
