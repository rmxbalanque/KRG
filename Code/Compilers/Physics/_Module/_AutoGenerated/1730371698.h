#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 1730371698

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Compilers\Physics\PhysicsMaterialDatabaseCompiler.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_RESOURCECOMPILERS_PHYSICS_API void serialize( Archive& archive, KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor& type )
    {
        archive( cereal::base_class<KRG::Resource::ResourceDescriptor>( &type ), KRG_NVP( m_materialLibraries ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor const* pActualDefaultTypeInstance = ( KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_materialLibraries" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::DataPath" );
            propertyInfo.m_parentTypeID = 1416584488;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_materialLibraries;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor, m_materialLibraries );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_materialLibraries.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_materialLibraries.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::DataPath );
            propertyInfo.m_size = sizeof( TVector<KRG::DataPath> );
            propertyInfo.m_flags.Set( 2 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_RESOURCECOMPILERS_PHYSICS_API TTypeHelper<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor> StaticTypeHelper;

                static void const* s_pDefaultTypeInstancePtr;

            public:

                virtual void const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor ), alignof( KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor ) );
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

                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pDefaultTypeInstance )->~PhysicsMaterialDatabaseResourceDescriptor();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor>();
                }

                virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
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

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMaterialDatabaseResourceDescriptor*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

            };
        }
    }
}
