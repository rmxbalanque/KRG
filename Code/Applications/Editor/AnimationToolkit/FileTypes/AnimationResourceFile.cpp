#include "AnimationResourceFile.h"
#include "Applications/Editor/AnimationToolkit/AnimationEventEditor.h"
#include "Applications/Editor/Editor/EditorModel.h"
#include "Engine/Animation/Components/AnimationPlayerComponent.h"
#include "Engine/Animation/Systems/AnimationSystem.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    AnimationResourceFile::~AnimationResourceFile()
    {
        if ( m_pTrackEditor != nullptr )
        {
            KRG::Delete( m_pTrackEditor );
        }
    }

    void AnimationResourceFile::SetActive( EntityWorld* pPreviewWorld )
    {
        KRG_ASSERT( m_pPreviewEntity == nullptr );

        // We dont own the entity as soon as we add it to the map
        auto pPersistentMap = pPreviewWorld->GetPersistentMap();

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        pPersistentMap->AddEntity( m_pPreviewEntity );

        m_pPreviewEntity->CreateSystem<AnimationSystem>();

        m_pAnimationComponent = KRG::New<AnimationPlayerComponent>( StringID( "Animation Component" ) );
        m_pAnimationComponent->SetAnimation( m_pResource.GetResourceID() );
        m_pPreviewEntity->AddComponent( m_pAnimationComponent );
    }

    void AnimationResourceFile::SetInactive( EntityWorld* pPreviewWorld )
    {
        KRG_ASSERT( m_pPreviewEntity != nullptr );

        auto pPersistentMap = pPreviewWorld->GetPersistentMap();
        pPersistentMap->DestroyEntity( m_pPreviewEntity->GetID() );
        m_pPreviewEntity = nullptr;
        m_pAnimationComponent = nullptr;
    }

    void AnimationResourceFile::DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( IsLoading() )
        {
            ImGui::Text( "Loading:" );
            ImGui::SameLine();
            ImGuiX::DrawSpinner( "Loading" );
            return;
        }

        if ( HasLoadingFailed() )
        {
            ImGui::Text( "Loading Failed: %s", m_pResource.GetResourceID().c_str() );
            return;
        }

        //-------------------------------------------------------------------------

        DrawAnimationInfo( context );
        DrawAnimationTimeline( context );
    }

    void AnimationResourceFile::DrawAnimationInfo( UpdateContext const& context )
    {
        ImGui::AlignTextToFramePadding();

        //-------------------------------------------------------------------------

        ImGui::TextColored( Colors::LightPink.ToFloat4(), "Average Velocity: %.2fm/s", 4.0f );

        ImGui::SameLine( 0, 15 );
        ImGui::TextColored( Colors::LightPink.ToFloat4(), "Distance Covered: %.2fm", 10 );

        ImGui::SameLine( 0, 15 );
        ImGui::TextColored( Colors::LightPink.ToFloat4(), "Frame: %.2f / %d", 0, m_pResource->GetNumFrames() );

        ImGui::SameLine( 0, 15 );
        ImGui::TextColored( Colors::LightPink.ToFloat4(), "Time: %.2fs / %.2fs", 0, m_pResource->GetDuration() );
    }

    void AnimationResourceFile::DrawAnimationTimeline( UpdateContext const& context )
    {
        if ( m_pTrackEditor == nullptr )
        {
            auto pTypeRegistry = context.GetSystem<TypeSystem::TypeRegistry>();
            m_pTrackEditor = KRG::New<AnimationEventEditor>( *pTypeRegistry, m_pResource.GetPtr() );
        }

        //-------------------------------------------------------------------------

        m_pTrackEditor->Update( context, m_pAnimationComponent );
    }
}