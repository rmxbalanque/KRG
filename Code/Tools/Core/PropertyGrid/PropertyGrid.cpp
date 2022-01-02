#include "PropertyGrid.h"
#include "PropertyGridEditors.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/PropertyInfo.h"

//-------------------------------------------------------------------------

using namespace KRG::TypeSystem;

//-------------------------------------------------------------------------

namespace KRG
{
    struct [[nodiscard]] ScopedChangeNotifier
    {
        ScopedChangeNotifier( PropertyGrid* pGrid, TypeSystem::PropertyInfo const* pPropertyInfo, PropertyEditInfo::Action action = PropertyEditInfo::Action::Edit )
            : m_pGrid( pGrid )
        {
            m_eventInfo.m_pEditedTypeInstance = m_pGrid->m_pTypeInstance;
            m_eventInfo.m_pPropertyInfo = pPropertyInfo;
            m_eventInfo.m_action = action;
            m_pGrid->m_preEditEvent.Execute( m_eventInfo );
        }

        ~ScopedChangeNotifier()
        {
            m_pGrid->m_postEditEvent.Execute( m_eventInfo );
            m_pGrid->m_isDirty = true;
        }

        PropertyGrid*                       m_pGrid = nullptr;
        PropertyEditInfo                    m_eventInfo;
    };

    namespace ImGuiX
    {
        static bool FlatButtonColored( ImVec4 const& foregroundColor, char const* label, ImVec2 const& size = ImVec2( 0, 0 ) )
        {
            ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 3, 3 ) );
            ImGui::PushStyleColor( ImGuiCol_Button, 0 );
            ImGui::PushStyleColor( ImGuiCol_Text, foregroundColor );
            bool const result = ImGui::Button( label, size );
            ImGui::PopStyleColor( 2 );
            ImGui::PopStyleVar( 1 );

