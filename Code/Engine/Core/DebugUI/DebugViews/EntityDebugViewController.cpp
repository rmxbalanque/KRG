#include "EntityDebugViewController.h"
#include "System/Imgui/ImguiCore.h"
#include "System/Entity/EntityWorld.h"
#include "System/Entity/Map/EntityMap.h"
#include "System/Entity/EntitySystem.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    EntityDebugViewController::EntityDebugViewController()
    {
        auto drawSystemLogMenuOptions = [this] ( UpdateContext const& context )
        {
            if ( ImGui::Button( "Show World Browser" ) )
            {
                m_isWorldBrowserOpen = true;
            }
        };

        m_menuCallbacks.emplace_back( Debug::DebugMenuCallback( "MapStats", "Entity", drawSystemLogMenuOptions ) );
    }

    void EntityDebugViewController::DrawWindows( UpdateContext const& context )
    {
        KRG_ASSERT( m_pWorld != nullptr );

        if ( m_isWorldBrowserOpen )
        {
            DrawWorldBrowser( context );
        }
    }

    //-------------------------------------------------------------------------
    // WORLD BROWSER
    //-------------------------------------------------------------------------

    void EntityDebugViewController::DrawComponentEntry( EntityComponent const* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );

        ImGui::Text( pComponent->GetName().c_str() );
        ImGui::SameLine();
        ImGui::Text( " - %s - ", pComponent->GetID().ToString().c_str() );
        ImGui::SameLine();

        EntityComponent::Status const componentStatus = pComponent->GetStatus();
        
        switch ( componentStatus )
        {
            case EntityComponent::Status::LoadingFailed:
            {
                ImGui::PushStyleColor( ImGuiCol_Text, 0xFF0000FF );
                ImGui::Text( "Load Failed" );
                ImGui::PopStyleColor( 1 );
            }
            break;

            case EntityComponent::Status::Unloaded:
            {
                ImGui::PushStyleColor( ImGuiCol_Text, 0xFF666666 );
                ImGui::Text( "Unloaded" );
                ImGui::PopStyleColor( 1 );
            }
            break;

            case EntityComponent::Status::Loading:
            {
                ImGui::PushStyleColor( ImGuiCol_Text, 0xFFAAAAAA );
                ImGui::Text( "Loading" );
                ImGui::PopStyleColor( 1 );
            }
            break;

            case EntityComponent::Status::Loaded:
            {
                ImGui::PushStyleColor( ImGuiCol_Text, 0xFF00FFFF );
                ImGui::Text( "Loaded" );
                ImGui::PopStyleColor( 1 );
            }
            break;

            case EntityComponent::Status::Initialized:
            {
                ImGui::PushStyleColor( ImGuiCol_Text, 0xFF00FFFF );
                ImGui::Text( "Initialized" );
                ImGui::PopStyleColor( 1 );
            }
            break;
        }
    }

    void EntityDebugViewController::DrawSpatialComponentTree( SpatialEntityComponent const* pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );

        bool const IsNodeExpanded = ImGui::TreeNodeEx( pComponent, ImGuiTreeNodeFlags_DefaultOpen, "" );
        ImGui::SameLine();
        DrawComponentEntry( pComponent );

        if( IsNodeExpanded )
        {
            for ( auto pChildComponent : pComponent->m_spatialChildren )
            {
                DrawSpatialComponentTree( pChildComponent );
            }
        }
    }

    void EntityDebugViewController::DrawWorldBrowser( UpdateContext const& context )
    {
        KRG_ASSERT( m_isWorldBrowserOpen );

        auto drawingCtx = context.GetDrawingContext();

        //-------------------------------------------------------------------------

        m_entities.clear();

        S32 numEntities = 0 ;
        S32 numSpatialEntities = 0;
        S32 numComponents = 0;

        bool foundSelectedEntity = false;

        for ( auto& pLoadedMap : m_pWorld->m_loadedMaps )
        {
            numEntities += (S32) pLoadedMap->GetEntities().size();
            m_entities.insert( m_entities.end(), pLoadedMap->GetEntities().begin(), pLoadedMap->GetEntities().end() );

            for ( Entity* pEntity : pLoadedMap->GetEntities() )
            {
                if ( pEntity == m_pSelectedEntity )
                {
                    foundSelectedEntity = true;
                }

                numComponents += pEntity->GetNumComponents();

                if ( pEntity->IsSpatialEntity() )
                {
                    numSpatialEntities++;
                }
            }
        }

        if ( !foundSelectedEntity )
        {
            m_pSelectedEntity = nullptr;
        }

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowBgAlpha( 0.5f );
        if ( ImGui::Begin( "World", &m_isWorldBrowserOpen ) )
        {
            // Draw World Info
            //-------------------------------------------------------------------------

            ImGui::Text( "Total number of entities: %d", numEntities );
            ImGui::Text( "Total number of components: %d", numComponents );
            ImGui::Text( "Number of spatial entities: %d", numSpatialEntities );

            ImGui::Separator();

            // Draw Entity List
            //-------------------------------------------------------------------------

            ImGui::BeginGroup();

            static ImGuiTextFilter filter;
            filter.Draw( "##EntityFilter", 300 );

            ImGui::SetNextWindowBgAlpha( 0.5f );
            ImGui::BeginChild( ImGui::GetID( (void*) (intptr_t) 0 ), ImVec2( 300, -1 ), true, 0 );

            for ( auto i = 0u; i < m_entities.size(); i++ )
            {
                if ( !filter.PassFilter( m_entities[i]->GetName().c_str() ) )
                {
                    continue;
                }

                if ( m_pSelectedEntity == m_entities[i] )
                {
                    ImGui::PushStyleColor( ImGuiCol_Text, 0xFF00FFFF );
                    ImGui::Button( m_entities[i]->GetName().c_str() );
                    ImGui::PopStyleColor( 1 );
                }
                else
                {
                    if ( ImGui::Button( m_entities[i]->GetName().c_str() ) )
                    {
                        m_pSelectedEntity = m_entities[i];
                    }
                }

                //-------------------------------------------------------------------------

                if ( ImGui::IsItemHovered() )
                {
                    if ( m_entities[i]->IsSpatialEntity() )
                    {
                        drawingCtx.DrawPoint( m_entities[i]->GetWorldTransform().GetTranslation(), Colors::Yellow, 10.0f );
                    }
                }
            }

            ImGui::EndChild();
            ImGui::EndGroup();

            //-------------------------------------------------------------------------

            ImGui::SameLine();

            // Draw Entity Details
            //-------------------------------------------------------------------------

            ImGui::BeginGroup();

            ImGui::SetNextWindowBgAlpha( 0.5f );
            ImGui::BeginChild( ImGui::GetID( (void*) (intptr_t) 1 ), ImVec2( -1, -1 ), true, 0 );

            if ( m_pSelectedEntity != nullptr )
            {
                ImGui::Text( "Entity Name: %s", m_pSelectedEntity->GetName().c_str() );
                ImGui::Text( "Entity ID: %s", m_pSelectedEntity->GetID().ToString().c_str() );

                ImGui::Separator();

                //-------------------------------------------------------------------------

                if ( !m_pSelectedEntity->GetSystems().empty() )
                {
                    if ( ImGui::CollapsingHeader( "Systems", ImGuiTreeNodeFlags_DefaultOpen ) )
                    {
                        for ( auto pSystem : m_pSelectedEntity->GetSystems() )
                        {
                            ImGui::Text( pSystem->GetName() );
                        }
                    }
                }

                //-------------------------------------------------------------------------

                TInlineVector<EntityComponentPtr, 10> components;
                for ( auto pComponent : m_pSelectedEntity->GetComponents() )
                {
                    if ( auto pSpatialComponent = TEntityComponentPtr<SpatialEntityComponent>( pComponent ) )
                    {
                        continue;
                    }

                    components.emplace_back( pComponent );
                }

                if ( !components.empty() )
                {
                    if ( ImGui::CollapsingHeader( "Components", ImGuiTreeNodeFlags_DefaultOpen ) )
                    {
                        for ( auto pComponent : components )
                        {
                            DrawComponentEntry( pComponent.GetRawPtr() );
                        }
                    }
                }

                //-------------------------------------------------------------------------

                auto pRootComponent = m_pSelectedEntity->GetRootSpatialComponent();
                if ( pRootComponent != nullptr )
                {
                    if ( ImGui::CollapsingHeader( "Spatial Components", ImGuiTreeNodeFlags_DefaultOpen ) )
                    {
                        DrawSpatialComponentTree( pRootComponent );
                    }
                }
            }

            ImGui::EndChild();
            ImGui::EndGroup();

            //-------------------------------------------------------------------------

            ImGui::End();
        }
    }
}
#endif