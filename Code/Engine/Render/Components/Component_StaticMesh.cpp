#include "Component_StaticMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    void StaticMeshComponent::Initialize()
    {
        MeshComponent::Initialize();

        if ( HasMeshResourceSet() )
        {
            SetLocalBounds( m_pMesh->GetBounds() );
        }
    }

    void StaticMeshComponent::ChangeMobility( Mobility newMobility )
    {
        if ( newMobility != m_mobility )
        {
            m_mobility = newMobility;
            m_mobilityChangedEvent.Execute( this );
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
        KRG_ASSERT( IsInitialized() && HasMeshResourceSet() );
        return m_pMesh->GetMaterials();
    }
}