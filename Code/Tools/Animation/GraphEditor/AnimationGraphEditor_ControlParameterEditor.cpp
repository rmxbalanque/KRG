#include "AnimationGraphEditor_ControlParameterEditor.h"
#include "EditorGraph/Animation_EditorGraph_Definition.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    GraphControlParameterEditor::GraphControlParameterEditor( EditorGraphDefinition* pGraphDefinition )
        : m_pGraphDefinition( pGraphDefinition )
    {}

    void GraphControlParameterEditor::UpdateAndDraw( UpdateContext const& context, DebugContext* pDebugContext, ImGuiWindowClass* pWindowClass, char const* pWindowName )
    {
        int32 windowFlags = 0;
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( pWindowName, nullptr, windowFlags ) )
        {
            DrawAddParameterUI();

            //-------------------------------------------------------------------------

            ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Style::s_backgroundColorLight.Value );
            ImGui::PushStyleColor( ImGuiCol_HeaderHovered, ImGuiX::Style::s_backgroundColorLight.Value );
            ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );

            if ( ImGui::BeginTable( "Parameters", 3, 0 ) )
            {
                ImGui::TableSetupColumn( "##Name", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "##Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 40 );
                ImGui::TableSetupColumn( "##Controls", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 38 );

                for ( auto pControlParameter : m_pGraphDefinition->GetControlParameters() )
                {
                    ImGui::PushID( pControlParameter );
                    ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pControlParameter->GetNodeColor() );

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( pControlParameter->GetDisplayName() );

                    ImGui::TableNextColumn();
                    ImGui::Text( GetNameForValueType( pControlParameter->GetValueType() ) );

                    ImGui::PopStyleColor();

                    ImGui::TableNextColumn();
                    if ( ImGui::Button( KRG_ICON_PENCIL, ImVec2( 19, 0 ) ) )
                    {
                        StartRename( pControlParameter->GetID() );
                    }
                    ImGui::SameLine( 0, 0 );
                    if ( ImGui::Button( KRG_ICON_TRASH, ImVec2( 19, 0 ) ) )
                    {
                        StartDelete( pControlParameter->GetID() );
                    }

                    ImGui::PopID();
                }

                //-------------------------------------------------------------------------

                for ( auto pVirtualParameter : m_pGraphDefinition->GetVirtualParameters() )
                {
                    ImGui::PushID( pVirtualParameter );
                    ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pVirtualParameter->GetNodeColor() );

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                    ImGui::Text( pVirtualParameter->GetDisplayName() );

                    ImGui::TableNextColumn();
                    ImGui::Text( GetNameForValueType( pVirtualParameter->GetValueType() ) );

                    ImGui::PopStyleColor();

                    ImGui::TableNextColumn();
                    if ( ImGui::Button( KRG_ICON_PENCIL, ImVec2( 19, 0 ) ) )
                    {
                        StartRename( pVirtualParameter->GetID() );
                    }
                    ImGui::SameLine( 0, 0 );
                    if ( ImGui::Button( KRG_ICON_TRASH, ImVec2( 19, 0 ) ) )
                    {
                        StartDelete( pVirtualParameter->GetID() );
                    }

                    ImGui::PopID();
                }

                ImGui::EndTable();
                ImGui::PopStyleColor( 3 );
            }

            //-------------------------------------------------------------------------

            if ( m_activeOperation != OperationType::None )
            {
                DrawActiveOperationUI();
            }
        }
        ImGui::End();
        ImGui::PopStyleVar( 1 );
    }

    void GraphControlParameterEditor::DrawAddParameterUI()
    {
        if ( ImGui::Button( "Add New Parameter", ImVec2( -1, 0 ) ) )
        {
            ImGui::OpenPopup( "AddParameterPopup" );
        }

        ImGui::Separator();

        if ( ImGui::BeginPopup( "AddParameterPopup" ) )
        {
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

            ImGui::Separator();

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

    //-------------------------------------------------------------------------

    void GraphControlParameterEditor::StartRename( UUID const& parameterID )
    {
        KRG_ASSERT( parameterID.IsValid() );
        m_currentOperationParameterID = parameterID;
        m_activeOperation = OperationType::Rename;

        if ( auto pControlParameter = m_pGraphDefinition->FindControlParameter( parameterID ) )
        {
            strncpy_s( m_buffer, pControlParameter->GetDisplayName(), 255 );
        }
        else if ( auto pVirtualParameter = m_pGraphDefinition->FindVirtualParameter( parameterID ) )
        {
            strncpy_s( m_buffer, pVirtualParameter->GetDisplayName(), 255 );
        }
        else
        {
            KRG_UNREACHABLE_CODE();
        }
    }

    void GraphControlParameterEditor::StartDelete( UUID const& parameterID )
    {
        m_currentOperationParameterID = parameterID;
        m_activeOperation = OperationType::Delete;
    }

    void GraphControlParameterEditor::DrawActiveOperationUI()
    {
        bool isDialogOpen = m_activeOperation != OperationType::None;

        if ( m_activeOperation == OperationType::Rename )
        {
            ImGui::OpenPopup( "Rename Dialog" );
            if ( ImGui::BeginPopupModal( "Rename Dialog", &isDialogOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize ) )
            {
                KRG_ASSERT( m_activeOperation == OperationType::Rename );
                bool nameChangeConfirmed = false;
                if ( ImGui::InputText( "##ParameterName", m_buffer, 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
                {
                    nameChangeConfirmed = true;
                }
                ImGui::NewLine();

                float const dialogWidth = ( ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() ).x;
                ImGui::SameLine( 0, dialogWidth - 104 );

                if ( ImGui::Button( "Ok", ImVec2( 50, 0 ) ) || nameChangeConfirmed )
                {
                    if ( auto pControlParameter = m_pGraphDefinition->FindControlParameter( m_currentOperationParameterID ) )
                    {
                        m_pGraphDefinition->RenameControlParameter( m_currentOperationParameterID, m_buffer );
                    }
                    else if ( auto pVirtualParameter = m_pGraphDefinition->FindVirtualParameter( m_currentOperationParameterID ) )
                    {
                        m_pGraphDefinition->RenameVirtualParameter( m_currentOperationParameterID, m_buffer );
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
}