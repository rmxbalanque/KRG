#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 1236551654

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Tools\Animation\Graph\Nodes\AnimationToolsNode_Pose.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::Graph::ZeroPoseToolsNode
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::Graph::ZeroPoseToolsNode& type )
    {
        archive( cereal::base_class<KRG::Animation::Graph::FlowToolsNode>( &type ), KRG_NVP( m_canvasPosition ), KRG_NVP( m_ID ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ZeroPoseToolsNode> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::Graph::ZeroPoseToolsNode const* pActualDefaultTypeInstance = ( KRG::Animation::Graph::ZeroPoseToolsNode const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_canvasPosition" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float2" );
            propertyInfo.m_parentTypeID = 3357685109;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_canvasPosition;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::ZeroPoseToolsNode, m_canvasPosition );
            propertyInfo.m_size = sizeof( KRG::Float2 );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 3357685109;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_ID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::ZeroPoseToolsNode, m_ID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::Graph::ZeroPoseToolsNode> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::Graph::ZeroPoseToolsNode> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::Graph::ZeroPoseToolsNode ), alignof( KRG::Animation::Graph::ZeroPoseToolsNode ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::Graph::ZeroPoseToolsNode;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::Graph::ZeroPoseToolsNode" );
                    typeInfo.m_size = sizeof( KRG::Animation::Graph::ZeroPoseToolsNode );
                    typeInfo.m_alignment = alignof( KRG::Animation::Graph::ZeroPoseToolsNode );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Animation::Graph::FlowToolsNode::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ZeroPoseToolsNode> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::Graph::ZeroPoseToolsNode::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::Graph::ZeroPoseToolsNode" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode*>( pDefaultTypeInstance )->~ZeroPoseToolsNode();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::Graph::ZeroPoseToolsNode>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::Graph::ZeroPoseToolsNode();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::Graph::ZeroPoseToolsNode::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2454640797 ) )
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
                    auto pType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode const*>( pOtherTypeInstance );

                    if ( propertyID == 2454640797 )
                    {
                        return pType->m_canvasPosition == pOtherType->m_canvasPosition;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        return pType->m_ID == pOtherType->m_ID;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ZeroPoseToolsNode*>( pTypeInstance );

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

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::Graph::ReferencePoseToolsNode
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::Graph::ReferencePoseToolsNode& type )
    {
        archive( cereal::base_class<KRG::Animation::Graph::FlowToolsNode>( &type ), KRG_NVP( m_canvasPosition ), KRG_NVP( m_ID ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ReferencePoseToolsNode> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::Graph::ReferencePoseToolsNode const* pActualDefaultTypeInstance = ( KRG::Animation::Graph::ReferencePoseToolsNode const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_canvasPosition" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float2" );
            propertyInfo.m_parentTypeID = 357720739;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_canvasPosition;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::ReferencePoseToolsNode, m_canvasPosition );
            propertyInfo.m_size = sizeof( KRG::Float2 );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 357720739;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_ID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::ReferencePoseToolsNode, m_ID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::Graph::ReferencePoseToolsNode> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::Graph::ReferencePoseToolsNode> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::Graph::ReferencePoseToolsNode ), alignof( KRG::Animation::Graph::ReferencePoseToolsNode ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::Graph::ReferencePoseToolsNode;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::Graph::ReferencePoseToolsNode" );
                    typeInfo.m_size = sizeof( KRG::Animation::Graph::ReferencePoseToolsNode );
                    typeInfo.m_alignment = alignof( KRG::Animation::Graph::ReferencePoseToolsNode );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Animation::Graph::FlowToolsNode::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::ReferencePoseToolsNode> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::Graph::ReferencePoseToolsNode::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::Graph::ReferencePoseToolsNode" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode*>( pDefaultTypeInstance )->~ReferencePoseToolsNode();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::Graph::ReferencePoseToolsNode>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::Graph::ReferencePoseToolsNode();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::Graph::ReferencePoseToolsNode::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2454640797 ) )
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
                    auto pType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode const*>( pOtherTypeInstance );

                    if ( propertyID == 2454640797 )
                    {
                        return pType->m_canvasPosition == pOtherType->m_canvasPosition;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        return pType->m_ID == pOtherType->m_ID;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::ReferencePoseToolsNode*>( pTypeInstance );

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

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::Graph::AnimationPoseToolsNode
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::Graph::AnimationPoseToolsNode& type )
    {
        archive( cereal::base_class<KRG::Animation::Graph::DataSlotToolsNode>( &type ), KRG_NVP( m_defaultResourceID ), KRG_NVP( m_overrides ), KRG_NVP( m_canvasPosition ), KRG_NVP( m_ID ), KRG_NVP( m_name ), KRG_NVP( m_inputTimeRemapRange ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::AnimationPoseToolsNode> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::Graph::AnimationPoseToolsNode const* pActualDefaultTypeInstance = ( KRG::Animation::Graph::AnimationPoseToolsNode const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_defaultResourceID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceID" );
            propertyInfo.m_parentTypeID = 467749769;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_defaultResourceID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::AnimationPoseToolsNode, m_defaultResourceID );
            propertyInfo.m_size = sizeof( KRG::ResourceID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_overrides" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Animation::Graph::DataSlotToolsNode::OverrideValue" );
            propertyInfo.m_parentTypeID = 467749769;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_overrides;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::AnimationPoseToolsNode, m_overrides );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_overrides.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_overrides.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::Animation::Graph::DataSlotToolsNode::OverrideValue );
            propertyInfo.m_size = sizeof( TVector<KRG::Animation::Graph::DataSlotToolsNode::OverrideValue> );
            propertyInfo.m_flags.Set( 36 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_canvasPosition" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float2" );
            propertyInfo.m_parentTypeID = 467749769;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_canvasPosition;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::AnimationPoseToolsNode, m_canvasPosition );
            propertyInfo.m_size = sizeof( KRG::Float2 );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 467749769;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_ID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::AnimationPoseToolsNode, m_ID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_name" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 467749769;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_name;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::AnimationPoseToolsNode, m_name );
            propertyInfo.m_size = sizeof( KRG::String );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_inputTimeRemapRange" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::FloatRange" );
            propertyInfo.m_parentTypeID = 467749769;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_inputTimeRemapRange;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::AnimationPoseToolsNode, m_inputTimeRemapRange );
            propertyInfo.m_size = sizeof( KRG::FloatRange );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::Graph::AnimationPoseToolsNode> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::Graph::AnimationPoseToolsNode> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::Graph::AnimationPoseToolsNode ), alignof( KRG::Animation::Graph::AnimationPoseToolsNode ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::Graph::AnimationPoseToolsNode;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::Graph::AnimationPoseToolsNode" );
                    typeInfo.m_size = sizeof( KRG::Animation::Graph::AnimationPoseToolsNode );
                    typeInfo.m_alignment = alignof( KRG::Animation::Graph::AnimationPoseToolsNode );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Animation::Graph::DataSlotToolsNode::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::AnimationPoseToolsNode> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::Graph::AnimationPoseToolsNode::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::Graph::AnimationPoseToolsNode" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode*>( pDefaultTypeInstance )->~AnimationPoseToolsNode();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::Graph::AnimationPoseToolsNode>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::Graph::AnimationPoseToolsNode();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode*>( pType );

                    for ( auto& propertyValue : pActualType->m_overrides )
                    {
                        KRG::Animation::Graph::DataSlotToolsNode::OverrideValue::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode*>( pType );

                    for ( auto& propertyValue : pActualType->m_overrides )
                    {
                        KRG::Animation::Graph::DataSlotToolsNode::OverrideValue::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    for ( auto& propertyValue : pActualType->m_overrides )
                    {
                        status = KRG::Animation::Graph::DataSlotToolsNode::OverrideValue::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &propertyValue );
                        if ( status == LoadingStatus::Loading )
                        {
                            return LoadingStatus::Loading;
                        }
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    for ( auto& propertyValue : pActualType->m_overrides )
                    {
                        status = KRG::Animation::Graph::DataSlotToolsNode::OverrideValue::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &propertyValue );
                        if ( status != LoadingStatus::Unloaded )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode*>( pType );

                    if ( arrayID == 2183863778 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_overrides.size() )
                        {
                            pActualType->m_overrides.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_overrides[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode const*>( pTypeInstance );

                    if ( arrayID == 2183863778 )
                    {
                        return pActualType->m_overrides.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 2183863778 )
                    {
                        return sizeof( KRG::Animation::Graph::DataSlotToolsNode::OverrideValue );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode*>( pTypeInstance );

                    if ( arrayID == 2183863778 )
                    {
                        pActualType->m_overrides.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode*>( pTypeInstance );

                    if ( arrayID == 2183863778 )
                    {
                        pActualType->m_overrides.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode*>( pTypeInstance );

                    if ( arrayID == 2183863778 )
                    {
                        pActualType->m_overrides.erase( pActualType->m_overrides.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::Graph::AnimationPoseToolsNode::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1592680383 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2183863778 ) )
                    {
                       return false;
                    }

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

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 14795458 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode const*>( pOtherTypeInstance );

                    if ( propertyID == 1592680383 )
                    {
                        return pType->m_defaultResourceID == pOtherType->m_defaultResourceID;
                    }

                    if ( propertyID == 2183863778 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_overrides.size() )
                            {
                                return false;
                            }

                            return KRG::Animation::Graph::DataSlotToolsNode::OverrideValue::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_overrides[arrayIdx], &pOtherType->m_overrides[arrayIdx] );
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_overrides.size() != pOtherType->m_overrides.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_overrides.size(); i++ )
                            {
                                if( !KRG::Animation::Graph::DataSlotToolsNode::OverrideValue::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_overrides[i], &pOtherType->m_overrides[i] ) )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

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

                    if ( propertyID == 14795458 )
                    {
                        return pType->m_inputTimeRemapRange == pOtherType->m_inputTimeRemapRange;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::AnimationPoseToolsNode*>( pTypeInstance );

                    if ( propertyID == 1592680383 )
                    {
                        pActualType->m_defaultResourceID = pDefaultType->m_defaultResourceID;
                        return;
                    }

                    if ( propertyID == 2183863778 )
                    {
                        pActualType->m_overrides = pDefaultType->m_overrides;
                        return;
                    }

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

                    if ( propertyID == 14795458 )
                    {
                        pActualType->m_inputTimeRemapRange = pDefaultType->m_inputTimeRemapRange;
                        return;
                    }

                }

            };
        }
    }
}
