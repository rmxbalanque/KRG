#include "CustomizerTestSystem.h"
#include "CustomizerTestComponent.h"
#include "System/Core/Update/UpdateContext.h"
#include "Engine/Render/Components/SkeletalMeshComponent.h"
#include "Engine/Animation/Components/AnimationPlayerComponent.h"
#include "Engine/Render/Components/StaticMeshComponent.h"
#include "Engine/Animation/Systems/AnimationSystem.h"
#include "System/Entity/Entity.h"
#include "System/Entity/EntityWorld.h"
#include "Engine/Animation/Components/AnimatedMeshComponent.h"
#include "System/Core/Math/Random.h"
#include "../TestSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    CustomizerTestSystem::~CustomizerTestSystem()
    {
        KRG_ASSERT( m_pComponent == nullptr );
    }

    void CustomizerTestSystem::RegisterComponent( EntityComponent* pComponent )
    {
        if ( auto pFoundComponent = ComponentCast<CustomizerTestComponent>( pComponent ) )
        {
            KRG_ASSERT( m_pComponent == nullptr );
            m_pComponent = pFoundComponent;
        }
    }

    void CustomizerTestSystem::UnregisterComponent( EntityComponent* pComponent )
    {
        if ( auto pFoundComponent = ComponentCast<CustomizerTestComponent>( pComponent ) )
        {
            if ( m_pComponent == pFoundComponent )
            {
                m_pComponent = nullptr;
            }
        }
    }

    void CustomizerTestSystem::Update( UpdateContext const& ctx )
    {
        if (  m_pComponent == nullptr )
        {
            return;
        }

        //-------------------------------------------------------------------------

        for ( auto& character : m_spawnedEntities )
        {
            if ( character.m_cooldownTimer.GetElapsedTimeMilliseconds() > character.m_cooldown )
            {
                RecustomizeCharacter( character );
            }
        }

        //-------------------------------------------------------------------------

        if ( !charactersSpawned )
        {
            charactersSpawned = true;

            //-------------------------------------------------------------------------

            auto pEntityWorld = ctx.GetSystem<EntityWorld>();
            auto pPersisentMap = pEntityWorld->GetPersistentMap();

            //-------------------------------------------------------------------------

            int32 const numPerRowCol = 10;
            for ( auto i = 0; i < numPerRowCol; i++ )
            {
                for ( auto j = 0; j < numPerRowCol; j++ )
                {
                    Transform characterTransform;
                    characterTransform.SetTranslation( Vector( (float) i, (float) j, 0 ) );
                    SpawnCharacter( pPersisentMap, characterTransform );
                }
            }
        }
    }

    void CustomizerTestSystem::SpawnCharacter( EntityModel::EntityMap* pMap, Transform const& characterTransform )
    {
        KRG_ASSERT( pMap != nullptr );

        //-------------------------------------------------------------------------

        int32 const meshIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_meshIDs.size() - 1 );
        int32 const materialIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_materialIDs.size() - 1 );
        int32 const animIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_animationIDs.size() - 1 );
        int32 const armorMeshIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_armorMeshIDs.size() - 1 );
        int32 const armorMaterialIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_materialIDs.size() - 1 );
        int32 const hairIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_hairMeshIDs.size() - 1 );
        int32 const hairMaterialIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_materialIDs.size() - 1 );

        //-------------------------------------------------------------------------

        auto pCharacterMeshComponent = KRG::New<Animation::AnimatedMeshComponent>( StringID( "Character" ) );
        pCharacterMeshComponent->SetMesh( m_pComponent->m_meshIDs[meshIdx] );
        pCharacterMeshComponent->SetMaterialOverride( 0, m_pComponent->m_materialIDs[materialIdx] );
        pCharacterMeshComponent->SetSkeleton( m_pComponent->m_skeletonID );
        pCharacterMeshComponent->SetLocalTransform( characterTransform );

        auto pArmorMeshComponent = KRG::New<Animation::AnimatedMeshComponent>( StringID( "Armor" ) );
        pArmorMeshComponent->SetMesh( m_pComponent->m_armorMeshIDs[armorMeshIdx] );
        pArmorMeshComponent->SetMaterialOverride( 0, m_pComponent->m_materialIDs[armorMaterialIdx] );
        pArmorMeshComponent->SetSkeleton( m_pComponent->m_skeletonID );

        auto pHairMeshComponent = KRG::New<Render::StaticMeshComponent>( StringID( "Hair" ) );
        pHairMeshComponent->SetMesh( m_pComponent->m_hairMeshIDs[hairIdx] );
        pHairMeshComponent->SetMaterialOverride( 0, m_pComponent->m_materialIDs[hairMaterialIdx] );
        pHairMeshComponent->SetAttachmentSocketID( StringID( "head" ) );
        pHairMeshComponent->ChangeMobility( Render::Mobility::Dynamic );

        //-------------------------------------------------------------------------

        auto pAnimationComponent = KRG::New<Animation::AnimationPlayerComponent>( StringID( "Animation" ) );
        pAnimationComponent->SetAnimation( m_pComponent->m_animationIDs[animIdx]);

        //-------------------------------------------------------------------------

        auto pEntity = KRG::New<Entity>( StringID( "Character" ) );

        pEntity->CreateSystem<Animation::AnimationSystem>();
        pEntity->AddComponent( pAnimationComponent );
        pEntity->AddComponent( pCharacterMeshComponent );
        pEntity->AddComponent( pArmorMeshComponent, pCharacterMeshComponent->GetID() );
        pEntity->AddComponent( pHairMeshComponent, pCharacterMeshComponent->GetID() );

        //-------------------------------------------------------------------------

        pMap->AddEntity( pEntity );

        //-------------------------------------------------------------------------

        CustomizedCharacter character;
        character.m_pCharacter = pEntity;
        character.m_meshComponentID = pCharacterMeshComponent->GetID();
        character.m_armorComponentID = pArmorMeshComponent->GetID();
        character.m_hairComponentID = pHairMeshComponent->GetID();
        character.m_cooldown = Milliseconds( Math::GetRandomFloat( 500, 1500 ) );
        character.m_cooldownTimer.Reset();
        character.m_createSystem = true;

        m_spawnedEntities.emplace_back( character );
    }

    void CustomizerTestSystem::RecustomizeCharacter( CustomizedCharacter& character )
    {
        Transform const characterTransform = character.m_pCharacter->GetWorldTransform();

        //-------------------------------------------------------------------------

        character.m_pCharacter->DestroyComponent( character.m_hairComponentID );
        character.m_pCharacter->DestroyComponent( character.m_armorComponentID );
        character.m_pCharacter->DestroyComponent( character.m_meshComponentID );

        //-------------------------------------------------------------------------

        int32 const meshIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_meshIDs.size() - 1 );
        int32 const materialIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_materialIDs.size() - 1 );
        int32 const animIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_animationIDs.size() - 1 );
        int32 const armorMeshIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_armorMeshIDs.size() - 1 );
        int32 const armorMaterialIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_materialIDs.size() - 1 );
        int32 const hairIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_hairMeshIDs.size() - 1 );
        int32 const hairMaterialIdx = Math::GetRandomInt( 0, (uint32) m_pComponent->m_materialIDs.size() - 1 );

        auto pCharacterMeshComponent = KRG::New<Animation::AnimatedMeshComponent>( StringID( "Character" ) );
        pCharacterMeshComponent->SetMesh( m_pComponent->m_meshIDs[meshIdx] );
        pCharacterMeshComponent->SetMaterialOverride( 0, m_pComponent->m_materialIDs[materialIdx] );
        pCharacterMeshComponent->SetSkeleton( m_pComponent->m_skeletonID );
        pCharacterMeshComponent->SetLocalTransform( characterTransform );

        auto pArmorMeshComponent = KRG::New<Animation::AnimatedMeshComponent>( StringID( "Armor" ) );
        pArmorMeshComponent->SetMesh( m_pComponent->m_armorMeshIDs[armorMeshIdx] );
        pArmorMeshComponent->SetMaterialOverride( 0, m_pComponent->m_materialIDs[armorMaterialIdx] );
        pArmorMeshComponent->SetSkeleton( m_pComponent->m_skeletonID );

        auto pHairMeshComponent = KRG::New<Render::StaticMeshComponent>( StringID( "Hair" ) );
        pHairMeshComponent->SetMesh( m_pComponent->m_hairMeshIDs[hairIdx] );
        pHairMeshComponent->SetMaterialOverride( 0, m_pComponent->m_materialIDs[hairMaterialIdx] );
        pHairMeshComponent->SetAttachmentSocketID( StringID( "head" ) );
        pHairMeshComponent->ChangeMobility( Render::Mobility::Dynamic );

        character.m_pCharacter->AddComponent( pCharacterMeshComponent );
        character.m_pCharacter->AddComponent( pArmorMeshComponent, pCharacterMeshComponent->GetID() );
        character.m_pCharacter->AddComponent( pHairMeshComponent, pCharacterMeshComponent->GetID() );

        if ( character.m_createSystem )
        {
            character.m_pCharacter->CreateSystem<GameTestSystem>();
        }
        else
        {
            character.m_pCharacter->DestroySystem<GameTestSystem>();
        }

        character.m_meshComponentID = pCharacterMeshComponent->GetID();
        character.m_armorComponentID = pArmorMeshComponent->GetID();
        character.m_hairComponentID = pHairMeshComponent->GetID();
        character.m_cooldown = Milliseconds( Math::GetRandomFloat( 500, 1500 ) );
        character.m_cooldownTimer.Reset();
        character.m_createSystem = !character.m_createSystem;
    }
}