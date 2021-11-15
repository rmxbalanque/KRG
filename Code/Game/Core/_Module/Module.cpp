#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Camera/CameraComponent.h"
#include "Engine/Core/Player/PlayerComponent.h"
#include "Engine/Core/Player/DefaultPlayerController.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Game
    {
        bool GameModule::Initialize( ModuleContext& context )
        {
            auto CreateDefaultEntities = [] ( EntityWorld* pNewWorld )
            {
                auto pCameraComponent = KRG::New<CameraComponent>( StringID( "Camera Component" ) );
                pCameraComponent->SetLocalTransform( Transform( Quaternion::Identity, Vector( 0, -1.5f, 1.0f ) ) );

                auto pPlayerComponent = KRG::New<PlayerComponent>( StringID( "Player Component" ) );

                auto pEntity = KRG::New<Entity>( StringID( "Default Player" ) );
                pEntity->AddComponent( pCameraComponent );
                pEntity->AddComponent( pPlayerComponent );
                pEntity->CreateSystem<DefaultPlayerController>();

                pNewWorld->GetPersistentMap()->AddEntity( pEntity );
            };

            auto pEntityWorld = context.GetEntityWorldManager();
            m_createDefaultEntitiesBinding = pEntityWorld->OnCreateNewWorld().Bind( CreateDefaultEntities );

            return true;
        }

        void GameModule::Shutdown( ModuleContext& context )
        {
            if ( m_createDefaultEntitiesBinding.IsValid() )
            {
                auto pEntityWorld = context.GetEntityWorldManager();
                pEntityWorld->OnCreateNewWorld().Unbind( m_createDefaultEntitiesBinding );
            }
        }
    }
}