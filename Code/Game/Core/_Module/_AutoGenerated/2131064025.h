#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 2131064025

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Game\Core\Test\CustomizerTestComponent.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::CustomizerTestComponent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_GAME_CORE_API void serialize( Archive& archive, KRG::CustomizerTestComponent& type )
    {
        archive( cereal::base_class<KRG::EntityComponent>( &type ), KRG_NVP( m_meshIDs ), KRG_NVP( m_armorMeshIDs ), KRG_NVP( m_hairMeshIDs ), KRG_NVP( m_materialIDs ), KRG_NVP( m_animationIDs ), KRG_NVP( m_skeletonID ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::CustomizerTestComponent> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::CustomizerTestComponent const* pActualDefaultTypeInstance = ( KRG::CustomizerTestComponent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_meshIDs" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceID" );
            propertyInfo.m_parentTypeID = 209741102;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_meshIDs;
            propertyInfo.m_offset = offsetof( KRG::CustomizerTestComponent, m_meshIDs );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_meshIDs.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_meshIDs.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::ResourceID );
            propertyInfo.m_size = sizeof( TVector<KRG::ResourceID> );
            propertyInfo.m_flags.Set( 2 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_armorMeshIDs" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceID" );
            propertyInfo.m_parentTypeID = 209741102;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_armorMeshIDs;
            propertyInfo.m_offset = offsetof( KRG::CustomizerTestComponent, m_armorMeshIDs );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_armorMeshIDs.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_armorMeshIDs.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::ResourceID );
            propertyInfo.m_size = sizeof( TVector<KRG::ResourceID> );
            propertyInfo.m_flags.Set( 2 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_hairMeshIDs" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceID" );
            propertyInfo.m_parentTypeID = 209741102;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_hairMeshIDs;
            propertyInfo.m_offset = offsetof( KRG::CustomizerTestComponent, m_hairMeshIDs );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_hairMeshIDs.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_hairMeshIDs.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::ResourceID );
            propertyInfo.m_size = sizeof( TVector<KRG::ResourceID> );
            propertyInfo.m_flags.Set( 2 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_materialIDs" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceID" );
            propertyInfo.m_parentTypeID = 209741102;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_materialIDs;
            propertyInfo.m_offset = offsetof( KRG::CustomizerTestComponent, m_materialIDs );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_materialIDs.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_materialIDs.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::ResourceID );
            propertyInfo.m_size = sizeof( TVector<KRG::ResourceID> );
            propertyInfo.m_flags.Set( 2 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_animationIDs" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceID" );
            propertyInfo.m_parentTypeID = 209741102;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_animationIDs;
            propertyInfo.m_offset = offsetof( KRG::CustomizerTestComponent, m_animationIDs );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_animationIDs.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_animationIDs.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::ResourceID );
            propertyInfo.m_size = sizeof( TVector<KRG::ResourceID> );
            propertyInfo.m_flags.Set( 2 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_skeletonID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceID" );
            propertyInfo.m_parentTypeID = 209741102;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_skeletonID;
            propertyInfo.m_offset = offsetof( KRG::CustomizerTestComponent, m_skeletonID );
            propertyInfo.m_size = sizeof( KRG::ResourceID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_GAME_CORE_API TTypeHelper<KRG::CustomizerTestComponent> : public ITypeHelper
            {
                static TTypeHelper<KRG::CustomizerTestComponent> StaticTypeHelper;

                static void const* DefaultTypeInstancePtr;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::CustomizerTestComponent ), alignof( KRG::CustomizerTestComponent ) );
                    new ( pDefaultTypeInstance ) KRG::CustomizerTestComponent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::CustomizerTestComponent" );
                    typeInfo.m_size = sizeof( KRG::CustomizerTestComponent );
                    typeInfo.m_alignment = alignof( KRG::CustomizerTestComponent );
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

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::CustomizerTestComponent> >( DefaultTypeInstancePtr );
                    KRG::CustomizerTestComponent::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::CustomizerTestComponent" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    reinterpret_cast<KRG::CustomizerTestComponent*>( pDefaultTypeInstance )->~CustomizerTestComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::CustomizerTestComponent>();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pType );
                    if ( arrayID == 1813730029 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_meshIDs.size() )
                        {
                            pActualType->m_meshIDs.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_meshIDs[arrayIdx];
                    }

                    if ( arrayID == 2453801430 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_armorMeshIDs.size() )
                        {
                            pActualType->m_armorMeshIDs.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_armorMeshIDs[arrayIdx];
                    }

                    if ( arrayID == 1010763108 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_hairMeshIDs.size() )
                        {
                            pActualType->m_hairMeshIDs.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_hairMeshIDs[arrayIdx];
                    }

                    if ( arrayID == 363942725 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_materialIDs.size() )
                        {
                            pActualType->m_materialIDs.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_materialIDs[arrayIdx];
                    }

                    if ( arrayID == 1835784676 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_animationIDs.size() )
                        {
                            pActualType->m_animationIDs.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_animationIDs[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

            };
        }
    }
}
