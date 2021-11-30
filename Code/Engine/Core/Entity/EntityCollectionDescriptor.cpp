#include "EntityCollectionDescriptor.h"
#include "../../TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    void EntityCollectionDescriptor::Reserve( int32 numEntities )
    {
        m_entityDescriptors.reserve( numEntities );
        m_entityLookupMap.reserve( numEntities );
    }

    void EntityCollectionDescriptor::GenerateSpatialAttachmentInfo()
    {
        m_entitySpatialAttachmentInfo.clear();
        m_entitySpatialAttachmentInfo.reserve( m_entityDescriptors.size() );

        int32 const numEntities = (int32) m_entityDescriptors.size();
        for ( int32 i = 0; i < numEntities; i++ )
        {
            auto const& entityDesc = m_entityDescriptors[i];
            if ( !entityDesc.IsSpatialEntity() || !entityDesc.HasSpatialParent() )
            {
                continue;
            }

            //-------------------------------------------------------------------------

            SpatialAttachmentInfo attachmentInfo;
            attachmentInfo.m_entityIdx = i;
            attachmentInfo.m_parentEntityIdx = FindEntityIndex( entityDesc.m_spatialParentName );

            if ( attachmentInfo.m_parentEntityIdx != InvalidIndex )
            {
                m_entitySpatialAttachmentInfo.push_back( attachmentInfo );
            }
        }
    }

    TVector<EntityCollectionDescriptor::SearchResult> EntityCollectionDescriptor::GetComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeID typeID, bool allowDerivedTypes )
    {
        TVector<SearchResult> foundComponents;

        for ( auto& entityDesc : m_entityDescriptors )
        {
            for ( auto& componentDesc : entityDesc.m_components )
            {
                if ( componentDesc.m_typeID == typeID )
                {
                    auto& result = foundComponents.emplace_back( SearchResult() );
                    result.m_pEntity = &entityDesc;
                    result.m_pComponent = &componentDesc;
                }
                else if ( allowDerivedTypes )
                {
                    auto pTypeInfo = typeRegistry.GetTypeInfo( componentDesc.m_typeID );
                    KRG_ASSERT( pTypeInfo != nullptr );

                    if ( pTypeInfo->IsDerivedFrom( typeID ) )
                    {
                        auto& result = foundComponents.emplace_back( SearchResult() );
                        result.m_pEntity = &entityDesc;
                        result.m_pComponent = &componentDesc;
                    }
                }
            }
        }

        //-------------------------------------------------------------------------

        return foundComponents;
    }
}