#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 2261760404

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Compilers\Render\Shaders\ShaderCompiler.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Render::ShaderResourceDescriptor
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_RESOURCECOMPILERS_RENDER_API void serialize( Archive& archive, KRG::Render::ShaderResourceDescriptor& type )
    {
        archive( cereal::base_class<KRG::Resource::ResourceDescriptor>( &type ), KRG_NVP( m_resourceTypeID ), KRG_NVP( m_shaderDataPath ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::ShaderResourceDescriptor> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Render::ShaderResourceDescriptor const* pActualDefaultTypeInstance = ( KRG::Render::ShaderResourceDescriptor const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_resourceTypeID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceTypeID" );
            propertyInfo.m_parentTypeID = 1656199231;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_resourceTypeID;
            propertyInfo.m_offset = offsetof( KRG::Render::ShaderResourceDescriptor, m_resourceTypeID );
            propertyInfo.m_size = sizeof( KRG::ResourceTypeID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_shaderDataPath" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::DataPath" );
            propertyInfo.m_parentTypeID = 1656199231;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_shaderDataPath;
            propertyInfo.m_offset = offsetof( KRG::Render::ShaderResourceDescriptor, m_shaderDataPath );
            propertyInfo.m_size = sizeof( KRG::DataPath );
            propertyInfo.m_flags.Set( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_RESOURCECOMPILERS_RENDER_API TTypeHelper<KRG::Render::ShaderResourceDescriptor> : public ITypeHelper
            {
                static TTypeHelper<KRG::Render::ShaderResourceDescriptor> StaticTypeHelper;

                static void const* DefaultTypeInstancePtr;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::Render::ShaderResourceDescriptor ), alignof( KRG::Render::ShaderResourceDescriptor ) );
                    new ( pDefaultTypeInstance ) KRG::Render::ShaderResourceDescriptor;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Render::ShaderResourceDescriptor" );
                    typeInfo.m_size = sizeof( KRG::Render::ShaderResourceDescriptor );
                    typeInfo.m_alignment = alignof( KRG::Render::ShaderResourceDescriptor );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Resource::ResourceDescriptor::StaticTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::ShaderResourceDescriptor> >( DefaultTypeInstancePtr );
                    KRG::Render::ShaderResourceDescriptor::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Render::ShaderResourceDescriptor" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Render::ShaderResourceDescriptor*>( pDefaultTypeInstance )->~ShaderResourceDescriptor();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Render::ShaderResourceDescriptor>();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Render::ShaderResourceDescriptor*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Render::ShaderResourceDescriptor*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::ShaderResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::ShaderResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::ShaderResourceDescriptor*>( pType );
                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::ShaderResourceDescriptor*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

            };
        }
    }
}
