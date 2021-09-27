#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 2131064025

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "d:\Kruger\Code\Game\Core\Test\CustomizerTestComponent.h"

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
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::CustomizerTestComponent> >( IRegisteredType const* pDefaultTypeInstance )
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
            propertyInfo.m_flags.Set( 5 );
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
            propertyInfo.m_flags.Set( 5 );
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
            propertyInfo.m_flags.Set( 5 );
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
            propertyInfo.m_flags.Set( 5 );
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
            propertyInfo.m_flags.Set( 5 );
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
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_GAME_CORE_API TTypeHelper<KRG::CustomizerTestComponent> final : public ITypeHelper
            {
                static TTypeHelper<KRG::CustomizerTestComponent> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::CustomizerTestComponent ), alignof( KRG::CustomizerTestComponent ) );
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

                    pParentType = KRG::EntityComponent::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::CustomizerTestComponent> >( s_pDefaultTypeInstancePtr );
                    KRG::CustomizerTestComponent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::CustomizerTestComponent" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::CustomizerTestComponent*>( pDefaultTypeInstance )->~CustomizerTestComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::CustomizerTestComponent>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::CustomizerTestComponent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
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

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent const*>( pTypeInstance );

                    if ( arrayID == 1813730029 )
                    {
                        return pActualType->m_meshIDs.size();
                    }

                    if ( arrayID == 2453801430 )
                    {
                        return pActualType->m_armorMeshIDs.size();
                    }

                    if ( arrayID == 1010763108 )
                    {
                        return pActualType->m_hairMeshIDs.size();
                    }

                    if ( arrayID == 363942725 )
                    {
                        return pActualType->m_materialIDs.size();
                    }

                    if ( arrayID == 1835784676 )
                    {
                        return pActualType->m_animationIDs.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 1813730029 )
                    {
                        return sizeof( KRG::ResourceID );
                    }

                    if ( arrayID == 2453801430 )
                    {
                        return sizeof( KRG::ResourceID );
                    }

                    if ( arrayID == 1010763108 )
                    {
                        return sizeof( KRG::ResourceID );
                    }

                    if ( arrayID == 363942725 )
                    {
                        return sizeof( KRG::ResourceID );
                    }

                    if ( arrayID == 1835784676 )
                    {
                        return sizeof( KRG::ResourceID );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pTypeInstance );

                    if ( arrayID == 1813730029 )
                    {
                        pActualType->m_meshIDs.clear();
                        return;
                    }

                    if ( arrayID == 2453801430 )
                    {
                        pActualType->m_armorMeshIDs.clear();
                        return;
                    }

                    if ( arrayID == 1010763108 )
                    {
                        pActualType->m_hairMeshIDs.clear();
                        return;
                    }

                    if ( arrayID == 363942725 )
                    {
                        pActualType->m_materialIDs.clear();
                        return;
                    }

                    if ( arrayID == 1835784676 )
                    {
                        pActualType->m_animationIDs.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pTypeInstance );

                    if ( arrayID == 1813730029 )
                    {
                        pActualType->m_meshIDs.emplace_back();
                        return;
                    }

                    if ( arrayID == 2453801430 )
                    {
                        pActualType->m_armorMeshIDs.emplace_back();
                        return;
                    }

                    if ( arrayID == 1010763108 )
                    {
                        pActualType->m_hairMeshIDs.emplace_back();
                        return;
                    }

                    if ( arrayID == 363942725 )
                    {
                        pActualType->m_materialIDs.emplace_back();
                        return;
                    }

                    if ( arrayID == 1835784676 )
                    {
                        pActualType->m_animationIDs.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pTypeInstance );

                    if ( arrayID == 1813730029 )
                    {
                        pActualType->m_meshIDs.erase( pActualType->m_meshIDs.begin() + arrayIdx );
                        return;
                    }

                    if ( arrayID == 2453801430 )
                    {
                        pActualType->m_armorMeshIDs.erase( pActualType->m_armorMeshIDs.begin() + arrayIdx );
                        return;
                    }

                    if ( arrayID == 1010763108 )
                    {
                        pActualType->m_hairMeshIDs.erase( pActualType->m_hairMeshIDs.begin() + arrayIdx );
                        return;
                    }

                    if ( arrayID == 363942725 )
                    {
                        pActualType->m_materialIDs.erase( pActualType->m_materialIDs.begin() + arrayIdx );
                        return;
                    }

                    if ( arrayID == 1835784676 )
                    {
                        pActualType->m_animationIDs.erase( pActualType->m_animationIDs.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::CustomizerTestComponent::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::CustomizerTestComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::CustomizerTestComponent const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1813730029 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2453801430 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1010763108 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 363942725 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1835784676 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1274071866 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::CustomizerTestComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::CustomizerTestComponent const*>( pOtherTypeInstance );

                    if ( propertyID == 1813730029 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_meshIDs.size() )
                            {
                                return false;
                            }

                            return pType->m_meshIDs[arrayIdx] == pOtherType->m_meshIDs[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_meshIDs.size() != pOtherType->m_meshIDs.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_meshIDs.size(); i++ )
                            {
                                if( pType->m_meshIDs[i] != pOtherType->m_meshIDs[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 2453801430 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_armorMeshIDs.size() )
                            {
                                return false;
                            }

                            return pType->m_armorMeshIDs[arrayIdx] == pOtherType->m_armorMeshIDs[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_armorMeshIDs.size() != pOtherType->m_armorMeshIDs.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_armorMeshIDs.size(); i++ )
                            {
                                if( pType->m_armorMeshIDs[i] != pOtherType->m_armorMeshIDs[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 1010763108 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_hairMeshIDs.size() )
                            {
                                return false;
                            }

                            return pType->m_hairMeshIDs[arrayIdx] == pOtherType->m_hairMeshIDs[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_hairMeshIDs.size() != pOtherType->m_hairMeshIDs.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_hairMeshIDs.size(); i++ )
                            {
                                if( pType->m_hairMeshIDs[i] != pOtherType->m_hairMeshIDs[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 363942725 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_materialIDs.size() )
                            {
                                return false;
                            }

                            return pType->m_materialIDs[arrayIdx] == pOtherType->m_materialIDs[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_materialIDs.size() != pOtherType->m_materialIDs.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_materialIDs.size(); i++ )
                            {
                                if( pType->m_materialIDs[i] != pOtherType->m_materialIDs[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 1835784676 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_animationIDs.size() )
                            {
                                return false;
                            }

                            return pType->m_animationIDs[arrayIdx] == pOtherType->m_animationIDs[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_animationIDs.size() != pOtherType->m_animationIDs.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_animationIDs.size(); i++ )
                            {
                                if( pType->m_animationIDs[i] != pOtherType->m_animationIDs[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 1274071866 )
                    {
                        return pType->m_skeletonID == pOtherType->m_skeletonID;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::CustomizerTestComponent const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::CustomizerTestComponent*>( pTypeInstance );

                    if ( propertyID == 1813730029 )
                    {
                        pActualType->m_meshIDs = pDefaultType->m_meshIDs;
                        return;
                    }

                    if ( propertyID == 2453801430 )
                    {
                        pActualType->m_armorMeshIDs = pDefaultType->m_armorMeshIDs;
                        return;
                    }

                    if ( propertyID == 1010763108 )
                    {
                        pActualType->m_hairMeshIDs = pDefaultType->m_hairMeshIDs;
                        return;
                    }

                    if ( propertyID == 363942725 )
                    {
                        pActualType->m_materialIDs = pDefaultType->m_materialIDs;
                        return;
                    }

                    if ( propertyID == 1835784676 )
                    {
                        pActualType->m_animationIDs = pDefaultType->m_animationIDs;
                        return;
                    }

                    if ( propertyID == 1274071866 )
                    {
                        pActualType->m_skeletonID = pDefaultType->m_skeletonID;
                        return;
                    }

                }

            };
        }
    }
}
