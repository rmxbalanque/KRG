#include "AnimationGraphEditor_VariationEditor.h"
#include "Tools/Core/Resource/DataFilePicker.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    void GraphVariationEditor::UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass, char const* pWindowName )
    {
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( pWindowName, nullptr, 0 ) )
        {
            if ( ImGui::BeginTable( "VariationMainSplitter", 2, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollX ) )
            {
                ImGui::TableSetupColumn( "VariationTree", ImGuiTableColumnFlags_WidthStretch, 0.2f );
                ImGui::TableSetupColumn( "Data", ImGuiTableColumnFlags_WidthStretch );

                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                DrawVariationTree();

                ImGui::TableNextColumn();
                DrawOverridesTable();

                ImGui::EndTable();
            }

            //-------------------------------------------------------------------------

            if ( m_activeOperation != OperationType::None )
            {
                DrawActiveOperationUI();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void GraphVariationEditor::DrawVariationTreeNode( VariationHierarchy& variationHierarchy, StringID variationID )
    {
        ImGui::PushID( variationID.GetID() );

        // Open Tree Node
        //-------------------------------------------------------------------------

        bool const isSelected = m_graphModel.GetSelectedVariationID() == variationID;

        ImGuiX::PushFontAndColor( isSelected ? ImGuiX::Font::SmallBold : ImGuiX::Font::Small, isSelected ? ImGuiX::ConvertColor( Colors::LimeGreen ) : ImGuiX::Theme::s_textColor );
        bool const isTreeNodeOpen = ImGui::TreeNode( variationID.c_str() );
        ImGui::PopFont();
        ImGui::PopStyleColor();

        // Click Handler
        //-------------------------------------------------------------------------

        if ( ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
        {
            m_graphModel.SetSelectedVariation( variationID );
        }

        // Context Menu
        //-------------------------------------------------------------------------

        if ( ImGui::BeginPopupContextItem( variationID.c_str() ) )
        {
            if ( ImGui::MenuItem( "Create Child" ) )
            {
                StartCreate( variationID );
            }

            if ( variationID != AnimationGraphVariation::DefaultVariationID )
            {
                ImGui::Separator();

                if ( ImGui::MenuItem( "Rename" ) )
                {
                    StartRename( variationID );
                }

                if ( ImGui::MenuItem( "Delete" ) )
                {
                    StartDelete( variationID );
                }
            }

            ImGui::EndPopup();
        }

        // Draw node contents
        //-------------------------------------------------------------------------

        if( isTreeNodeOpen )
        {
            auto const childVariations = variationHierarchy.GetChildVariations( variationID );
            for ( StringID const& childVariationID : childVariations )
            {
                DrawVariationTreeNode( variationHierarchy, childVariationID );
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    void GraphVariationEditor::DrawVariationTree()
    {
        AnimationToolsGraph* pGraph = m_graphModel.GetGraph();
        DrawVariationTreeNode( pGraph->GetVariationHierarchy(), AnimationGraphVariation::DefaultVariationID );
    }

    void GraphVariationEditor::DrawOverridesTable()
    {
        auto dataSlotNodes = m_graphModel.GetGraph()->GetAllDataSlotNodes();
        bool isDefaultVariationSelected = m_graphModel.IsDefaultVariationSelected();

        //-------------------------------------------------------------------------

        ImGui::AlignTextToFramePadding();
        ImGui::Text( "Skeleton: " );
        ImGui::SameLine( 0, 0 );

        auto pGraph = m_graphModel.GetGraph();
        auto pVariation = pGraph->GetVariation( m_graphModel.GetSelectedVariationID() );
        ResourceID resourceID = pVariation->m_pSkeleton.GetResourceID();
        if ( DataFilePicker::DrawPickerControl( m_graphModel.GetSourceDataDirectory(), Skeleton::GetStaticResourceTypeID(), &resourceID ) )
        {
            pVariation->m_pSkeleton = resourceID;
        }

        //-------------------------------------------------------------------------

        if ( ImGui::BeginTable( "SourceTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollX ) )
        {
            ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableSetupColumn( "Path", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableSetupColumn( "Source", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableSetupColumn( "##Override", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 20 );
            ImGui::TableHeadersRow();

            //-------------------------------------------------------------------------

            StringID const currentVariationID = m_graphModel.GetSelectedVariationID();

            for ( auto pDataSlotNode : dataSlotNodes )
            {
                ImGui::PushID( pDataSlotNode );

                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::Text( pDataSlotNode->GetDisplayName() );

                ImGui::TableNextColumn();
                ImGui::Text( pDataSlotNode->GetPathFromRoot().c_str() );

                ImGui::TableNextColumn();

                // Default variations always have values created
                if ( isDefaultVariationSelected )
                {
                    ResourceID* pResourceID = pDataSlotNode->GetOverrideValueForVariation( currentVariationID );
                    DataFilePicker::DrawPickerControl( m_graphModel.GetSourceDataDirectory(), pDataSlotNode->GetSlotResourceType(), pResourceID );
                }
                else // Variation
                {
                    // If we have an override for this variation
                    if ( pDataSlotNode->HasOverrideForVariation( currentVariationID ) )
                    {
                        ResourceID* pResourceID = pDataSlotNode->GetOverrideValueForVariation( currentVariationID );
                        if ( DataFilePicker::DrawPickerControl( m_graphModel.GetSourceDataDirectory(), pDataSlotNode->GetSlotResourceType(), pResourceID ) )
                        {
                            // If we've cleared the resource ID and it's not the default, remove the override
                            if ( !pResourceID->IsValid() && !m_graphModel.IsDefaultVariationSelected() )
                            {
                                pDataSlotNode->RemoveOverride( currentVariationID );
                            }
                        }
                    }
                    else // Show current value
                    {
                        ImGui::Text( pDataSlotNode->GetValue( pGraph->GetVariationHierarchy(), currentVariationID ).c_str() );
                    }
                }

                //-------------------------------------------------------------------------

                ImGui::TableNextColumn();
                if ( !isDefaultVariationSelected )
                {
                    if ( pDataSlotNode->HasOverrideForVariation( currentVariationID ) )
                    {
                        if ( ImGuiX::ButtonColored( ImGuiX::ConvertColor( Colors::Red ), KRG_ICON_TIMES ) )
                        {
                            pDataSlotNode->RemoveOverride( currentVariationID );
                        }
                    }
                    else // Create an override
                    {
                        if ( ImGuiX::ButtonColored( ImGuiX::ConvertColor( Colors::LimeGreen ), KRG_ICON_PLUS ) )
                        {
                            pDataSlotNode->CreateOverride( currentVariationID );
                        }
                    }
                }

                ImGui::PopID();
            }

            //-------------------------------------------------------------------------

            ImGui::EndTable();
        }
    }

    //-------------------------------------------------------------------------

    void GraphVariationEditor::StartCreate( StringID variationID )
    {
        KRG_ASSERT( variationID.IsValid() );
        m_activeOperationVariationID = variationID;
        m_activeOperation = OperationType::Create;
        strncpy_s( m_buffer, "New Child Variation", 255 );
    }

    void GraphVariationEditor::StartRename( StringID variationID )
    {
        KRG_ASSERT( variationID.IsValid() );
        m_activeOperationVariationID = variationID;
        m_activeOperation = OperationType::Rename;
        strncpy_s( m_buffer, variationID.c_str(), 255 );
    }

    void GraphVariationEditor::StartDelete( StringID variationID )
    {
        KRG_ASSERT( variationID.IsValid() );
        m_activeOperationVariationID = variationID;
        m_activeOperation = OperationType::Delete;
    }

    void GraphVariationEditor::DrawActiveOperationUI()
    {
        bool isDialogOpen = m_activeOperation != OperationType::None;
        auto pGraph = m_graphModel.GetGraph();

        if ( m_activeOperation == OperationType::Create )
        {
            ImGui::OpenPopup( "Create" );
            if ( ImGui::BeginPopupModal( "Create", &isDialogOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize ) )
            {
                bool nameChangeConfirmed = false;

                ImGui::PushStyleColor( ImGuiCol_Text, pGraph->IsValidVariation( StringID( m_buffer ) ) ? ImGuiX::ConvertColor( Colors::Red ).Value : ImGuiX::Theme::s_textColor );
                if ( ImGui::InputText( "##VariationName", m_buffer, 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
                {
                    nameChangeConfirmed = true;
                }
                ImGui::PopStyleColor();
                ImGui::NewLine();

                float const dialogWidth = ( ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() ).x;
                ImGui::SameLine( 0, dialogWidth - 104 );

                if ( ImGui::Button( "Ok", ImVec2( 50, 0 ) ) || nameChangeConfirmed )
                {
                    // Only allow creations of unique variation names
                    StringID newVariationID( m_buffer );
                    if ( !pGraph->IsValidVariation( newVariationID ) )
                    {
                        pGraph->GetVariationHierarchy().CreateVariation( newVariationID, m_activeOperationVariationID );
                        m_activeOperationVariationID = StringID();
                        m_activeOperation = OperationType::None;
                    }
                }

                ImGui::SameLine( 0, 4 );

                if ( ImGui::Button( "Cancel", ImVec2( 50, 0 ) ) )
                {
                    m_activeOperation = OperationType::None;
                }

                ImGui::EndPopup();
            }
        }

        //-------------------------------------------------------------------------

        if ( m_activeOperation == OperationType::Rename )
        {
            ImGui::OpenPopup( "Rename" );
            if ( ImGui::BeginPopupModal( "Rename", &isDialogOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize ) )
            {
                bool nameChangeConfirmed = false;

                ImGui::PushStyleColor( ImGuiCol_Text, pGraph->IsValidVariation( StringID( m_buffer ) ) ? ImGuiX::ConvertColor( Colors::Red ).Value : ImGuiX::Theme::s_textColor );
                if ( ImGui::InputText( "##VariationName", m_buffer, 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
                {
                    nameChangeConfirmed = true;
                }
                ImGui::PopStyleColor();
                ImGui::NewLine();

                float const dialogWidth = ( ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() ).x;
                ImGui::SameLine( 0, dialogWidth - 104 );

                if ( ImGui::Button( "Ok", ImVec2( 50, 0 ) ) || nameChangeConfirmed )
                {
                    // Only allow rename to unique variation names
                    StringID newVariationID( m_buffer );
                    if ( !pGraph->IsValidVariation( newVariationID ) )
                    {
                        // Rename actual variation
                        pGraph->GetVariationHierarchy().RenameVariation( m_activeOperationVariationID, newVariationID );

                        // Update all data slot nodes
                        auto dataSlotNodes = pGraph->GetAllDataSlotNodes();
                        for ( auto pDataSlotNode : dataSlotNodes )
                        {
                            pDataSlotNode->RenameOverride( m_activeOperationVariationID, newVariationID );
                        }

                        m_activeOperationVariationID = StringID();
                        m_activeOperation = OperationType::None;
                    }
                }

                ImGui::SameLine( 0, 4 );

                if ( ImGui::Button( "Cancel", ImVec2( 50, 0 ) ) )
                {
                    m_activeOperation = OperationType::None;
                }

                ImGui::EndPopup();
            }
        }

        //-------------------------------------------------------------------------

        if ( m_activeOperation == OperationType::Delete )
        {
            ImGui::OpenPopup( "Delete" );
            if ( ImGui::BeginPopupModal( "Delete", &isDialogOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize ) )
            {
                ImGui::Text( "Are you sure you want to delete this variation?" );
                ImGui::NewLine();

                float const dialogWidth = ( ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() ).x;
                ImGui::SameLine( 0, dialogWidth - 64 );

                if ( ImGui::Button( "Yes", ImVec2( 30, 0 ) ) )
                {
                    KRG_ASSERT( m_activeOperationVariationID != AnimationGraphVariation::DefaultVariationID );

                    // Update selection
                    auto const pVariation = pGraph->GetVariation( m_activeOperationVariationID );
                    m_graphModel.SetSelectedVariation( pVariation->m_parentID );

                    // Destroy variation
                    pGraph->GetVariationHierarchy().DestroyVariation( m_activeOperationVariationID );
                    m_activeOperationVariationID = StringID();
                    m_activeOperation = OperationType::None;
                }

                ImGui::SameLine( 0, 4 );

                if ( ImGui::Button( "No", ImVec2( 30, 0 ) ) )
                {
                    m_activeOperation = OperationType::None;
                }

                ImGui::EndPopup();
            }
        }

        //-------------------------------------------------------------------------

        if ( !isDialogOpen )
        {
            m_activeOperation = OperationType::None;
        }
    }
}