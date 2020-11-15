#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 3968648659

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Engine\Animation\Components\SimpleAnimationComponent.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::SimpleAnimationComponent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_ENGINE_ANIMATION_API void serialize( Archive& archive, KRG::Animation::SimpleAnimationComponent& type )
    {
        archive( cereal::base_class<KRG::Animation::AnimationComponent>( &type ), KRG_NVP( m_pAnimation ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::SimpleAnimationComponent> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::SimpleAnimationComponent const* pActualDefaultTypeInstance = ( KRG::Animation::SimpleAnimationComponent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_pAnimation" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 3150771824;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Animation::AnimationData" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_pAnimation;
            propertyInfo.m_offset = offsetof( KRG::Animation::SimpleAnimationComponent, m_pAnimation );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Animation::AnimationData> );
            propertyInfo.m_flags.SetAll( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_ANIMATION_API TTypeHelper<KRG::Animation::SimpleAnimationComponent> : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::SimpleAnimationComponent> StaticTypeHelper;

                static void const* DefaultTypeInstancePtr;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::Animation::SimpleAnimationComponent ), alignof( KRG::Animation::SimpleAnimationComponent ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::SimpleAnimationComponent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::SimpleAnimationComponent" );
                    typeInfo.m_size = sizeof( KRG::Animation::SimpleAnimationComponent );
                    typeInfo.m_alignment = alignof( KRG::Animation::SimpleAnimationComponent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntityComponent );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Animation::AnimationComponent::StaticTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::SimpleAnimationComponent> >( DefaultTypeInstancePtr );
                    KRG::Animation::SimpleAnimationComponent::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::SimpleAnimationComponent" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::SimpleAnimationComponent*>( pDefaultTypeInstance )->~SimpleAnimationComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::SimpleAnimationComponent>();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::SimpleAnimationComponent*>( pType );

                    if ( pActualType->m_pAnimation.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pAnimation, requesterID );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::SimpleAnimationComponent*>( pType );

                    if ( pActualType->m_pAnimation.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pAnimation, requesterID );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::SimpleAnimationComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    if ( !pActualType->m_pAnimation.IsValid() || pActualType->m_pAnimation.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( pActualType->m_pAnimation.IsUnloaded() || pActualType->m_pAnimation.IsLoading() )
                    {
                        return LoadingStatus::Loading;
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::SimpleAnimationComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    KRG_ASSERT( !pActualType->m_pAnimation.IsLoading() );
                    if ( !pActualType->m_pAnimation.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, U32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::SimpleAnimationComponent*>( pType );
                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, U32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::SimpleAnimationComponent*>( pType );
                    if ( propertyID == 1571632376 )
                    {
                        return KRG::Animation::AnimationData::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

            };
        }
    }
}
