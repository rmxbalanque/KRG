#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 1978636638

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "E:\Git\KRG\Code\Engine\Physics\PhysicsMaterial.h"

//-------------------------------------------------------------------------
// Enum Helper: KRG::Physics::PhysicsCombineMode
//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace EnumHelpers
        {
            class EnumHelper_KRG_Physics_PhysicsCombineMode
            {
            public:

                static void RegisterEnum( TypeSystem::TypeRegistry& typeRegistry );
                static void UnregisterEnum( TypeSystem::TypeRegistry& typeRegistry );
            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Physics::PhysicsMaterialSettings
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_ENGINE_PHYSICS_API void serialize( Archive& archive, KRG::Physics::PhysicsMaterialSettings& type )
    {
        archive( cereal::base_class<KRG::IRegisteredType>( &type ), KRG_NVP( m_ID ), KRG_NVP( m_staticFriction ), KRG_NVP( m_dynamicFriction ), KRG_NVP( m_restitution ), KRG_NVP( m_frictionCombineMode ), KRG_NVP( m_restitutionCombineMode ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMaterialSettings> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Physics::PhysicsMaterialSettings const* pActualDefaultTypeInstance = ( KRG::Physics::PhysicsMaterialSettings const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 612453115;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_ID;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMaterialSettings, m_ID );
            propertyInfo.m_size = sizeof( KRG::StringID );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_staticFriction" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 612453115;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_staticFriction;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMaterialSettings, m_staticFriction );
            propertyInfo.m_size = sizeof( float );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_dynamicFriction" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 612453115;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_dynamicFriction;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMaterialSettings, m_dynamicFriction );
            propertyInfo.m_size = sizeof( float );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_restitution" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 612453115;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_restitution;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMaterialSettings, m_restitution );
            propertyInfo.m_size = sizeof( float );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_frictionCombineMode" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Physics::PhysicsCombineMode" );
            propertyInfo.m_parentTypeID = 612453115;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_frictionCombineMode;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMaterialSettings, m_frictionCombineMode );
            propertyInfo.m_size = sizeof( KRG::Physics::PhysicsCombineMode );
            propertyInfo.m_flags.Set( 9 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_restitutionCombineMode" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Physics::PhysicsCombineMode" );
            propertyInfo.m_parentTypeID = 612453115;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_restitutionCombineMode;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMaterialSettings, m_restitutionCombineMode );
            propertyInfo.m_size = sizeof( KRG::Physics::PhysicsCombineMode );
            propertyInfo.m_flags.Set( 9 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_PHYSICS_API TTypeHelper<KRG::Physics::PhysicsMaterialSettings> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Physics::PhysicsMaterialSettings> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Physics::PhysicsMaterialSettings ), alignof( KRG::Physics::PhysicsMaterialSettings ) );
                    new ( pDefaultTypeInstance ) KRG::Physics::PhysicsMaterialSettings;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Physics::PhysicsMaterialSettings" );
                    typeInfo.m_size = sizeof( KRG::Physics::PhysicsMaterialSettings );
                    typeInfo.m_alignment = alignof( KRG::Physics::PhysicsMaterialSettings );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::IRegisteredType::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMaterialSettings> >( s_pDefaultTypeInstancePtr );
                    KRG::Physics::PhysicsMaterialSettings::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Physics::PhysicsMaterialSettings" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Physics::PhysicsMaterialSettings*>( pDefaultTypeInstance )->~PhysicsMaterialSettings();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Physics::PhysicsMaterialSettings>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Physics::PhysicsMaterialSettings();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Physics::PhysicsMaterialSettings::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4230898639 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3791681129 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2513867306 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2226414746 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3844339017 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1096257185 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings const*>( pOtherTypeInstance );

                    if ( propertyID == 4230898639 )
                    {
                        return pType->m_ID == pOtherType->m_ID;
                    }

                    if ( propertyID == 3791681129 )
                    {
                        return pType->m_staticFriction == pOtherType->m_staticFriction;
                    }

                    if ( propertyID == 2513867306 )
                    {
                        return pType->m_dynamicFriction == pOtherType->m_dynamicFriction;
                    }

                    if ( propertyID == 2226414746 )
                    {
                        return pType->m_restitution == pOtherType->m_restitution;
                    }

                    if ( propertyID == 3844339017 )
                    {
                        return pType->m_frictionCombineMode == pOtherType->m_frictionCombineMode;
                    }

                    if ( propertyID == 1096257185 )
                    {
                        return pType->m_restitutionCombineMode == pOtherType->m_restitutionCombineMode;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialSettings*>( pTypeInstance );

                    if ( propertyID == 4230898639 )
                    {
                        pActualType->m_ID = pDefaultType->m_ID;
                        return;
                    }

                    if ( propertyID == 3791681129 )
                    {
                        pActualType->m_staticFriction = pDefaultType->m_staticFriction;
                        return;
                    }

                    if ( propertyID == 2513867306 )
                    {
                        pActualType->m_dynamicFriction = pDefaultType->m_dynamicFriction;
                        return;
                    }

                    if ( propertyID == 2226414746 )
                    {
                        pActualType->m_restitution = pDefaultType->m_restitution;
                        return;
                    }

                    if ( propertyID == 3844339017 )
                    {
                        pActualType->m_frictionCombineMode = pDefaultType->m_frictionCombineMode;
                        return;
                    }

                    if ( propertyID == 1096257185 )
                    {
                        pActualType->m_restitutionCombineMode = pDefaultType->m_restitutionCombineMode;
                        return;
                    }

                }

            };
        }
    }
}
