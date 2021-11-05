#include "StaticMeshComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    void StaticMeshComponent::Initialize()
    {
        MeshComponent::Initialize();
        SetLocalBounds( m_pMesh->GetBounds() );
    }

    void StaticMeshComponent::ChangeMobility( Mobility newMobility )
    {
        if ( newMobility != m_mobility )
        {
            m_mobility = newMobility;
            if ( m_mobilityChangedEvent.HasBoundUser() )
            {
                m_mobilityChangedEvent.Execute( this );
            }
        }
    }

    void StaticMeshComponent::OnWorldTransformUpdated()
    {
        if ( m_mobility == Mobility::Static )
        {
            m_staticMobilityTransformUpdatedEvent.Execute( this );
        }
    }

    TVector<TResourcePtr<Render::Material>> const& StaticMeshComponent::GetDefaultMaterials() const
    {
        KRG_ASSERT( IsInitialized() );
        return m_pMesh->GetMaterials();
    }
}