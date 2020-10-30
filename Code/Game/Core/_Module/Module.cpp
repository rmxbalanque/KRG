#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"
#include "System/Entity/EntityWorld.h"
#include "Game/Core/PlayerController/DefaultPlayerController.h"
#include "Engine/Core/Camera/CameraComponent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Game
    {
        bool GameModule::Initialize( ModuleContext& context )
        {
            auto OnCreatePersistentEntities = [] ( EntityModel::EntityMap* pPersistentMap )
            {
                auto pCameraComponent = KRG::New<CameraComponent>( StringID( "Camera Component" ) );
                
                pCameraComponent->SetLocalTransform( Transform( Quaternion::Identity, Vector( 0, -1.5f, 1.0f ) ) );

                auto pEntity = KRG::New<Entity>( StringID( "Default Player" ) );

                pEntity->AddComponent( pCameraComponent );
                pEntity->CreateSystem<DefaultPlayerController>();
                pPersistentMap->AddEntity( pEntity );
            };

            auto pEntityWorld = context.GetEntityWorld();
            m_createPersistentEntitiesBinding = pEntityWorld->OnCreatePersistentEntities().Bind( OnCreatePersistentEntities );

            return true;
        }

        void GameModule::Shutdown( ModuleContext& context )
        {
            auto pEntityWorld = context.GetEntityWorld();
            pEntityWorld->OnCreatePersistentEntities().Unbind( m_createPersistentEntitiesBinding );
        }
    }
}