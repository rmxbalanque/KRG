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
            attachmentInfo.m_parentEntityIdx = FindEntityIndex( entityDesc.m_spatialParentID );

            if ( attachmentInfo.m_parentEntityIdx != InvalidIndex )
            {
                m_entitySpatialAttachmentInfo.push_back( attachmentInfo );
            }
        }
    }

    TVector<EntityComponentDescriptor*> EntityCollectionDescriptor::GetComponentsOfType( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeID typeID, bool allowDerivedTypes )
    {
        TVector<EntityComponentDescriptor*> foundComponents;

        for ( auto& entityDesc : m_entityDescriptors )
        {
            for ( auto& componentDesc : entityDesc.m_components )
            {
                if ( componentDesc.m_typeID == typeID )
                {
                    foundComponents.emplace_back( &componentDesc );
                }
                else if ( allowDerivedTypes )
                {
                    auto pTypeInfo = typeRegistry.GetTypeInfo( componentDesc.m_typeID );
                    KRG_ASSERT( pTypeInfo != nullptr );

                    if ( pTypeInfo->IsDerivedFrom( typeID ) )
                    {
                        foundComponents.emplace_back( &componentDesc );
                    }
                }
            }
        }

        //-------------------------------------------------------------------------

        return foundComponents;
    }
}