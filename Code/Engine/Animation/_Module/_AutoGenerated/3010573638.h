#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 3010573638

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Engine\Animation\Components\AnimationGraphComponent.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::AnimationGraphComponent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_ENGINE_ANIMATION_API void serialize( Archive& archive, KRG::Animation::AnimationGraphComponent& type )
    {
        archive( cereal::base_class<KRG::Animation::AnimationComponent>( &type ), KRG_NVP( m_pGraph ), KRG_NVP( m_pDataSet ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::AnimationGraphComponent> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::AnimationGraphComponent const* pActualDefaultTypeInstance = ( KRG::Animation::AnimationGraphComponent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_pGraph" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 1471731978;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Animation::AnimationGraph" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_pGraph;
            propertyInfo.m_offset = offsetof( KRG::Animation::AnimationGraphComponent, m_pGraph );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Animation::AnimationGraph> );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_pDataSet" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 1471731978;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Animation::AnimationGraphDataSet" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_pDataSet;
            propertyInfo.m_offset = offsetof( KRG::Animation::AnimationGraphComponent, m_pDataSet );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Animation::AnimationGraphDataSet> );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_ANIMATION_API TTypeHelper<KRG::Animation::AnimationGraphComponent> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::AnimationGraphComponent> StaticTypeHelper;

                static void const* s_pDefaultTypeInstancePtr;

            public:

                virtual void const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::Animation::AnimationGraphComponent ), alignof( KRG::Animation::AnimationGraphComponent ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::AnimationGraphComponent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::AnimationGraphComponent" );
                    typeInfo.m_size = sizeof( KRG::Animation::AnimationGraphComponent );
                    typeInfo.m_alignment = alignof( KRG::Animation::AnimationGraphComponent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntityComponent );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Animation::AnimationComponent::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::AnimationGraphComponent> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::AnimationGraphComponent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::AnimationGraphComponent" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::AnimationGraphComponent*>( pDefaultTypeInstance )->~AnimationGraphComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::AnimationGraphComponent>();
                }

                virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::AnimationGraphComponent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphComponent*>( pType );

                    if ( pActualType->m_pGraph.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pGraph, requesterID );
                    }

                    if ( pActualType->m_pDataSet.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pDataSet, requesterID );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphComponent*>( pType );

                    if ( pActualType->m_pGraph.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pGraph, requesterID );
                    }

                    if ( pActualType->m_pDataSet.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pDataSet, requesterID );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    if ( !pActualType->m_pGraph.IsValid() || pActualType->m_pGraph.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( pActualType->m_pGraph.IsUnloaded() || pActualType->m_pGraph.IsLoading() )
                    {
                        return LoadingStatus::Loading;
                    }

                    if ( !pActualType->m_pDataSet.IsValid() || pActualType->m_pDataSet.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( pActualType->m_pDataSet.IsUnloaded() || pActualType->m_pDataSet.IsLoading() )
                    {
                        return LoadingStatus::Loading;
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    KRG_ASSERT( !pActualType->m_pGraph.IsLoading() );
                    if ( !pActualType->m_pGraph.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    KRG_ASSERT( !pActualType->m_pDataSet.IsLoading() );
                    if ( !pActualType->m_pDataSet.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphComponent*>( pType );
                    if ( propertyID == 620966544 )
                    {
                        return KRG::Animation::AnimationGraph::GetStaticResourceTypeID();
                    }

                    if ( propertyID == 4118139169 )
                    {
                        return KRG::Animation::AnimationGraphDataSet::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphComponent*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( void const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphComponent const*>( pTypeInstance );

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

                virtual void ClearArray( void* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphComponent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( void* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphComponent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( void* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphComponent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( void const* pTypeInstance, void const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::AnimationGraphComponent::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::AnimationGraphComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::AnimationGraphComponent const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 620966544 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4118139169 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( void const* pTypeInstance, void const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::AnimationGraphComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::AnimationGraphComponent const*>( pOtherTypeInstance );

                    if ( propertyID == 620966544 )
                    {
                        return pType->m_pGraph == pOtherType->m_pGraph;
                    }

                    if ( propertyID == 4118139169 )
                    {
                        return pType->m_pDataSet == pOtherType->m_pDataSet;
                    }

                    return false;
                }

                virtual void ResetToDefault( void* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::AnimationGraphComponent const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphComponent*>( pTypeInstance );

                    if ( propertyID == 620966544 )
                    {
                        pActualType->m_pGraph = pDefaultType->m_pGraph;
                        return;
                    }

                    if ( propertyID == 4118139169 )
                    {
                        pActualType->m_pDataSet = pDefaultType->m_pDataSet;
                        return;
                    }

                }

            };
        }
    }
}
