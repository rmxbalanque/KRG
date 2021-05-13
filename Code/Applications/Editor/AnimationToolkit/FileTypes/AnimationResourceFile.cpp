#include "AnimationResourceFile.h"
#include "Applications/Editor/Editor/EditorModel.h"
#include "Tools/Animation/Events/AnimationEventEditor.h"
#include "Engine/Animation/Components/AnimationPlayerComponent.h"
#include "Engine/Animation/Systems/AnimationSystem.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Entity/EntityWorld.h"
#include "Tools/Core/TypeSystem/Serialization/TypeReader.h"
#include "Tools/Animation/ResourceCompilers/AnimationCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
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

        if ( m_pTrackEditor == nullptr )
        {
            m_pTrackEditor = KRG::New<Tools::EventEditor>( m_pModel->GetTypeRegistry(), m_pModel->GetSourceDataDirectory(), m_pResource.GetPtr() );
        }

        // Anim Info
        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 0 ) );
        ImGui::BeginChild( "AnimInfo", ImVec2( 0, 18 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding );
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextColored( Colors::LightPink.ToFloat4(), "Average Velocity: %.2fm/s", 4.0f );

            ImGui::SameLine( 0, 15 );
            ImGui::TextColored( Colors::LightPink.ToFloat4(), "Distance Covered: %.2fm", 10 );

            ImGui::SameLine( 0, 15 );
            ImGui::TextColored( Colors::LightPink.ToFloat4(), "Frame: %.2f / %d", 0, m_pResource->GetNumFrames() );

            ImGui::SameLine( 0, 15 );
            ImGui::TextColored( Colors::LightPink.ToFloat4(), "Time: %.2fs / %.2fs", 0, m_pResource->GetDuration() );
        }
        ImGui::EndChild();
        ImGui::PopStyleVar();

        // Track editor and property grid
        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 0, 0 ) );
        if ( ImGui::BeginTable( "BlahTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders ) )
        {
            ImGui::TableSetupColumn( "TrackEditor", ImGuiTableColumnFlags_NoHide );
            ImGui::TableSetupColumn( "PropertyGrid", ImGuiTableColumnFlags_NoHide );

            ImGui::TableNextRow();

            ImGui::TableNextColumn();
            ImGui::BeginChild( "TE", ImVec2( 0, 0 ), false );
            {
                m_pTrackEditor->Update( context, m_pAnimationComponent );
            }
            ImGui::EndChild();

            ImGui::TableNextColumn();
            ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 0 ) );
            ImGui::BeginChild( "PG", ImVec2( 0, 0 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding );
            {
                m_propertyGrid.DrawGrid();
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
    }
}