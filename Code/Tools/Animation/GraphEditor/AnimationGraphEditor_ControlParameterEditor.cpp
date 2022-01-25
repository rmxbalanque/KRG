#include "AnimationGraphEditor_ControlParameterEditor.h"
#include "EditorGraph/Animation_EditorGraph_Definition.h"
#include "Tools/Core/Helpers/CategoryTree.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    GraphControlParameterEditor::GraphControlParameterEditor( EditorGraphDefinition* pGraphDefinition )
        : m_pGraphDefinition( pGraphDefinition )
    {}

    bool GraphControlParameterEditor::UpdateAndDraw( UpdateContext const& context, DebugContext* pDebugContext, ImGuiWindowClass* pWindowClass, char const* pWindowName )
    {
        m_pVirtualParamaterToEdit = nullptr;

        int32 windowFlags = 0;
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( pWindowName, nullptr, windowFlags ) )
        {
            if ( pDebugContext != nullptr )
            {
                DrawParameterPreviewControls( pDebugContext );
            }
            else
            {
                DrawParameterList();
            }

            //-------------------------------------------------------------------------

            if ( m_activeOperation != OperationType::None )
            {
                DrawDialogs();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar( 1 );

        //-------------------------------------------------------------------------

        return m_pVirtualParamaterToEdit != nullptr;
    }

    void GraphControlParameterEditor::DrawAddParameterCombo()
    {
        if ( ImGui::Button( "Add New Parameter", ImVec2( -1, 0 ) ) )
        {
            ImGui::OpenPopup( "AddParameterPopup" );
        }

        if ( ImGui::BeginPopup( "AddParameterPopup" ) )
        {
            ImGuiX::TextSeparator( "Control Parameters" );

            if ( ImGui::MenuItem( "Control Parameter - Bool" ) )
            {
                m_pGraphDefinition->CreateControlParameter( GraphValueType::Bool );
            }

            if ( ImGui::MenuItem( "Control Parameter - ID" ) )
            {
                m_pGraphDefinition->CreateControlParameter( GraphValueType::ID );
            }

            if ( ImGui::MenuItem( "Control Parameter - Int" ) )
            {
                m_pGraphDefinition->CreateControlParameter( GraphValueType::Int );
            }

            if ( ImGui::MenuItem( "Control Parameter - Float" ) )
            {
                m_pGraphDefinition->CreateControlParameter( GraphValueType::Float );
            }

            if ( ImGui::MenuItem( "Control Parameter - Vector" ) )
            {
                m_pGraphDefinition->CreateControlParameter( GraphValueType::Vector );
            }

            if ( ImGui::MenuItem( "Control Parameter - Target" ) )
            {
                m_pGraphDefinition->CreateControlParameter( GraphValueType::Target );
            }

            //-------------------------------------------------------------------------

            ImGuiX::TextSeparator( "Virtual Parameters" );

            if ( ImGui::MenuItem( "Virtual Parameter - Bool" ) )
            {
                m_pGraphDefinition->CreateVirtualParameter( GraphValueType::Bool );
            }

            if ( ImGui::MenuItem( "Virtual Parameter - ID" ) )
            {
                m_pGraphDefinition->CreateVirtualParameter( GraphValueType::ID );
            }

            if ( ImGui::MenuItem( "Virtual Parameter - Int" ) )
            {
                m_pGraphDefinition->CreateVirtualParameter( GraphValueType::Int );
            }

            if ( ImGui::MenuItem( "Virtual Parameter - Float" ) )
            {
                m_pGraphDefinition->CreateVirtualParameter( GraphValueType::Float );
            }

            if ( ImGui::MenuItem( "Virtual Parameter - Vector" ) )
            {
                m_pGraphDefinition->CreateVirtualParameter( GraphValueType::Vector );
            }

            if ( ImGui::MenuItem( "Virtual Parameter - Target" ) )
            {
                m_pGraphDefinition->CreateVirtualParameter( GraphValueType::Target );
            }

            if ( ImGui::MenuItem( "Virtual Parameter - Bone Mask" ) )
            {
                m_pGraphDefinition->CreateVirtualParameter( GraphValueType::BoneMask );
            }

            ImGui::EndPopup();
        }
    }

    void GraphControlParameterEditor::DrawDialogs()
    {
        bool isDialogOpen = m_activeOperation != OperationType::None;

        if ( m_activeOperation == OperationType::Rename )
        {
            ImGui::OpenPopup( "Rename Dialog" );
            if ( ImGui::BeginPopupModal( "Rename Dialog", &isDialogOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize ) )
            {
                KRG_ASSERT( m_activeOperation == OperationType::Rename );
                bool updateConfirmed = false;

                ImGui::AlignTextToFramePadding();
                ImGui::Text( "Name: " );
                ImGui::SameLine( 80 );
                if ( ImGui::InputText( "##ParameterName", m_parameterNameBuffer, 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
                {
                    updateConfirmed = true;
                }

                ImGui::AlignTextToFramePadding();
                ImGui::Text( "Category: " );
                ImGui::SameLine( 80 );
                if ( ImGui::InputText( "##ParameterCategory", m_parameterCategoryBuffer, 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
                {
                    updateConfirmed = true;
                }

                ImGui::NewLine();

                float const dialogWidth = ( ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() ).x;
                ImGui::SameLine( 0, dialogWidth - 104 );

                if ( ImGui::Button( "Ok", ImVec2( 50, 0 ) ) || updateConfirmed )
                {
                    if ( auto pControlParameter = m_pGraphDefinition->FindControlParameter( m_currentOperationParameterID ) )
                    {
                        m_pGraphDefinition->RenameControlParameter( m_currentOperationParameterID, m_parameterNameBuffer, m_parameterCategoryBuffer );
                    }
                    else if ( auto pVirtualParameter = m_pGraphDefinition->FindVirtualParameter( m_currentOperationParameterID ) )
                    {
                        m_pGraphDefinition->RenameVirtualParameter( m_currentOperationParameterID, m_parameterNameBuffer, m_parameterCategoryBuffer );
                    }
                    else
                    {
                        KRG_UNREACHABLE_CODE();
                    }

                    m_activeOperation = OperationType::None;
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
            ImGui::OpenPopup( "Delete Dialog" );
            if ( ImGui::BeginPopupModal( "Delete Dialog", &isDialogOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize ) )
            {
                KRG_ASSERT( m_activeOperation == OperationType::Delete );

                ImGui::Text( "Are you sure you want to delete this parameter?" );
                ImGui::NewLine();

                float const dialogWidth = ( ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() ).x;
                ImGui::SameLine( 0, dialogWidth - 64 );

                if ( ImGui::Button( "Yes", ImVec2( 30, 0 ) ) )
                {
                    if ( auto pControlParameter = m_pGraphDefinition->FindControlParameter( m_currentOperationParameterID ) )
                    {
                        m_pGraphDefinition->DestroyControlParameter( m_currentOperationParameterID );
                    }
                    else if ( auto pVirtualParameter = m_pGraphDefinition->FindVirtualParameter( m_currentOperationParameterID ) )
                    {
                        m_pGraphDefinition->DestroyVirtualParameter( m_currentOperationParameterID );
                    }
                    else
                    {
                        KRG_UNREACHABLE_CODE();
                    }

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

    void GraphControlParameterEditor::DrawParameterList()
    {
        CategoryTree<GraphNodes::EditorGraphNode*> parameterTree;

        for ( auto pControlParameter : m_pGraphDefinition->GetControlParameters() )
        {
            parameterTree.AddItem( pControlParameter->GetParameterCategory(), pControlParameter->GetDisplayName(), pControlParameter );
        }

        for ( auto pVirtualParameter : m_pGraphDefinition->GetVirtualParameters() )
        {
            parameterTree.AddItem( pVirtualParameter->GetParameterCategory(), pVirtualParameter->GetDisplayName(), pVirtualParameter );
        }

        //-------------------------------------------------------------------------

        DrawAddParameterCombo();

        //-------------------------------------------------------------------------

        auto DrawCategoryRow = [] ( String const& categoryName )
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text( categoryName.c_str() );
        };

        auto DrawControlParameterRow = [this] ( GraphNodes::ControlParameterEditorNode* pControlParameter )
        {
            ImGui::PushID( pControlParameter );
            ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pControlParameter->GetNodeTitleColor() );

            ImGui::TableNextRow();
            ImGui::TableNextColumn();

            ImGui::AlignTextToFramePadding();
            ImGui::Indent();
            ImGui::Selectable( pControlParameter->GetDisplayName() );
            if ( ImGui::BeginDragDropSource() )
            {
                ImGui::SetDragDropPayload( "AnimGraphParameter", pControlParameter->GetDisplayName(), strlen( pControlParameter->GetDisplayName() ) + 1 );
                ImGui::Text( pControlParameter->GetDisplayName() );
                ImGui::EndDragDropSource();
            }
            ImGui::Unindent();

            ImGui::TableNextColumn();
            ImGui::Text( GetNameForValueType( pControlParameter->GetValueType() ) );

            ImGui::PopStyleColor();

            ImGui::TableNextColumn();
            ImGui::Dummy( ImVec2( 19, 0 ) );
            ImGui::SameLine( 0, 0 );
            if ( ImGui::Button( KRG_ICON_SPELL_CHECK, ImVec2( 19, 0 ) ) )
            {
                StartParameterRename( pControlParameter->GetID() );
            }
            ImGuiX::ItemTooltip( "Rename Parameter" );

            ImGui::SameLine( 0, 0 );
            if ( ImGui::Button( KRG_ICON_TRASH, ImVec2( 19, 0 ) ) )
            {
                StartParameterDelete( pControlParameter->GetID() );
            }
            ImGuiX::ItemTooltip( "Delete Parameter" );

            ImGui::PopID();
        };

        auto DrawVirtualParameterRow = [this] ( GraphNodes::VirtualParameterEditorNode* pVirtualParameter )
        {
            ImGui::PushID( pVirtualParameter );
            ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pVirtualParameter->GetNodeTitleColor() );

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Indent();
            ImGui::Selectable( pVirtualParameter->GetDisplayName() );
            if ( ImGui::BeginDragDropSource() )
            {
                ImGui::SetDragDropPayload( "AnimGraphParameter", pVirtualParameter->GetDisplayName(), strlen( pVirtualParameter->GetDisplayName() ) + 1 );
                ImGui::Text( pVirtualParameter->GetDisplayName() );
                ImGui::EndDragDropSource();
            }
            ImGui::Unindent();

            ImGui::TableNextColumn();
            ImGui::Text( GetNameForValueType( pVirtualParameter->GetValueType() ) );

            ImGui::PopStyleColor();

            ImGui::TableNextColumn();
            if ( ImGui::Button( KRG_ICON_EDIT, ImVec2( 19, 0 ) ) )
            {
                m_pVirtualParamaterToEdit = pVirtualParameter;
            }
            ImGuiX::ItemTooltip( "Show Virtual Parameter Graph" );

            ImGui::SameLine( 0, 0 );
            if ( ImGui::Button( KRG_ICON_SPELL_CHECK, ImVec2( 19, 0 ) ) )
            {
                StartParameterRename( pVirtualParameter->GetID() );
            }
            ImGuiX::ItemTooltip( "Rename Parameter" );

            ImGui::SameLine( 0, 0 );
            if ( ImGui::Button( KRG_ICON_TRASH, ImVec2( 19, 0 ) ) )
            {
                StartParameterDelete( pVirtualParameter->GetID() );
            }
            ImGuiX::ItemTooltip( "Delete Parameter" );

            ImGui::PopID();
        };

        //-------------------------------------------------------------------------

        ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Style::s_backgroundColorLight.Value );
        ImGui::PushStyleColor( ImGuiCol_HeaderHovered, ImGuiX::Style::s_backgroundColorLight.Value );
        ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );

        //ImGuiX::TextSeparator( "Control Parameters" );

        if ( ImGui::BeginTable( "CPT", 3, 0 ) )
        {
            ImGui::TableSetupColumn( "##Name", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableSetupColumn( "##Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 40 );
            ImGui::TableSetupColumn( "##Controls", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 60 );

            //-------------------------------------------------------------------------

            DrawCategoryRow( "General" );

            for ( auto const& item : parameterTree.GetRootCategory().m_items )
            {
                if ( auto pCP = TryCast<GraphNodes::ControlParameterEditorNode>( item.m_data ) )
                {
                    DrawControlParameterRow( pCP );
                }
                else
                {
                    DrawVirtualParameterRow( TryCast<GraphNodes::VirtualParameterEditorNode>( item.m_data ) );
                }
            }

            //-------------------------------------------------------------------------

            for ( auto const& category : parameterTree.GetRootCategory().m_childCategories )
            {
                DrawCategoryRow( category.m_name );

                for ( auto const& item : category.m_items )
                {
                    if ( auto pCP = TryCast<GraphNodes::ControlParameterEditorNode>( item.m_data ) )
                    {
                        DrawControlParameterRow( pCP );
                    }
                    else
                    {
                        DrawVirtualParameterRow( TryCast<GraphNodes::VirtualParameterEditorNode>( item.m_data ) );
                    }
                }
            }

            ImGui::EndTable();
        }

        /*ImGuiX::TextSeparator( "Virtual Parameters" );

        if ( ImGui::BeginTable( "VPT", 3, 0 ) )
        {
            ImGui::TableSetupColumn( "##Name", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableSetupColumn( "##Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 40 );
            ImGui::TableSetupColumn( "##Controls", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 60 );

            for ( auto pVirtualParameter : m_pGraphDefinition->GetVirtualParameters() )
            {
              
            }

            ImGui::EndTable();
        }*/

        ImGui::PopStyleColor( 3 );
    }

    void GraphControlParameterEditor::DrawParameterPreviewControls( DebugContext* pDebugContext )
    {
        KRG_ASSERT( pDebugContext != nullptr );

        int32 const numParameters = m_pGraphDefinition->GetNumControlParameters();
        m_parameterPreviewBuffers.resize( numParameters );

        if ( ImGui::BeginTable( "Parameter Preview", 2, ImGuiTableFlags_Resizable, ImVec2( -1, -1 ) ) )
        {
            ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableSetupColumn( "Editor", ImGuiTableColumnFlags_WidthStretch );

            //-------------------------------------------------------------------------

            for ( int32 i = 0; i < numParameters; i++ )
            { 
                auto pControlParameter = m_pGraphDefinition->GetControlParameters()[i];
                GraphNodeIndex const parameterIdx = pDebugContext->GetRuntimeGraphNodeIndex( pControlParameter->GetID() );

                ImGui::PushID( pControlParameter );
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex( 0 );
                ImGui::AlignTextToFramePadding();
                ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pControlParameter->GetNodeTitleColor() );
                ImGui::Text( pControlParameter->GetDisplayName() );
                ImGui::PopStyleColor();

                ImGui::TableSetColumnIndex( 1 );
                switch ( pControlParameter->GetValueType() )
                {
                    case GraphValueType::Bool:
                    {
                        auto value = pDebugContext->m_pGraphComponent->GetControlParameterValue<bool>( parameterIdx );
                        if ( ImGui::Checkbox( "##bp", &value ) )
                        {
                            pDebugContext->m_pGraphComponent->SetControlParameterValue( parameterIdx, value );
                        }
                    }
                    break;

                    case GraphValueType::Int:
                    {
                        auto value = pDebugContext->m_pGraphComponent->GetControlParameterValue<int32>( parameterIdx );
                        ImGui::SetNextItemWidth( -1 );
                        if ( ImGui::InputInt( "##ip", &value ) )
                        {
                            pDebugContext->m_pGraphComponent->SetControlParameterValue( parameterIdx, value );
                        }
                    }
                    break;

                    case GraphValueType::Float:
                    {
                        auto value = pDebugContext->m_pGraphComponent->GetControlParameterValue<float>( parameterIdx );
                        ImGui::SetNextItemWidth( -1 );
                        if ( ImGui::InputFloat( "##fp", &value ) )
                        {
                            pDebugContext->m_pGraphComponent->SetControlParameterValue( parameterIdx, value );
                        }
                    }
                    break;

                    case GraphValueType::Vector:
                    {
                        auto value = pDebugContext->m_pGraphComponent->GetControlParameterValue<Vector>( parameterIdx ).ToFloat4();
                        ImGui::SetNextItemWidth( -1 );
                        if ( ImGui::InputFloat4( "##vp", &value.m_x ) )
                        {
                            pDebugContext->m_pGraphComponent->SetControlParameterValue( parameterIdx, value );
                        }
                    }
                    break;

                    case GraphValueType::ID:
                    {
                        auto value = pDebugContext->m_pGraphComponent->GetControlParameterValue<StringID>( parameterIdx );
                        if ( value.IsValid() )
                        {
                            strncpy_s( m_parameterPreviewBuffers[i].data(), 255, value.c_str(), strlen( value.c_str() ) );
                        }
                        else
                        {
                            memset( m_parameterPreviewBuffers[i].data(), 0, 255 );
                        }

                        ImGui::SetNextItemWidth( -1 );
                        if ( ImGui::InputText( "##tp", m_parameterPreviewBuffers[i].data(), 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
                        {
                            pDebugContext->m_pGraphComponent->SetControlParameterValue( parameterIdx, StringID( m_parameterPreviewBuffers[i].data() ) );
                        }
                    }
                    break;

                    case GraphValueType::Target:
                    {
                        // TODO
                    }
                    break;
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }
    }

    //-------------------------------------------------------------------------

    void GraphControlParameterEditor::StartParameterRename( UUID const& parameterID )
    {
        KRG_ASSERT( parameterID.IsValid() );
        m_currentOperationParameterID = parameterID;
        m_activeOperation = OperationType::Rename;

        if ( auto pControlParameter = m_pGraphDefinition->FindControlParameter( parameterID ) )
        {
            strncpy_s( m_parameterNameBuffer, pControlParameter->GetDisplayName(), 255 );
            strncpy_s( m_parameterCategoryBuffer, pControlParameter->GetParameterCategory().c_str(), 255);
        }
        else if ( auto pVirtualParameter = m_pGraphDefinition->FindVirtualParameter( parameterID ) )
        {
            strncpy_s( m_parameterNameBuffer, pVirtualParameter->GetDisplayName(), 255 );
            strncpy_s( m_parameterCategoryBuffer, pControlParameter->GetParameterCategory().c_str(), 255 );
        }
        else
        {
            KRG_UNREACHABLE_CODE();
        }
    }

    void GraphControlParameterEditor::StartParameterDelete( UUID const& parameterID )
    {
        m_currentOperationParameterID = parameterID;
        m_activeOperation = OperationType::Delete;
    }
}