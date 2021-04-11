#include "PropertyGrid.h"
#include "PropertyEditors.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    PropertyGrid::PropertyGrid( TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataPath )
        : m_typeRegistry( typeRegistry )
        , m_sourceDataPath( sourceDataPath )
    {
        m_pTypeInstanceModel = KRG::New<TypeInstanceModel>( m_typeRegistry, m_typeRegistry.GetTypeInfo( TypeID( "KRG::TestComponent" ) ) );
    }

    PropertyGrid::~PropertyGrid()
    {
        KRG::Delete( m_pTypeInstanceModel );
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::SetTypeToEdit( TypeInstanceModel* pTypeInstance )
    {
        //m_pTypeInstance = pTypeInstance;
        m_isDirty = false;
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::DrawGrid()
    {
        if ( m_pTypeInstanceModel == nullptr )
        {
            return;
        }

        //-------------------------------------------------------------------------

        ImGuiTableFlags const flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersH | ImGuiTableFlags_Resizable;
        ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 4, 4 ) );
        if ( ImGui::BeginTable( "PropertyGrid", 3, flags ) )
        {
            ImGui::TableSetupColumn( "Property", ImGuiTableColumnFlags_NoHide );
            ImGui::TableSetupColumn( "##EditorWidget", ImGuiTableColumnFlags_NoHide );
            ImGui::TableSetupColumn( "##ExtraControls", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 20 );
            ImGui::TableHeadersRow();

            //-------------------------------------------------------------------------

            for ( auto& propertyModel : m_pTypeInstanceModel->GetProperties() )
            {
                ImGui::TableNextRow();
                DrawPropertyRow( propertyModel );
            }
            
            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
    }

    void PropertyGrid::DrawPropertyRow( PropertyInstanceModel* pPropertyModel )
    {
        KRG_ASSERT( pPropertyModel != nullptr );

        if ( pPropertyModel->IsArray() )
        {
            DrawArrayPropertyRow( pPropertyModel );
        }
        else if ( pPropertyModel->IsStructure() )
        {
            DrawStructPropertyRow( pPropertyModel );
        }
        else // Core Types
        {
            DrawCoreTypePropertyRow( pPropertyModel );
        }
    }

    void PropertyGrid::DrawArrayPropertyRow( PropertyInstanceModel* pPropertyModel )
    {
        KRG_ASSERT( pPropertyModel != nullptr );
        KRG_ASSERT( pPropertyModel->IsArray() );

        // Name
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();

        bool showContents = false;
        if ( ImGui::TreeNodeEx( pPropertyModel->GetID().c_str(), ImGuiTreeNodeFlags_None ) )
        {
            showContents = true;
        }

        // Editor
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();
        if ( pPropertyModel->IsDynamicArray() )
        {
            float const cellContentWidth = ImGui::GetContentRegionAvail().x;
            float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
            float const buttonAreaWidth = 21;
            float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;
            float const buttonStartPosX = textAreaWidth + itemSpacing;

            ImGui::AlignTextToFramePadding();
            ImGui::TextColored( Colors::Gray.ToFloat4(), "%d Elements - %s", pPropertyModel->GetNumArrayElements(), pPropertyModel->GetFriendlyTypeName() );
            float const actualTextWidth = ImGui::GetItemRectSize().x;

            ImGui::SameLine( 0, textAreaWidth - actualTextWidth + itemSpacing );
            if ( !pPropertyModel->IsDefaultValue() )
            {
                ImGui::PushID( pPropertyModel );
                if ( ImGuiX::ButtonColored( KRG_ICON_UNDO, Colors::LightGray ) )
                {
                    pPropertyModel->ResetToDefaultValue();
                    m_isDirty = true;
                }
                ImGui::PopID();
            }
        }
        else
        {
            ImGui::TextColored( Colors::Gray.ToFloat4(), "%d Elements - %s", pPropertyModel->GetNumArrayElements(), pPropertyModel->GetFriendlyTypeName() );
        }

        // Extra Controls
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();
        DrawRowControls( pPropertyModel );

        // Array Elements
        //-------------------------------------------------------------------------

        if ( showContents )
        {
            // Draw child elements
            for ( auto& childPropertyModel : pPropertyModel->GetProperties() )
            {
                ImGui::TableNextRow();
                DrawPropertyRow( childPropertyModel );
            }

            ImGui::TreePop();
        }
    }

    void PropertyGrid::DrawStructPropertyRow( PropertyInstanceModel* pPropertyModel )
    {
        KRG_ASSERT( pPropertyModel != nullptr );
        KRG_ASSERT( pPropertyModel->IsStructure() );

        // Name
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();

        bool showContents = false;
        if ( ImGui::TreeNodeEx( pPropertyModel->GetFriendlyName(), ImGuiTreeNodeFlags_SpanFullWidth ) )
        {
            showContents = true;
        }

        // Editor
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();
        ImGui::TextColored( Colors::Gray.ToFloat4(), pPropertyModel->GetFriendlyTypeName() );

        // Controls
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();
        DrawRowControls( pPropertyModel );

        // Child Properties
        //-------------------------------------------------------------------------

        if ( showContents )
        {
            // Draw child elements
            for ( auto& childPropertyModel : pPropertyModel->GetProperties() )
            {
                ImGui::TableNextRow();
                DrawPropertyRow( childPropertyModel );
            }

            ImGui::TreePop();
        }
    }

    void PropertyGrid::DrawCoreTypePropertyRow( PropertyInstanceModel* pPropertyModel )
    {
        KRG_ASSERT( pPropertyModel != nullptr );
        KRG_ASSERT( !pPropertyModel->IsStructure() && !pPropertyModel->IsArray() );

        // Name
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();
        ImGui::Text( pPropertyModel->GetFriendlyName() );

        // Editor
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();
        PG::Context ctx( m_typeRegistry, m_sourceDataPath );
        m_isDirty = PG::CreatePropertyEditor( ctx, *pPropertyModel );

        // Controls
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();
        DrawRowControls( pPropertyModel );
    }

    void PropertyGrid::DrawRowControls( PropertyInstanceModel* pPropertyModel )
    {
        KRG_ASSERT( pPropertyModel != nullptr );

        if ( pPropertyModel->IsDynamicArray() )
        {
            ImGui::PushID( pPropertyModel );
            if ( ImGuiX::ButtonColored( KRG_ICON_PLUS, Colors::LightGreen ) )
            {
                pPropertyModel->AddArrayElement();
                m_isDirty = true;
            }
            ImGui::PopID();
        }
        else if( pPropertyModel->IsArrayElement() )
        {
            auto pParentPropertyModel = pPropertyModel->GetParentProperty();
            if ( pParentPropertyModel->IsDynamicArray() )
            {
                ImGui::PushID( pPropertyModel );
                if ( ImGuiX::ButtonColored( KRG_ICON_MINUS, Colors::PaleVioletRed ) )
                {
                    pParentPropertyModel->RemoveArrayElement( pPropertyModel->GetArrayElementIndex() );
                    m_isDirty = true;
                }
                ImGui::PopID();
            }
            else // Static array element (only allow reset value)
            {
                if ( !pPropertyModel->IsDefaultValue() )
                {
                    ImGui::PushID( pPropertyModel );
                    if ( ImGuiX::ButtonColored( KRG_ICON_UNDO, Colors::LightGray ) )
                    {
                        pPropertyModel->ResetToDefaultValue();
                        m_isDirty = true;
                    }
                    ImGui::PopID();
                }
            }
        }
        else if ( pPropertyModel->IsStructure() )
        {
            // Do Nothing
        }
        else // Regular type
        {
            if ( !pPropertyModel->IsDefaultValue() )
            {
                ImGui::PushID( pPropertyModel );
                if ( ImGuiX::ButtonColored( KRG_ICON_UNDO, Colors::LightGray ) )
                {
                    pPropertyModel->ResetToDefaultValue();
                    m_isDirty = true;
                }
                ImGui::PopID();
            }
        }
    }
}