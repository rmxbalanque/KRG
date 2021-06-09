#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 3679790120

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Tools\Core\GraphEditor\GraphEditor.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::GraphEditor::BaseNode
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_CORE_API void serialize( Archive& archive, KRG::GraphEditor::BaseNode& type )
    {
        archive( cereal::base_class<KRG::IRegisteredType>( &type ), KRG_NVP( m_ID ), KRG_NVP( m_canvasPosition ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::GraphEditor::BaseNode> >( IRegisteredType const* pDefaultTypeInstance )
        {
            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 3537492806;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = nullptr;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_canvasPosition" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float2" );
            propertyInfo.m_parentTypeID = 3537492806;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = nullptr;
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_CORE_API TTypeHelper<KRG::GraphEditor::BaseNode> final : public ITypeHelper
            {
                static TTypeHelper<KRG::GraphEditor::BaseNode> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::GraphEditor::BaseNode" );
                    typeInfo.m_size = sizeof( KRG::GraphEditor::BaseNode );
                    typeInfo.m_alignment = alignof( KRG::GraphEditor::BaseNode );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::Abstract );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::IRegisteredType::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::GraphEditor::BaseNode> >( s_pDefaultTypeInstancePtr );
                    KRG::GraphEditor::BaseNode::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::GraphEditor::BaseNode" );
                    typeRegistry.UnregisterType( ID );

                }

                virtual IRegisteredType* CreateType() const override final
                {
                    KRG_HALT(); // Error! Trying to instantiate an abstract type!
                    return nullptr;
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_HALT(); // Error! Trying to instantiate an abstract type!
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseNode*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseNode*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseNode*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseNode*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseNode*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseNode*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseNode const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseNode*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::GraphEditor::BaseNode::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::GraphEditor::BaseNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::GraphEditor::BaseNode const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4230898639 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2454640797 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::GraphEditor::BaseNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::GraphEditor::BaseNode const*>( pOtherTypeInstance );

                    if ( propertyID == 4230898639 )
                    {
                        return pType->m_ID == pOtherType->m_ID;
                    }

                    if ( propertyID == 2454640797 )
                    {
                        return pType->m_canvasPosition == pOtherType->m_canvasPosition;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::GraphEditor::BaseNode const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseNode*>( pTypeInstance );

                    if ( propertyID == 4230898639 )
                    {
                        pActualType->m_ID = pDefaultType->m_ID;
                        return;
                    }

                    if ( propertyID == 2454640797 )
                    {
                        pActualType->m_canvasPosition = pDefaultType->m_canvasPosition;
                        return;
                    }

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::GraphEditor::BaseGraph
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_CORE_API void serialize( Archive& archive, KRG::GraphEditor::BaseGraph& type )
    {
        archive( cereal::base_class<KRG::IRegisteredType>( &type ), KRG_NVP( m_ID ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::GraphEditor::BaseGraph> >( IRegisteredType const* pDefaultTypeInstance )
        {
            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 1601682968;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = nullptr;
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_CORE_API TTypeHelper<KRG::GraphEditor::BaseGraph> final : public ITypeHelper
            {
                static TTypeHelper<KRG::GraphEditor::BaseGraph> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::GraphEditor::BaseGraph" );
                    typeInfo.m_size = sizeof( KRG::GraphEditor::BaseGraph );
                    typeInfo.m_alignment = alignof( KRG::GraphEditor::BaseGraph );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::Abstract );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::IRegisteredType::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::GraphEditor::BaseGraph> >( s_pDefaultTypeInstancePtr );
                    KRG::GraphEditor::BaseGraph::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::GraphEditor::BaseGraph" );
                    typeRegistry.UnregisterType( ID );

                }

                virtual IRegisteredType* CreateType() const override final
                {
                    KRG_HALT(); // Error! Trying to instantiate an abstract type!
                    return nullptr;
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_HALT(); // Error! Trying to instantiate an abstract type!
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseGraph*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseGraph*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseGraph*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseGraph*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseGraph*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseGraph*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseGraph const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseGraph*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseGraph*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseGraph*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::GraphEditor::BaseGraph::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::GraphEditor::BaseGraph const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::GraphEditor::BaseGraph const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4230898639 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::GraphEditor::BaseGraph const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::GraphEditor::BaseGraph const*>( pOtherTypeInstance );

                    if ( propertyID == 4230898639 )
                    {
                        return pType->m_ID == pOtherType->m_ID;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::GraphEditor::BaseGraph const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::GraphEditor::BaseGraph*>( pTypeInstance );

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
