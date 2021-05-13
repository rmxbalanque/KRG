#include "PropertyGrid.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "Applications/Editor/Editor/EditorModel.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    PropertyGrid::PropertyGrid( EditorModel* pModel )
        : m_pModel( pModel )
        , m_context( pModel->GetTypeRegistry(), pModel->GetSourceDataDirectory() )
    {
        KRG_ASSERT( pModel );

        m_context.m_preChangeDelegate = TFunction<void()>( [this] () { PreChange(); } );
        m_context.m_postChangeDelegate = TFunction<void()>( [this] () { PostChange(); } );
    }

    void PropertyGrid::PreChange()
    {
        // TODO: undo/redo support
    }

    void PropertyGrid::PostChange()
    {
        // TODO: undo/redo support
        m_isDirty = true;
    }

    void PropertyGrid::DrawGrid()
    {
        if ( m_pTypeInstance == nullptr )
        {
            ImGui::Text( "Nothing To Edit" );
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

            for ( auto const& propertyInfo : m_pTypeInfo->m_properties )
            {
                ImGui::TableNextRow();
                DrawPropertyRow( *m_pTypeInfo, m_pTypeInstance, propertyInfo, m_pTypeInstance + propertyInfo.m_offset );
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
    }

    void PropertyGrid::DrawPropertyRow( TypeInfo const& typeInfo, Byte* pTypeInstance, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        if ( propertyInfo.IsArrayProperty() )
        {
            DrawArrayPropertyRow( typeInfo, pTypeInstance, propertyInfo, pPropertyInstance );
        }
        else
        {
            DrawValuePropertyRow( typeInfo, pTypeInstance, propertyInfo, pPropertyInstance );
        }
    }

    void PropertyGrid::DrawValuePropertyRow( TypeInfo const& typeInfo, Byte* pTypeInstance, PropertyInfo const& propertyInfo, Byte* pPropertyInstance, int32 arrayIdx )
    {
        //-------------------------------------------------------------------------
        // Name
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();

        InlineString<255> propertyName;
        if ( arrayIdx != InvalidIndex )
        {
            propertyName.sprintf( "%d", arrayIdx );
        }
        else
        {
            propertyName = propertyInfo.m_ID.c_str();
        }

        bool showContents = false;

        if ( propertyInfo.IsStructureProperty() )
        {
            if ( ImGui::TreeNodeEx( propertyName.c_str(), ImGuiTreeNodeFlags_SpanFullWidth ) )
            {
                showContents = true;
            }
        }
        else
        {
            ImGui::Text( propertyName.c_str() );
        }

        //-------------------------------------------------------------------------

        auto pActualPropertyInstance = propertyInfo.IsArrayProperty() ? typeInfo.m_pTypeHelper->GetArrayElementDataPtr( pTypeInstance, propertyInfo.m_ID, arrayIdx ) : pPropertyInstance;

        //-------------------------------------------------------------------------
        // Editor
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();
        
        if ( propertyInfo.IsStructureProperty() )
        {
            ImGui::TextColored( Colors::Gray.ToFloat4(), propertyInfo.m_typeID.c_str() );
        }
        else // Create property editor
        {
            PG::CreatePropertyEditor( m_context, propertyInfo, pActualPropertyInstance );
        }

        //-------------------------------------------------------------------------
        // Controls
        //-------------------------------------------------------------------------

        enum class Command { None, ResetToDefault, RemoveElement };
        Command command = Command::None;

        ImGui::TableNextColumn();

        ImGui::PushID( pActualPropertyInstance );
        if ( propertyInfo.IsDynamicArrayProperty() )
        {
            KRG_ASSERT( arrayIdx != InvalidIndex );
            auto const pArrayElementInstance = typeInfo.m_pTypeHelper->GetArrayElementDataPtr( pTypeInstance, propertyInfo.m_ID, arrayIdx );
            if ( ImGuiX::ButtonColored( Colors::PaleVioletRed.ToFloat4(), KRG_ICON_MINUS ) )
            {
                command = Command::RemoveElement;
            }
        }
        else if ( !typeInfo.m_pTypeHelper->IsPropertyValueSetToDefault( pTypeInstance, propertyInfo.m_ID, arrayIdx ) )
        {
            if ( ImGuiX::ButtonColored( Colors::LightGray.ToFloat4(), KRG_ICON_UNDO ) )
            {
                command = Command::ResetToDefault;
            }
        }
        ImGui::PopID();

        //-------------------------------------------------------------------------
        // Child Properties
        //-------------------------------------------------------------------------
        // Only relevant for structure properties

        if ( showContents )
        {
            KRG_ASSERT( propertyInfo.IsStructureProperty() );

            TypeInfo const* pChildTypeInfo = m_pModel->GetTypeRegistry().GetTypeInfo( propertyInfo.m_typeID );
            KRG_ASSERT( pChildTypeInfo != nullptr );
            Byte* pChildTypeInstance = pActualPropertyInstance;

            for ( auto const& childPropertyInfo : pChildTypeInfo->m_properties )
            {
                ImGui::TableNextRow();
                DrawPropertyRow( *pChildTypeInfo, pChildTypeInstance, childPropertyInfo, pChildTypeInstance + childPropertyInfo.m_offset );
            }

            ImGui::TreePop();
        }

        //-------------------------------------------------------------------------
        // Handle control requests
        //-------------------------------------------------------------------------
        // This needs to be done after we have finished drawing the UI

        switch ( command ) 
        {
            case Command::RemoveElement:
            {
                PreChange();
                typeInfo.m_pTypeHelper->RemoveArrayElement( pTypeInstance, propertyInfo.m_ID, arrayIdx );
                PostChange();
            }
            break;

            case Command::ResetToDefault:
            {
                PreChange();
                typeInfo.m_pTypeHelper->ResetToDefault( pTypeInstance, propertyInfo.m_ID );
                PostChange();
            }
            break;
        }
    }

    void PropertyGrid::DrawArrayPropertyRow( TypeInfo const& typeInfo, Byte* pTypeInstance, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        KRG_ASSERT( propertyInfo.IsArrayProperty() );

        ImGui::PushID( pPropertyInstance );

        // Name
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();

        bool showContents = false;
        if ( ImGui::TreeNodeEx( propertyInfo.m_ID.c_str(), ImGuiTreeNodeFlags_None ) )
        {
            showContents = true;
        }

        // Editor
        //-------------------------------------------------------------------------

        size_t const arraySize = typeInfo.m_pTypeHelper->GetArraySize( pTypeInstance, propertyInfo.m_ID );

        ImGui::TableNextColumn();
        if ( propertyInfo.IsDynamicArrayProperty() )
        {
            float const cellContentWidth = ImGui::GetContentRegionAvail().x;
            float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
            float const buttonAreaWidth = 21;
            float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;
            float const buttonStartPosX = textAreaWidth + itemSpacing;

            ImGui::AlignTextToFramePadding();
            ImGui::TextColored( Colors::Gray.ToFloat4(), "%d Elements - %s", arraySize, propertyInfo.m_typeID.c_str() );
            float const actualTextWidth = ImGui::GetItemRectSize().x;

            ImGui::SameLine( 0, textAreaWidth - actualTextWidth + itemSpacing );
            if ( !typeInfo.m_pTypeHelper->IsPropertyValueSetToDefault( pTypeInstance, propertyInfo.m_ID ) )
            {
                if ( ImGuiX::ButtonColored( Colors::LightGray.ToFloat4(), KRG_ICON_UNDO ) )
                {
                    PreChange();
                    typeInfo.m_pTypeHelper->ResetToDefault( pTypeInstance, propertyInfo.m_ID );
                    PostChange();
                }
            }
        }
        else
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextColored( Colors::Gray.ToFloat4(), "%d Elements - %s", arraySize, propertyInfo.m_typeID.c_str() );
        }

        // Extra Controls
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();
        if ( propertyInfo.IsDynamicArrayProperty() )
        {
            if ( ImGuiX::ButtonColored( Colors::LightGreen.ToFloat4(), KRG_ICON_PLUS ) )
            {
                PreChange();
                typeInfo.m_pTypeHelper->AddArrayElement( pTypeInstance, propertyInfo.m_ID );
                PostChange();
            }
        }

        // Array Elements
        //-------------------------------------------------------------------------

        if ( showContents )
        {
            // We need to ask for the array size each iteration since we may destroy a row as part of drawing it
            for ( auto i = 0u; i < typeInfo.m_pTypeHelper->GetArraySize( pTypeInstance, propertyInfo.m_ID ); i++ )
            {
                DrawValuePropertyRow( typeInfo, pTypeInstance, propertyInfo, pPropertyInstance, i );
            }

            ImGui::TreePop();
        }

        ImGui::PopID();
    }
}