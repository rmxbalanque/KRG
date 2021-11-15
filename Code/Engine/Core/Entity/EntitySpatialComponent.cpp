#include "EntitySpatialComponent.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Transform SpatialEntityComponent::GetAttachmentSocketTransform( StringID socketID ) const
    {
        Transform socketTransform;

        // If the socket ID is invalid, just return the current transform
        if ( !socketID.IsValid() )
        {
            socketTransform = m_worldTransform;
            return socketTransform;
        }

        //-------------------------------------------------------------------------

        // Try to find the attachment socket transform and if it succeeds return it
        if ( TryFindAttachmentSocketTransform( socketID, socketTransform ) )
        {
            return socketTransform;
        }

        //-------------------------------------------------------------------------

        // Search all children
        for ( auto pChildSpatialComponent : m_spatialChildren )
        {
            auto foundTransform = pChildSpatialComponent->TryGetAttachmentSocketTransform( socketID, socketTransform );
            if ( foundTransform )
            {
                return socketTransform;
            }
        }

        // Log warning only when we are loaded/initialized
        if ( m_status == EntityComponent::Status::Loaded || m_status == EntityComponent::Status::Initialized )
        {
            KRG_LOG_WARNING( "Entity", "Failed to find socket %s on component %s (%s)", socketID.c_str(), GetName().c_str(), GetID().ToString().c_str() );
        }

        // Fallback to the world transform
        socketTransform = m_worldTransform;
        return socketTransform;
    }

    bool SpatialEntityComponent::TryGetAttachmentSocketTransform( StringID socketID, Transform& outSocketWorldTransform ) const
    {
        // Try to find the attachment socket transform and if it succeeds return it
        if ( TryFindAttachmentSocketTransform( socketID, outSocketWorldTransform ) )
        {
            return true;
        }

        // Search all children
        for ( auto pChildSpatialComponent : m_spatialChildren )
        {
            auto foundTransform = pChildSpatialComponent->TryGetAttachmentSocketTransform( socketID, outSocketWorldTransform );
            if ( foundTransform )
            {
                return true;
            }
        }

        return false;
    }

    bool SpatialEntityComponent::TryFindAttachmentSocketTransform( StringID socketID, Transform& outSocketWorldTransform ) const
    {
        outSocketWorldTransform = m_worldTransform;
        return false;
    }

    void SpatialEntityComponent::NotifySocketsUpdated()
    {
        for ( auto& pChildComponent : m_spatialChildren )
        {
            pChildComponent->CalculateWorldTransform();
        }
    }
}