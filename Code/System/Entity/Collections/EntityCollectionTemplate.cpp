#include "EntityCollectionTemplate.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    void EntityCollectionTemplate::Reserve( S32 numEntities )
    {
        m_entityDescriptors.reserve( numEntities );
        m_entityLookupMap.reserve( numEntities );
    }

    void EntityCollectionTemplate::GenerateSpatialAttachmentInfo()
    {
        m_entitySpatialAttachmentInfo.clear();
        m_entitySpatialAttachmentInfo.reserve( m_entityDescriptors.size() );

        S32 const numEntities = (S32) m_entityDescriptors.size();
        for ( S32 i = 0; i < numEntities; i++ )
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
}