#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 2511172527

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "E:\Git\KRG\Code\Tools\Physics\ResourceDescriptors\ResourceDescriptor_PhysicsMaterialDatabase.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_PHYSICS_API void serialize( Archive& archive, KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor& type )
    {
        archive( cereal::base_class<KRG::Resource::ResourceDescriptor>( &type ), KRG_NVP( m_materialLibraries ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor const* pActualDefaultTypeInstance = ( KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_materialLibraries" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourcePath" );
            propertyInfo.m_parentTypeID = 1416584488;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_materialLibraries;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor, m_materialLibraries );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_materialLibraries.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_materialLibraries.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::ResourcePath );
            propertyInfo.m_size = sizeof( TVector<KRG::ResourcePath> );
            propertyInfo.m_flags.Set( 5 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_PHYSICS_API TTypeHelper<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor ), alignof( KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor ) );
                    new ( pDefaultTypeInstance ) KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor" );
                    typeInfo.m_size = sizeof( KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor );
                    typeInfo.m_alignment = alignof( KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Resource::ResourceDescriptor::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor> >( s_pDefaultTypeInstancePtr );
                    KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pDefaultTypeInstance )->~PhysicsMaterialDatabaseResourceDescriptor();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pType );

                    if ( arrayID == 3549991386 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_materialLibraries.size() )
                        {
                            pActualType->m_materialLibraries.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_materialLibraries[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor const*>( pTypeInstance );

                    if ( arrayID == 3549991386 )
                    {
                        return pActualType->m_materialLibraries.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 3549991386 )
                    {
                        return sizeof( KRG::ResourcePath );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pTypeInstance );

                    if ( arrayID == 3549991386 )
                    {
                        pActualType->m_materialLibraries.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pTypeInstance );

                    if ( arrayID == 3549991386 )
                    {
                        pActualType->m_materialLibraries.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pTypeInstance );

                    if ( arrayID == 3549991386 )
                    {
                        pActualType->m_materialLibraries.erase( pActualType->m_materialLibraries.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3549991386 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor const*>( pOtherTypeInstance );

                    if ( propertyID == 3549991386 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_materialLibraries.size() )
                            {
                                return false;
                            }

                            return pType->m_materialLibraries[arrayIdx] == pOtherType->m_materialLibraries[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_materialLibraries.size() != pOtherType->m_materialLibraries.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_materialLibraries.size(); i++ )
                            {
                                if( pType->m_materialLibraries[i] != pOtherType->m_materialLibraries[i] )
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
                    auto pDefaultType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pTypeInstance );

                    if ( propertyID == 3549991386 )
                    {
                        pActualType->m_materialLibraries = pDefaultType->m_materialLibraries;
                        return;
                    }

                }

            };
        }
    }
}
