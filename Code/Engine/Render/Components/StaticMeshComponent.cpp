#include "StaticMeshComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    void StaticMeshComponent::Initialize()
    {
        MeshComponent::Initialize();
        SetLocalBounds( m_pMesh->GetBounds() );
    }

    void StaticMeshComponent::OnWorldTransformUpdated()
    {
        if ( m_mobility == Mobility::Static )
        {
            KRG_LOG_WARNING( "Render", "Static Mesh Component (%s) (%s) Moved. Mobility changed to Dynamic!", GetName().c_str(), GetID().ToString().c_str() );
            ChangeMobility( Mobility::Dynamic );
        }
    }

    TVector<TResourcePtr<Render::Material>> const& StaticMeshComponent::GetDefaultMaterials() const
    {
        KRG_ASSERT( IsInitialized() );
        return m_pMesh->GetMaterials();
    }
}
