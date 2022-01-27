#include "AnimationGraphEditor_ControlParameterEditor.h"
#include "EditorGraph/Animation_EditorGraph_Definition.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"
#include "Tools/Core/Helpers/CategoryTree.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    struct BoolParameterState : public GraphControlParameterEditor::ParameterPreviewState
    {
        using ParameterPreviewState::ParameterPreviewState;

        virtual void DrawPreviewEditor( DebugContext* pDebugContext ) override
        {
            GraphNodeIndex const parameterIdx = pDebugContext->GetRuntimeGraphNodeIndex( m_pParameter->GetID() );

            auto value = pDebugContext->m_pGraphComponent->GetControlParameterValue<bool>( parameterIdx );
            if ( ImGui::Checkbox( "##bp", &value ) )
            {
                pDebugContext->m_pGraphComponent->SetControlParameterValue( parameterIdx, value );
            }
        }
    };

    struct IntParameterState : public GraphControlParameterEditor::ParameterPreviewState
    {
        using ParameterPreviewState::ParameterPreviewState;

        virtual void DrawPreviewEditor( DebugContext* pDebugContext ) override
        {
            GraphNodeIndex const parameterIdx = pDebugContext->GetRuntimeGraphNodeIndex( m_pParameter->GetID() );

            auto value = pDebugContext->m_pGraphComponent->GetControlParameterValue<int32>( parameterIdx );
            ImGui::SetNextItemWidth( -1 );
            if ( ImGui::InputInt( "##ip", &value ) )
            {
                pDebugContext->m_pGraphComponent->SetControlParameterValue( parameterIdx, value );
            }
        }
    };

    struct FloatParameterState : public GraphControlParameterEditor::ParameterPreviewState
    {
        using ParameterPreviewState::ParameterPreviewState;

        virtual void DrawPreviewEditor( DebugContext* pDebugContext ) override
        {
            GraphNodeIndex const parameterIdx = pDebugContext->GetRuntimeGraphNodeIndex( m_pParameter->GetID() );
            float value = pDebugContext->m_pGraphComponent->GetControlParameterValue<float>( parameterIdx );

            //-------------------------------------------------------------------------

            auto UpdateLimits = [this, pDebugContext, &value, parameterIdx] ()
            {
                if ( m_min > m_max )
                {
                    m_max = m_min;
                }

                // Clamp the value to the range
                value = Math::Clamp( value, m_min, m_max );
                pDebugContext->m_pGraphComponent->SetControlParameterValue( parameterIdx, value );
            };

            //-------------------------------------------------------------------------

            constexpr float const limitWidth = 40;

            ImGui::SetNextItemWidth( limitWidth );
            if ( ImGui::InputFloat( "##min", &m_min, 0.0f, 0.0f, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue ) )
            {
                UpdateLimits();
            }

            ImGui::SameLine();
            ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail().x - limitWidth - ImGui::GetStyle().ItemSpacing.x );
            if ( ImGui::SliderFloat( "##fp", &value, m_min, m_max ) )
            {
                pDebugContext->m_pGraphComponent->SetControlParameterValue( parameterIdx, value );
            }

            ImGui::SameLine();
            ImGui::SetNextItemWidth( limitWidth );
            if ( ImGui::InputFloat( "##max", &m_max, 0.0f, 0.0f, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue ) )
            {
                UpdateLimits();
            }
        }

    private:

        float m_min = 0.0f;
        float m_max = 1.0f;
    };

    struct VectorParameterState : public GraphControlParameterEditor::ParameterPreviewState
    {
        using ParameterPreviewState::ParameterPreviewState;

        virtual void DrawPreviewEditor( DebugContext* pDebugContext ) override
        {
            GraphNodeIndex const parameterIdx = pDebugContext->GetRuntimeGraphNodeIndex( m_pParameter->GetID() );

            auto value = pDebugContext->m_pGraphComponent->GetControlParameterValue<Vector>( parameterIdx ).ToFloat4();
            ImGui::SetNextItemWidth( -1 );
            if ( ImGui::InputFloat4( "##vp", &value.m_x ) )
            {
                pDebugContext->m_pGraphComponent->SetControlParameterValue( parameterIdx, value );
            }
        }
    };

    struct IDParameterState : public GraphControlParameterEditor::ParameterPreviewState
    {
        using ParameterPreviewState::ParameterPreviewState;

        virtual void DrawPreviewEditor( DebugContext* pDebugContext ) override
        {
            GraphNodeIndex const parameterIdx = pDebugContext->GetRuntimeGraphNodeIndex( m_pParameter->GetID() );
            auto value = pDebugContext->m_pGraphComponent->GetControlParameterValue<StringID>( parameterIdx );
            if ( value.IsValid() )
            {
                strncpy_s( m_buffer, 255, value.c_str(), strlen( value.c_str() ) );
            }
            else
            {
                memset( m_buffer, 0, 255 );
            }

            ImGui::SetNextItemWidth( -1 );
            if ( ImGui::InputText( "##tp", m_buffer, 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
            {
                pDebugContext->m_pGraphComponent->SetControlParameterValue( parameterIdx, StringID( m_buffer ) );
            }
        }

    private:

        char m_buffer[256];
    };

    struct TargetParameterState : public GraphControlParameterEditor::ParameterPreviewState
    {
        using ParameterPreviewState::ParameterPreviewState;

        virtual void DrawPreviewEditor( DebugContext* pDebugContext ) override
        {
            GraphNodeIndex const parameterIdx = pDebugContext->GetRuntimeGraphNodeIndex( m_pParameter->GetID() );
            Target value = pDebugContext->m_pGraphComponent->GetControlParameterValue<Target>( parameterIdx );

            // Reflect actual state
            //-------------------------------------------------------------------------

            m_isSet = value.IsTargetSet();
            m_isBoneTarget = value.IsBoneTarget();

            if ( m_isBoneTarget )
            {
                StringID const boneID = value.GetBoneID();
                if ( boneID.IsValid() )
                {
                    strncpy_s( m_buffer, 255, boneID.c_str(), strlen( boneID.c_str() ) );
                }
                else
                {
                    memset( m_buffer, 0, 255 );
                }

                m_useBoneSpaceOffsets = value.IsUsingBoneSpaceOffsets();
            }

            if ( m_isBoneTarget && value.HasOffsets() )
            {
                m_rotationOffset = value.GetRotationOffset().ToEulerAngles();
                m_translationOffset = value.GetTranslationOffset().ToFloat3();
            }

            if ( m_isSet && !m_isBoneTarget )
            {
                m_transform = value.GetTransform();
            }

            //-------------------------------------------------------------------------

            bool updateValue = false;

            //-------------------------------------------------------------------------

            ImGui::AlignTextToFramePadding();
            ImGui::Text( "Is Set:" );
            ImGui::SameLine();

            if ( ImGui::Checkbox( "##IsSet", &m_isSet ) )
            {
                updateValue = true;
            }

            //-------------------------------------------------------------------------

            if ( m_isSet )
            {
                ImGui::SameLine( 0, 8 );
                ImGui::Text( "Is Bone:" );
                ImGui::SameLine();

                if ( ImGui::Checkbox( "##IsBone", &m_isBoneTarget ) )
                {
                    updateValue = true;
                }

                if ( m_isBoneTarget )
                {
                    if ( ImGui::BeginTable( "BoneSettings", 2 ) )
                    {
                        ImGui::TableSetupColumn( "Label", ImGuiTableColumnFlags_WidthFixed, 45 );
                        ImGui::TableSetupColumn( "Editor", ImGuiTableColumnFlags_WidthStretch );

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::AlignTextToFramePadding();
                        {
                            ImGuiX::ScopedFont sf( ImGuiX::Font::Tiny );
                            ImGui::Text( "Bone ID" );
                        }

                        ImGui::TableNextColumn();
                        if ( ImGui::InputText( "##tp", m_buffer, 255, ImGuiInputTextFlags_EnterReturnsTrue ) )
                        {
                            updateValue = true;
                        }

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::AlignTextToFramePadding();
                        {
                            ImGuiX::ScopedFont sf( ImGuiX::Font::Tiny );
                            ImGui::Text( "Offset R" );
                        }

                        ImGui::TableNextColumn();
                        Float3 degrees = m_rotationOffset.GetAsDegrees();
                        if ( ImGuiX::InputFloat3( "##RotationOffset", degrees ) )
                        {
                            m_rotationOffset = EulerAngles( degrees );
                            updateValue = true;
                        }

                        ImGui::TableNextRow();
                        ImGui::TableNextColumn();
                        ImGui::AlignTextToFramePadding();

                        {
                            ImGuiX::ScopedFont sf( ImGuiX::Font::Tiny );
                            ImGui::Text( "Offset T" );
                        }

                        ImGui::TableNextColumn();
                        if ( ImGuiX::InputFloat3( "##TranslationOffset", m_translationOffset ) )
                        {
                            updateValue = true;
                        }

                        ImGui::EndTable();
                    }
                }
                else
                {
                    if ( ImGuiX::InputTransform( "##Transform", m_transform ) )
                    {
                        updateValue = true;
                    }
                }
            }

            //-------------------------------------------------------------------------

            if ( updateValue )
            {
                if ( m_isSet )
                {
                    if ( m_isBoneTarget )
                    {
                        value = Target( StringID( m_buffer ) );

                        if ( !Quaternion( m_rotationOffset ).IsIdentity() || !Vector( m_translationOffset ).IsNearZero3() )
                        {
                            value.SetOffsets( Quaternion( m_rotationOffset ), m_translationOffset, m_useBoneSpaceOffsets );
                        }
                    }
                    else
                    {
                        value = Target( m_transform );
                    }
                }
                else
                {
                    value = Target();
                }

                pDebugContext->m_pGraphComponent->SetControlParameterValue( parameterIdx, value );
            }
        }

    private:

        bool            m_isSet = false;
        bool            m_isBoneTarget = false;
        bool            m_useBoneSpaceOffsets = false;
        char            m_buffer[256] = {0};
        EulerAngles     m_rotationOffset;
        Float3          m_translationOffset = Float3::Zero;
        Transform       m_transform = Transform::Identity;
    };

    //-------------------------------------------------------------------------

    GraphControlParameterEditor::GraphControlParameterEditor( EditorGraphDefinition* pGraphDefinition )
        : m_pGraphDefinition( pGraphDefinition )
    {}

    GraphControlParameterEditor::~GraphControlParameterEditor()
    {
        DestroyPreviewStates();
    }

    bool GraphControlParameterEditor::UpdateAndDraw( UpdateContext const& context, DebugContext* pDebugContext, ImGuiWindowClass* pWindowClass, char const* pWindowName )
    {
        m_pVirtualParamaterToEdit = nullptr;

        int32 windowFlags = 0;
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 4 ) );
        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( pWindowName, nullptr, windowFlags ) )
        {

            bool const isPreviewing = pDebugContext != nullptr;
            if ( isPreviewing )
            {
                if ( m_parameterPreviewStates.empty() )
                {
                    CreatePreviewStates();
                }

                DrawParameterPreviewControls( pDebugContext );
            }
            else
            {
                if ( !m_parameterPreviewStates.empty() )
                {
                    DestroyPreviewStates();
                }

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

        ImGui::PopStyleColor( 3 );
    }

    void GraphControlParameterEditor::DrawParameterPreviewControls( DebugContext* pDebugContext )
    {
        KRG_ASSERT( pDebugContext != nullptr );

        //-------------------------------------------------------------------------

        CategoryTree<ParameterPreviewState*> parameterTree;
        for ( auto pPreviewState : m_parameterPreviewStates )
        {
            auto pControlParameter = pPreviewState->m_pParameter;
            parameterTree.AddItem( pControlParameter->GetParameterCategory(), pControlParameter->GetDisplayName(), pPreviewState );
        }

        //-------------------------------------------------------------------------

        auto DrawCategoryRow = [] ( String const& categoryName )
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::AlignTextToFramePadding();
            ImGui::Text( categoryName.c_str() );
        };

        auto DrawControlParameterEditorRow = [this, pDebugContext] ( ParameterPreviewState* pPreviewState )
        {
            auto pControlParameter = pPreviewState->m_pParameter;
            GraphNodeIndex const parameterIdx = pDebugContext->GetRuntimeGraphNodeIndex( pControlParameter->GetID() );

            ImGui::PushID( pControlParameter );
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex( 0 );
            ImGui::AlignTextToFramePadding();
            ImGui::PushStyleColor( ImGuiCol_Text, (ImVec4) pControlParameter->GetNodeTitleColor() );
            ImGui::Indent();
            ImGui::Text( pControlParameter->GetDisplayName() );
            ImGui::Unindent();
            ImGui::PopStyleColor();

            ImGui::TableSetColumnIndex( 1 );
            pPreviewState->DrawPreviewEditor( pDebugContext );
            ImGui::PopID();
        };

        //-------------------------------------------------------------------------

        if ( ImGui::BeginTable( "Parameter Preview", 2, ImGuiTableFlags_Resizable, ImVec2( -1, -1 ) ) )
        {
            ImGui::TableSetupColumn( "Name", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableSetupColumn( "Editor", ImGuiTableColumnFlags_WidthStretch );

            //-------------------------------------------------------------------------

            DrawCategoryRow( "General" );

            for ( auto const& item : parameterTree.GetRootCategory().m_items )
            {
                DrawControlParameterEditorRow( item.m_data );
            }

            //-------------------------------------------------------------------------

            for ( auto const& category : parameterTree.GetRootCategory().m_childCategories )
            {
                DrawCategoryRow( category.m_name );

                for ( auto const& item : category.m_items )
                {
                    DrawControlParameterEditorRow( item.m_data );
                }
            }

            ImGui::EndTable();
        }
    }

    //-------------------------------------------------------------------------

    void GraphControlParameterEditor::CreatePreviewStates()
    {
        int32 const numParameters = m_pGraphDefinition->GetNumControlParameters();
        for ( int32 i = 0; i < numParameters; i++ )
        {
            auto pControlParameter = m_pGraphDefinition->GetControlParameters()[i];
            switch ( pControlParameter->GetValueType() )
            {
                case GraphValueType::Bool:
                {
                    m_parameterPreviewStates.emplace_back( KRG::New<BoolParameterState>( pControlParameter ) );
                }
                break;

                case GraphValueType::Int:
                {
                    m_parameterPreviewStates.emplace_back( KRG::New<IntParameterState>( pControlParameter ) );
                }
                break;

                case GraphValueType::Float:
                {
                    m_parameterPreviewStates.emplace_back( KRG::New<FloatParameterState>( pControlParameter ) );
                }
                break;

                case GraphValueType::Vector:
                {
                    m_parameterPreviewStates.emplace_back( KRG::New<VectorParameterState>( pControlParameter ) );
                }
                break;

                case GraphValueType::ID:
                {
                    m_parameterPreviewStates.emplace_back( KRG::New<IDParameterState>( pControlParameter ) );
                }
                break;

                case GraphValueType::Target:
                {
                    m_parameterPreviewStates.emplace_back( KRG::New<TargetParameterState>( pControlParameter ) );
                }
                break;
            }
        }
    }

    void GraphControlParameterEditor::DestroyPreviewStates()
    {
        for ( auto& pPreviewState : m_parameterPreviewStates )
        {
            KRG::Delete( pPreviewState );
        }

        m_parameterPreviewStates.clear();
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