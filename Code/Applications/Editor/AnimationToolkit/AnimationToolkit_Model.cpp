#include "AnimationToolkit_Model.h"
#include "Engine/Animation/Systems/AnimationSystem.h"
#include "System/Resource/ResourceSystem.h"
#include "System/Entity/EntityWorld.h"
#include "Engine/Animation/Components/AnimationPlayerComponent.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    void Model::Initialize( UpdateContext const& context )
    {
        EditorModel::Initialize( context );

        // Create preview entity
        //-------------------------------------------------------------------------

        // We dont own the entity as soon as we add it to the map
        auto pWorld = context.GetSystem<EntityWorld>();
        auto pPersistentMap = pWorld->GetPersistentMap();

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        m_pPreviewEntity->CreateSystem<AnimationSystem>();
        m_pPreviewAnimationSystem = m_pPreviewEntity->GetSystem<AnimationSystem>();

        pPersistentMap->AddEntity( m_pPreviewEntity );
    }

    void Model::Shutdown( UpdateContext const& context )
    {
        EditorModel::Shutdown( context );

        // These need to be cleared after the base shutdown since the "OnClearActiveResource" uses them
        m_pPreviewEntity = nullptr;
        m_pPreviewAnimationSystem = nullptr;
    }

    void Model::OnSetActiveResource( ResourceID const& resourceID )
    {
        m_previewAnimationComponent = KRG::New<AnimationPlayerComponent>( StringID( "Anim Player" ) );
        m_previewAnimationComponent->SetAnimation( m_activeResource );
        m_previewAnimationComponent->SetPlayMode( AnimationPlayerComponent::PlayMode::Posed );
        m_pPreviewEntity->AddComponent( m_previewAnimationComponent );
    }

    void Model::OnClearActiveResource()
    {
        m_pPreviewEntity->DestroyComponent( m_previewAnimationComponent->GetID() );
        m_previewAnimationComponent = nullptr;
    }
}