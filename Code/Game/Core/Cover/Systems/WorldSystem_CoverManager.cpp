#include "WorldSystem_CoverManager.h"
#include "Game/Core/Cover/Components/Component_CoverVolume.h"
#include "Engine/Core/Entity/Entity.h"
#include "Engine/Core/Entity/EntityWorldUpdateContext.h"
#include "Engine/Core/Entity/EntityMap.h"

//-------------------------------------------------------------------------

namespace KRG
{
    void CoverManager::ShutdownSystem()
    {
        KRG_ASSERT( m_coverVolumes.empty() );
    }

    void CoverManager::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pCoverComponent = TryCast<CoverVolumeComponent>( pComponent ) )
        {
            m_coverVolumes.Add( pCoverComponent );
        }
    }

    void CoverManager::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        if ( auto pCoverComponent = TryCast<CoverVolumeComponent>( pComponent ) )
        {
            m_coverVolumes.Remove( pCoverComponent );
        }
    }

    //-------------------------------------------------------------------------

    void CoverManager::UpdateSystem( EntityWorldUpdateContext const& ctx )
    {
    }
}