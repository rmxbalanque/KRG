#include "EntityEditor_Outliner.h"
#include "EntityEditor_Context.h"
#include "Engine/Core/Entity/EntityMap.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Core/Math/Random.h"
#include "Engine/Core/Entity/Entity.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityEditorOutliner::EntityEditorOutliner( EntityEditorContext& ctx )
        : m_context( ctx )
    {}

    void EntityEditorOutliner::Draw( UpdateContext const& context )
    {
        EntityMap const* pMap = m_context.GetMap();
        if ( pMap == nullptr )
        {
            return;
        }

        // Create New Entity
        //-------------------------------------------------------------------------

        {
            ImGuiX::ScopedFont const sf( ImGuiX::Font::SmallBold );
            if ( ImGuiX::ColoredButton( Colors::Green, Colors::White, "CREATE NEW ENTITY", ImVec2( -1, 0 ) ) )
            {
                InlineString entityName( "New Entity" );
                StringID entityNameID( entityName.c_str() );
                for ( auto pEntity : pMap->GetEntities() )
                {
                    if ( pEntity->GetName() == entityNameID )
                    {
                        entityName.sprintf( "New Entity %u", (uint32) Math::GetRandomUInt() );
                        entityNameID = StringID( entityName.c_str() );
                        break;
                    }
                }

                Entity* pEntity = m_context.CreateEntity();
                m_context.SelectEntity( pEntity );
            }
        }

        //-------------------------------------------------------------------------

        if ( ImGui::BeginChild( "EntityList", ImVec2( -1, 0 ) ) )
        {
            int32 const numEntities = (int32) pMap->GetEntities().size();
            for ( int32 i = 0; i < numEntities; i++ )
            {
                Entity* pEntity = pMap->GetEntities()[i];

                bool const isSelected = m_context.IsSelected( pEntity );
                if ( isSelected )
                {
                    ImGuiX::PushFontAndColor( ImGuiX::Font::SmallBold, ImGuiX::Style::s_selectionAccent );
                }

                String const buttonLabel( String::CtorSprintf(), "%s##%u", pEntity->GetName().c_str(), pEntity->GetID().ToUint64() );
                ImGui::Selectable( buttonLabel.c_str(), isSelected, 0 );

                if ( isSelected )
                {
                    ImGui::PopFont();
                    ImGui::PopStyleColor();
                }

                if ( ImGui::IsItemHovered( ImGuiHoveredFlags_AllowWhenBlockedByPopup ) )
                {
                    if ( pEntity->IsSpatialEntity() )
                    {
                        auto drawingCtx = m_context.GetDrawingContext();
                        drawingCtx.DrawWireBox( pEntity->GetCombinedWorldBounds(), (ImVec4) ImGuiX::Style::s_selectionAccent, 2.0f );
                    }

                    // Left click follows regular selection logic
                    if ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) )
                    {
                        if ( ImGui::GetIO().KeyShift )
                        {
                            // TODO
                            // Find selection bounds and bulk select everything between them
                        }
                        else if ( ImGui::GetIO().KeyCtrl )
                        {
                            if ( isSelected )
                            {
                                m_context.RemoveFromSelectedEntities( pEntity );
                            }
                            else
                            {
                                m_context.AddToSelectedEntities( pEntity );
                            }
                        }
                        else
                        {
                            m_context.SelectEntity( pEntity );
                        }
                    }
                    // Right click never deselects! Nor does it support multi-selection
                    else if ( ImGui::IsMouseClicked( ImGuiMouseButton_Right ) )
                    {
                        if ( !isSelected )
                        {
                            m_context.SelectEntity( pEntity );
                        }
                    }
                    else if ( ImGui::IsKeyReleased( ImGui::GetKeyIndex( ImGuiKey_Enter ) ) )
                    {
                        m_context.SelectEntity( pEntity );
                    }
                }

                // Handle context menu
                if ( ImGui::BeginPopupContextItem( buttonLabel.c_str() ) )
                {
                    if ( ImGui::MenuItem( "Delete" ) )
                    {
                        m_context.DestroySelectedEntities();
                    }

                    ImGui::EndPopup();
                }
            }
        }
        ImGui::EndChild();
    }
}