#include "StaticMeshComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        void StaticMeshComponent::Initialize()
        {
            SetLocalBounds( m_pMesh->GetBounds() );
        }

        void StaticMeshComponent::Shutdown()
        {}

        void StaticMeshComponent::OnWorldTransformUpdated()
        {
            if ( m_mobility == Mobility::Static )
            {
                KRG_LOG_WARNING( "Render", "Static Mesh Component (%s) (%s) Moved. Mobility changed to Dynamic!", GetName().c_str(), GetID().ToString().c_str() );
                ChangeMobility( Mobility::Dynamic );
            }
        }
    }
}
