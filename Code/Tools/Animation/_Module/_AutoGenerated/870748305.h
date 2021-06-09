#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 870748305

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Tools\Animation\Graph\Nodes\AnimationToolsNode_Parameters.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::Graph::ControlParameterToolsNode
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::Graph::ControlParameterToolsNode& type )
    {
        archive( cereal::base_class<KRG::Animation::Graph::ToolsNode>( &type ), KRG_NVP( m_canvasPosition ), KRG_NVP( m_ID ), KRG_NVP( m_name ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ControlParameterToolsNode> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::Graph::ControlParameterToolsNode const* pActualDefaultTypeInstance = ( KRG::Animation::Graph::ControlParameterToolsNode const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_canvasPosition" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float2" );
            propertyInfo.m_parentTypeID = 1966630063;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_canvasPosition;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::ControlParameterToolsNode, m_canvasPosition );
            propertyInfo.m_size = sizeof( KRG::Float2 );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 1966630063;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_ID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::ControlParameterToolsNode, m_ID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_name" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 1966630063;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_name;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::ControlParameterToolsNode, m_name );
            propertyInfo.m_size = sizeof( KRG::String );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::Graph::ControlParameterToolsNode> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::Graph::ControlParameterToolsNode> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::Graph::ControlParameterToolsNode ), alignof( KRG::Animation::Graph::ControlParameterToolsNode ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::Graph::ControlParameterToolsNode;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::Graph::ControlParameterToolsNode" );
                    typeInfo.m_size = sizeof( KRG::Animation::Graph::ControlParameterToolsNode );
                    typeInfo.m_alignment = alignof( KRG::Animation::Graph::ControlParameterToolsNode );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Animation::Graph::ToolsNode::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ControlParameterToolsNode> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::Graph::ControlParameterToolsNode::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::Graph::ControlParameterToolsNode" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode*>( pDefaultTypeInstance )->~ControlParameterToolsNode();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::Graph::ControlParameterToolsNode>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::Graph::ControlParameterToolsNode();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::Graph::ControlParameterToolsNode::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2454640797 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4230898639 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3460589943 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode const*>( pOtherTypeInstance );

                    if ( propertyID == 2454640797 )
                    {
                        return pType->m_canvasPosition == pOtherType->m_canvasPosition;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        return pType->m_ID == pOtherType->m_ID;
                    }

                    if ( propertyID == 3460589943 )
                    {
                        return pType->m_name == pOtherType->m_name;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterToolsNode*>( pTypeInstance );

                    if ( propertyID == 2454640797 )
                    {
                        pActualType->m_canvasPosition = pDefaultType->m_canvasPosition;
                        return;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        pActualType->m_ID = pDefaultType->m_ID;
                        return;
                    }

                    if ( propertyID == 3460589943 )
                    {
                        pActualType->m_name = pDefaultType->m_name;
                        return;
                    }

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::Graph::ControlParameterReferenceToolsNode
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::Graph::ControlParameterReferenceToolsNode& type )
    {
        archive( cereal::base_class<KRG::Animation::Graph::ToolsNode>( &type ), KRG_NVP( m_canvasPosition ), KRG_NVP( m_ID ), KRG_NVP( m_parameterUUID ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ControlParameterReferenceToolsNode> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::Graph::ControlParameterReferenceToolsNode const* pActualDefaultTypeInstance = ( KRG::Animation::Graph::ControlParameterReferenceToolsNode const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_canvasPosition" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float2" );
            propertyInfo.m_parentTypeID = 2569738001;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_canvasPosition;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::ControlParameterReferenceToolsNode, m_canvasPosition );
            propertyInfo.m_size = sizeof( KRG::Float2 );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 2569738001;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_ID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::ControlParameterReferenceToolsNode, m_ID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_parameterUUID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 2569738001;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_parameterUUID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::ControlParameterReferenceToolsNode, m_parameterUUID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::Graph::ControlParameterReferenceToolsNode> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::Graph::ControlParameterReferenceToolsNode> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::Graph::ControlParameterReferenceToolsNode ), alignof( KRG::Animation::Graph::ControlParameterReferenceToolsNode ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::Graph::ControlParameterReferenceToolsNode;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::Graph::ControlParameterReferenceToolsNode" );
                    typeInfo.m_size = sizeof( KRG::Animation::Graph::ControlParameterReferenceToolsNode );
                    typeInfo.m_alignment = alignof( KRG::Animation::Graph::ControlParameterReferenceToolsNode );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Animation::Graph::ToolsNode::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ControlParameterReferenceToolsNode> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::Graph::ControlParameterReferenceToolsNode::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::Graph::ControlParameterReferenceToolsNode" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode*>( pDefaultTypeInstance )->~ControlParameterReferenceToolsNode();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::Graph::ControlParameterReferenceToolsNode>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::Graph::ControlParameterReferenceToolsNode();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::Graph::ControlParameterReferenceToolsNode::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2454640797 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4230898639 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1603881148 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode const*>( pOtherTypeInstance );

                    if ( propertyID == 2454640797 )
                    {
                        return pType->m_canvasPosition == pOtherType->m_canvasPosition;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        return pType->m_ID == pOtherType->m_ID;
                    }

                    if ( propertyID == 1603881148 )
                    {
                        return pType->m_parameterUUID == pOtherType->m_parameterUUID;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ControlParameterReferenceToolsNode*>( pTypeInstance );

                    if ( propertyID == 2454640797 )
                    {
                        pActualType->m_canvasPosition = pDefaultType->m_canvasPosition;
                        return;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        pActualType->m_ID = pDefaultType->m_ID;
                        return;
                    }

                    if ( propertyID == 1603881148 )
                    {
                        pActualType->m_parameterUUID = pDefaultType->m_parameterUUID;
                        return;
                    }

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::Graph::VirtualParameterToolsNode
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::Graph::VirtualParameterToolsNode& type )
    {
        archive( cereal::base_class<KRG::Animation::Graph::ToolsNode>( &type ), KRG_NVP( m_canvasPosition ), KRG_NVP( m_ID ), KRG_NVP( m_name ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::VirtualParameterToolsNode> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::Graph::VirtualParameterToolsNode const* pActualDefaultTypeInstance = ( KRG::Animation::Graph::VirtualParameterToolsNode const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_canvasPosition" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float2" );
            propertyInfo.m_parentTypeID = 2876909626;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_canvasPosition;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::VirtualParameterToolsNode, m_canvasPosition );
            propertyInfo.m_size = sizeof( KRG::Float2 );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 2876909626;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_ID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::VirtualParameterToolsNode, m_ID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_name" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 2876909626;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_name;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::VirtualParameterToolsNode, m_name );
            propertyInfo.m_size = sizeof( KRG::String );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::Graph::VirtualParameterToolsNode> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::Graph::VirtualParameterToolsNode> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::Graph::VirtualParameterToolsNode ), alignof( KRG::Animation::Graph::VirtualParameterToolsNode ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::Graph::VirtualParameterToolsNode;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::Graph::VirtualParameterToolsNode" );
                    typeInfo.m_size = sizeof( KRG::Animation::Graph::VirtualParameterToolsNode );
                    typeInfo.m_alignment = alignof( KRG::Animation::Graph::VirtualParameterToolsNode );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Animation::Graph::ToolsNode::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::VirtualParameterToolsNode> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::Graph::VirtualParameterToolsNode::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::Graph::VirtualParameterToolsNode" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode*>( pDefaultTypeInstance )->~VirtualParameterToolsNode();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::Graph::VirtualParameterToolsNode>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::Graph::VirtualParameterToolsNode();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::Graph::VirtualParameterToolsNode::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2454640797 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4230898639 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3460589943 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode const*>( pOtherTypeInstance );

                    if ( propertyID == 2454640797 )
                    {
                        return pType->m_canvasPosition == pOtherType->m_canvasPosition;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        return pType->m_ID == pOtherType->m_ID;
                    }

                    if ( propertyID == 3460589943 )
                    {
                        return pType->m_name == pOtherType->m_name;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterToolsNode*>( pTypeInstance );

                    if ( propertyID == 2454640797 )
                    {
                        pActualType->m_canvasPosition = pDefaultType->m_canvasPosition;
                        return;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        pActualType->m_ID = pDefaultType->m_ID;
                        return;
                    }

                    if ( propertyID == 3460589943 )
                    {
                        pActualType->m_name = pDefaultType->m_name;
                        return;
                    }

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::Graph::VirtualParameterReferenceToolsNode
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::Graph::VirtualParameterReferenceToolsNode& type )
    {
        archive( cereal::base_class<KRG::Animation::Graph::ToolsNode>( &type ), KRG_NVP( m_canvasPosition ), KRG_NVP( m_ID ), KRG_NVP( m_parameterUUID ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::VirtualParameterReferenceToolsNode> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::Graph::VirtualParameterReferenceToolsNode const* pActualDefaultTypeInstance = ( KRG::Animation::Graph::VirtualParameterReferenceToolsNode const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_canvasPosition" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float2" );
            propertyInfo.m_parentTypeID = 2336335862;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_canvasPosition;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::VirtualParameterReferenceToolsNode, m_canvasPosition );
            propertyInfo.m_size = sizeof( KRG::Float2 );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 2336335862;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_ID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::VirtualParameterReferenceToolsNode, m_ID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_parameterUUID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 2336335862;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_parameterUUID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::VirtualParameterReferenceToolsNode, m_parameterUUID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::Graph::VirtualParameterReferenceToolsNode> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::Graph::VirtualParameterReferenceToolsNode> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::Graph::VirtualParameterReferenceToolsNode ), alignof( KRG::Animation::Graph::VirtualParameterReferenceToolsNode ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::Graph::VirtualParameterReferenceToolsNode;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::Graph::VirtualParameterReferenceToolsNode" );
                    typeInfo.m_size = sizeof( KRG::Animation::Graph::VirtualParameterReferenceToolsNode );
                    typeInfo.m_alignment = alignof( KRG::Animation::Graph::VirtualParameterReferenceToolsNode );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Animation::Graph::ToolsNode::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::VirtualParameterReferenceToolsNode> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::Graph::VirtualParameterReferenceToolsNode::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::Graph::VirtualParameterReferenceToolsNode" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode*>( pDefaultTypeInstance )->~VirtualParameterReferenceToolsNode();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::Graph::VirtualParameterReferenceToolsNode>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::Graph::VirtualParameterReferenceToolsNode();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::Graph::VirtualParameterReferenceToolsNode::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2454640797 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4230898639 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1603881148 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode const*>( pOtherTypeInstance );

                    if ( propertyID == 2454640797 )
                    {
                        return pType->m_canvasPosition == pOtherType->m_canvasPosition;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        return pType->m_ID == pOtherType->m_ID;
                    }

                    if ( propertyID == 1603881148 )
                    {
                        return pType->m_parameterUUID == pOtherType->m_parameterUUID;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::VirtualParameterReferenceToolsNode*>( pTypeInstance );

                    if ( propertyID == 2454640797 )
                    {
                        pActualType->m_canvasPosition = pDefaultType->m_canvasPosition;
                        return;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        pActualType->m_ID = pDefaultType->m_ID;
                        return;
                    }

                    if ( propertyID == 1603881148 )
                    {
                        pActualType->m_parameterUUID = pDefaultType->m_parameterUUID;
                        return;
                    }

                }

            };
        }
    }
}
