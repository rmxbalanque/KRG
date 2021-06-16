#include "AnimationGraphEditor_ControlParameterEditor.h"
#include "System/DevTools/ImguiX.h"
#include "Tools/Animation/Graph/AnimationGraphTools_AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    GraphControlParameterEditor::GraphControlParameterEditor( EditorModel* pModel )
        : TEditorTool<GraphEditorModel>( pModel )
    {}

    void GraphControlParameterEditor::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( !IsOpen() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        ImGui::ShowDemoWindow();

        //-------------------------------------------------------------------------

        auto pGraph = m_model.GetGraph();
        if ( pGraph == nullptr )
        {
            if ( ImGui::Begin( "Control Parameters" ) )
            {
                ImGui::Text( "No Graph Open" );
            }
            ImGui::End();
            return;
        }

        //-------------------------------------------------------------------------

        int32 windowFlags = 0;
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
        if ( ImGui::Begin( "Control Parameters", nullptr, windowFlags ) )
        {
            DrawAddParameterUI();

            //-------------------------------------------------------------------------

            ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Theme::s_backgroundColorLight );
            ImGui::PushStyleColor( ImGuiCol_HeaderHovered, ImGuiX::Theme::s_backgroundColorLight );
            ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0, 0, 0, 0 ) );

            if ( ImGui::BeginTable( "Parameters", 3, 0 ) )
            {
                ImGui::TableSetupColumn( "##Name", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "##Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 40 );
                ImGui::TableSetupColumn( "##Controls", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 38 );

                for ( auto pControlParameter : pGraph->GetControlParameters() )
                {
                    ImGui::PushID( pControlParameter );
                    ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pControlParameter->GetHighlightColor() );

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();

                    bool const isParameterSelected = m_model.IsSelected( pControlParameter );
                    ImGui::AlignTextToFramePadding();
                    if ( ImGui::Selectable( pControlParameter->GetDisplayName(), isParameterSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2( 0, 0 ) ) )
                    {
                        m_model.SetSelection( pControlParameter );
                    }

                    ImGui::TableNextColumn();
                    ImGui::Text( GetNameForValueType( pControlParameter->GetValueType() ) );

                    ImGui::PopStyleColor();

                    ImGui::TableNextColumn();
                    if ( ImGui::Button( KRG_ICON_PENCIL, ImVec2( 19, 0 ) ) )
                    {
                        StartRename( pControlParameter->GetID() );
                    }
                    ImGui::SameLine(0, 0);
                    if ( ImGui::Button( KRG_ICON_TRASH, ImVec2( 19, 0 ) ) )
                    {
                        StartDelete( pControlParameter->GetID() );
                    }

                    ImGui::PopID();
                }

                //-------------------------------------------------------------------------

                for ( auto pVirtualParameter : pGraph->GetVirtualParameters() )
                {
                    ImGui::PushID( pVirtualParameter );
                    ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pVirtualParameter->GetHighlightColor() );

                    ImGui::TableNextRow();
                    ImGui::TableNextColumn();
                     
                    bool const isParameterSelected = m_model.IsSelected( pVirtualParameter );
                    if ( ImGui::Selectable( pVirtualParameter->GetDisplayName(), isParameterSelected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap, ImVec2( 0, 0 ) ) )
                    {
                        m_model.SetSelection( pVirtualParameter );
                    }

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
        auto pGraph = m_model.GetGraph();

        //-------------------------------------------------------------------------

        if ( ImGui::Button( "Add New Parameter", ImVec2( -1, 0 ) ) )
        {
            ImGui::OpenPopup( "AddParameterPopup" );
        }

        ImGui::Separator();

        if ( ImGui::BeginPopup( "AddParameterPopup" ) )
        {
            if ( ImGui::MenuItem( "Control Parameter - Bool" ) )
            {
                pGraph->CreateControlParameter( NodeValueType::Bool );
            }

            if ( ImGui::MenuItem( "Control Parameter - ID" ) )
            {
                pGraph->CreateControlParameter( NodeValueType::ID );
            }

            if ( ImGui::MenuItem( "Control Parameter - Int" ) )
            {
                pGraph->CreateControlParameter( NodeValueType::Int );
            }

            if ( ImGui::MenuItem( "Control Parameter - Float" ) )
            {
                pGraph->CreateControlParameter( NodeValueType::Float );
            }

            if ( ImGui::MenuItem( "Control Parameter - Vector" ) )
            {
                pGraph->CreateControlParameter( NodeValueType::Vector );
            }

            if ( ImGui::MenuItem( "Control Parameter - Target" ) )
            {
                pGraph->CreateControlParameter( NodeValueType::Target );
            }

            ImGui::Separator();

            if ( ImGui::MenuItem( "Virtual Parameter - Bool" ) )
            {
                pGraph->CreateVirtualParameter( NodeValueType::Bool );
            }

            if ( ImGui::MenuItem( "Virtual Parameter - ID" ) )
            {
                pGraph->CreateVirtualParameter( NodeValueType::ID );
            }

            if ( ImGui::MenuItem( "Virtual Parameter - Int" ) )
            {
                pGraph->CreateVirtualParameter( NodeValueType::Int );
            }

            if ( ImGui::MenuItem( "Virtual Parameter - Float" ) )
            {
                pGraph->CreateVirtualParameter( NodeValueType::Float );
            }

            if ( ImGui::MenuItem( "Virtual Parameter - Vector" ) )
            {
                pGraph->CreateVirtualParameter( NodeValueType::Vector );
            }

            if ( ImGui::MenuItem( "Virtual Parameter - Target" ) )
            {
                pGraph->CreateVirtualParameter( NodeValueType::Target );
            }

            if ( ImGui::MenuItem( "Virtual Parameter - Bone Mask" ) )
            {
                pGraph->CreateVirtualParameter( NodeValueType::BoneMask );
            }

            ImGui::EndPopup();
        }
    }

    //-------------------------------------------------------------------------

    void GraphControlParameterEditor::StartRename( UUID parameterID )
    {
        KRG_ASSERT( parameterID.IsValid() );
        m_currentOperationParameterID = parameterID;
        m_activeOperation = OperationType::Rename;

        if ( auto pControlParameter = m_model.GetGraph()->FindControlParameter( parameterID ) )
        {
            strncpy_s( m_buffer, pControlParameter->GetDisplayName(), 255 );
        }
        else if ( auto pVirtualParameter = m_model.GetGraph()->FindVirtualParameter( parameterID ) )
        {
            strncpy_s( m_buffer, pVirtualParameter->GetDisplayName(), 255 );
        }
        else
        {
            KRG_UNREACHABLE_CODE();
        }
    }

    void GraphControlParameterEditor::StartDelete( UUID parameterID )
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

                float const dialogWidth = ImGui::GetWindowContentRegionWidth();
                ImGui::SameLine( 0, dialogWidth - 104 );

                if ( ImGui::Button( "Ok", ImVec2( 50, 0 ) ) || nameChangeConfirmed )
                {
                    if ( auto pControlParameter = m_model.GetGraph()->FindControlParameter( m_currentOperationParameterID ) )
                    {
                        m_model.GetGraph()->RenameControlParameter( m_currentOperationParameterID, m_buffer );
                    }
                    else if ( auto pVirtualParameter = m_model.GetGraph()->FindVirtualParameter( m_currentOperationParameterID ) )
                    {
                        m_model.GetGraph()->RenameVirtualParameter( m_currentOperationParameterID, m_buffer );
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

                float const dialogWidth = ImGui::GetWindowContentRegionWidth();
                ImGui::SameLine( 0, dialogWidth - 64 );

                if ( ImGui::Button( "Yes", ImVec2( 30, 0 ) ) )
                {
                    if ( auto pControlParameter = m_model.GetGraph()->FindControlParameter( m_currentOperationParameterID ) )
                    {
                        m_model.GetGraph()->DestroyControlParameter( m_currentOperationParameterID );
                    }
                    else if ( auto pVirtualParameter = m_model.GetGraph()->FindVirtualParameter( m_currentOperationParameterID ) )
                    {
                        m_model.GetGraph()->DestroyVirtualParameter( m_currentOperationParameterID );
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