            return result;
        }
    }

    //-------------------------------------------------------------------------

    PropertyGrid::PropertyGrid( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDatabase )
        : m_typeRegistry( typeRegistry )
        , m_resourcePicker( resourceDatabase )
    {}

    PropertyGrid::~PropertyGrid()
    {
        DestroyPropertyEditors();
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::SetTypeToEdit( IRegisteredType* pTypeInstance )
    {
        KRG_ASSERT( pTypeInstance != nullptr );

        if ( pTypeInstance != m_pTypeInstance )
        {
            m_pTypeInfo = pTypeInstance->GetTypeInfo();
            m_pTypeInstance = pTypeInstance;
        }

        // Always reset editors and dirty flag
        DestroyPropertyEditors();
        m_isDirty = false;
    }

    void PropertyGrid::SetTypeToEdit( nullptr_t )
    {
        m_pTypeInfo = nullptr;
        m_pTypeInstance = nullptr;
        m_isDirty = false;

        DestroyPropertyEditors();
    }

    //-------------------------------------------------------------------------

    PropertyEditor* PropertyGrid::GetPropertyEditor( PropertyInfo const& propertyInfo, Byte* pActualPropertyInstance )
    {
        PropertyEditor* pPropertyEditor = nullptr;

        auto foundIter = m_propertyEditors.find( pActualPropertyInstance );
        if ( foundIter != m_propertyEditors.end() )
        {
            pPropertyEditor = foundIter->second;
        }
        else // Create new editor instance
        {
            pPropertyEditor = CreatePropertyEditor( m_typeRegistry, m_resourcePicker, propertyInfo, pActualPropertyInstance );
            m_propertyEditors[pActualPropertyInstance] = pPropertyEditor;
        }

        return pPropertyEditor;
    }

    void PropertyGrid::DestroyPropertyEditors()
    {
        for ( auto& pair : m_propertyEditors )
        {
            KRG::Delete( pair.second );
        }

        m_propertyEditors.clear();
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::DrawGrid()
    {
        if ( m_pTypeInstance == nullptr )
        {
            ImGui::Text( "Nothing To Edit" );
            return;
        }

        //-------------------------------------------------------------------------

        ImGuiTableFlags const flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersH | ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingFixedFit;
        ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 2, 4 ) );
        if ( ImGui::BeginTable( "PropertyGrid", 3, flags ) )
        {
            ImGui::TableSetupColumn( "Property", ImGuiTableColumnFlags_WidthFixed, 100 );
            ImGui::TableSetupColumn( "##EditorWidget", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableSetupColumn( "##ExtraControls", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 19 );
            ImGui::TableHeadersRow();

            //-------------------------------------------------------------------------

            for ( auto const& propertyInfo : m_pTypeInfo->m_properties )
            {
                if ( !propertyInfo.IsExposedProperty() )
                {
                    continue;
                }

                ImGui::TableNextRow();
                DrawPropertyRow( *m_pTypeInfo, m_pTypeInstance, propertyInfo, reinterpret_cast<Byte*>( m_pTypeInstance ) + propertyInfo.m_offset );
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();
    }

    void PropertyGrid::DrawPropertyRow( TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
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

    void PropertyGrid::DrawValuePropertyRow( TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, PropertyInfo const& propertyInfo, Byte* pPropertyInstance, int32 arrayIdx )
    {
        //-------------------------------------------------------------------------
        // Name
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();

        InlineString propertyName;
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

        PropertyEditor* pPropertyEditor = GetPropertyEditor( propertyInfo, pActualPropertyInstance );

        ImGui::TableNextColumn();
        
        if ( propertyInfo.IsStructureProperty() )
        {
            ImGui::TextColored( Colors::Gray.ToFloat4(), propertyInfo.m_typeID.c_str() );
        }
        else // Create property editor
        {
            if ( pPropertyEditor != nullptr )
            {
                if ( pPropertyEditor->UpdateAndDraw() )
                {
                    ScopedChangeNotifier cn( this, &propertyInfo );
                    pPropertyEditor->UpdateInstanceValue();
                }
            }
            else
            {
                ImGui::Text( "No Editor Found!" );
            }
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
            if ( ImGuiX::FlatButtonColored( Colors::PaleVioletRed.ToFloat4(), KRG_ICON_MINUS ) )
            {
                command = Command::RemoveElement;
            }
        }
        else if ( !typeInfo.m_pTypeHelper->IsPropertyValueSetToDefault( pTypeInstance, propertyInfo.m_ID, arrayIdx ) )
        {
            if ( ImGuiX::FlatButtonColored( Colors::LightGray.ToFloat4(), KRG_ICON_UNDO ) )
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

            TypeInfo const* pChildTypeInfo = m_typeRegistry.GetTypeInfo( propertyInfo.m_typeID );
            KRG_ASSERT( pChildTypeInfo != nullptr );
            Byte* pChildTypeInstance = pActualPropertyInstance;

            for ( auto const& childPropertyInfo : pChildTypeInfo->m_properties )
            {
                ImGui::TableNextRow();
                DrawPropertyRow( *pChildTypeInfo, reinterpret_cast<IRegisteredType*>( pChildTypeInstance ), childPropertyInfo, pChildTypeInstance + childPropertyInfo.m_offset );
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
                ScopedChangeNotifier cn( this, &propertyInfo, PropertyEditInfo::Action::RemoveArrayElement );
                typeInfo.m_pTypeHelper->RemoveArrayElement( pTypeInstance, propertyInfo.m_ID, arrayIdx );
            }
            break;

            case Command::ResetToDefault:
            {
                ScopedChangeNotifier cn( this, &propertyInfo );
                typeInfo.m_pTypeHelper->ResetToDefault( pTypeInstance, propertyInfo.m_ID );

                if ( pPropertyEditor != nullptr )
                {
                    pPropertyEditor->ResetWorkingCopy();
                }
            }
            break;
        }
    }

    void PropertyGrid::DrawArrayPropertyRow( TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        KRG_ASSERT( propertyInfo.IsArrayProperty() );

        ImGui::PushID( pPropertyInstance );

        // Name
        //-------------------------------------------------------------------------

        ImGui::TableNextColumn();
        ImGui::AlignTextToFramePadding();

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
                if ( ImGuiX::FlatButtonColored( Colors::LightGray.ToFloat4(), KRG_ICON_UNDO ) )
                {
                    ScopedChangeNotifier cn( this, &propertyInfo );
                    typeInfo.m_pTypeHelper->ResetToDefault( pTypeInstance, propertyInfo.m_ID );
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
            if ( ImGuiX::FlatButtonColored( Colors::LightGreen.ToFloat4(), KRG_ICON_PLUS ) )
            {
                ScopedChangeNotifier cn( this, &propertyInfo, PropertyEditInfo::Action::AddArrayElement );
                typeInfo.m_pTypeHelper->AddArrayElement( pTypeInstance, propertyInfo.m_ID );
                ImGui::GetStateStorage()->SetInt( ImGui::GetID( propertyInfo.m_ID.c_str() ), 1 );
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