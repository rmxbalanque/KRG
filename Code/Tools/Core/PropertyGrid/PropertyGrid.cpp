#include "PropertyGrid.h"
#include "Tools/Core/Resource/DataFilePicker.h"
#include "Tools/Core/Widgets/NumericEditors.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/PropertyInfo.h"

//-------------------------------------------------------------------------

using namespace KRG::TypeSystem;

//-------------------------------------------------------------------------

namespace KRG
{
    constexpr static char const* const g_emptyLabel = "##";

    struct [[nodiscard]] ScopedChangeNotifier
    {
        ScopedChangeNotifier( PropertyGrid* pGrid, TypeSystem::PropertyInfo const* pPropertyInfo, PropertyEditInfo::Action action = PropertyEditInfo::Action::Edit )
            : m_pGrid( pGrid )
        {
            m_eventInfo.m_pEditedTypeInstance = m_pGrid->m_pTypeInstance;
            m_eventInfo.m_pPropertyInfo = pPropertyInfo;
            m_eventInfo.m_action = action;

            if ( m_pGrid->m_preEditEvent.HasBoundUser() )
            {
                m_pGrid->m_preEditEvent.Execute( m_eventInfo );
            }
        }

        ~ScopedChangeNotifier()
        {
            if ( m_pGrid->m_postEditEvent.HasBoundUser() )
            {
                m_pGrid->m_postEditEvent.Execute( m_eventInfo );
            }
            m_pGrid->m_isDirty = true;
        }

        PropertyGrid*                       m_pGrid = nullptr;
        PropertyEditInfo                    m_eventInfo;
    };

    //-------------------------------------------------------------------------

    PropertyGrid::PropertyGrid( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& rawResourceDirectoryPath )
        : m_typeRegistry( typeRegistry )
        , m_rawResourceDirectoryPath( rawResourceDirectoryPath )
    {}

    PropertyGrid::~PropertyGrid()
    {
        ClearCache();
    }

    //-------------------------------------------------------------------------

    // Set the type instance to edit, will reset dirty status
    void PropertyGrid::SetTypeToEdit( IRegisteredType* pTypeInstance )
    {
        if ( pTypeInstance == m_pTypeInstance )
        {
            return;
        }

        KRG_ASSERT( pTypeInstance != nullptr );
        m_pTypeInfo = pTypeInstance->GetTypeInfo();
        m_pTypeInstance = pTypeInstance;
        ClearCache();
        m_isDirty = false;
    }

    // Set the type instance to edit, will reset dirty status
    void PropertyGrid::SetTypeToEdit( nullptr_t )
    {
        m_pTypeInfo = nullptr;
        m_pTypeInstance = nullptr;
        ClearCache();
        m_isDirty = false;
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
            CreatePropertyEditor( propertyInfo, pActualPropertyInstance );
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

                CoreTypes const coreType = GetCoreType( propertyInfo.m_typeID );
                if ( coreType == CoreTypes::Matrix || coreType == CoreTypes::Transform )
                {
                    ResetCachedRotation( propertyInfo, pPropertyInstance );
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
            if ( ImGuiX::ButtonColored( Colors::LightGreen.ToFloat4(), KRG_ICON_PLUS ) )
            {
                ScopedChangeNotifier cn( this, &propertyInfo, PropertyEditInfo::Action::AddArrayElement );
                typeInfo.m_pTypeHelper->AddArrayElement( pTypeInstance, propertyInfo.m_ID );
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

    void PropertyGrid::CreatePropertyEditor( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        ImGui::PushID( pPropertyInstance );

        if ( propertyInfo.IsEnumProperty() )
        {
            CreateEditorEnum( propertyInfo, pPropertyInstance );
        }
        else
        {
            CoreTypes const coreType = GetCoreType( propertyInfo.m_typeID );
            switch ( coreType )
            {
                case CoreTypes::Bool:
                {
                    CreateEditorBool( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Int8:
                {
                    CreateEditorInt8( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Int16:
                {
                    CreateEditorInt16( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Int32:
                {
                    CreateEditorInt32( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Int64:
                {
                    CreateEditorInt64( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Uint8:
                {
                    CreateEditorUInt8( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Uint16:
                {
                    CreateEditorUInt16( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Uint32:
                {
                    CreateEditorUInt32( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Uint64:
                {
                    CreateEditorUInt64( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Float:
                {
                    CreateEditorFloat( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Double:
                {
                    CreateEditorDouble( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::UUID:
                {
                    CreateEditorUUID( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::StringID:
                {
                    CreateEditorStringID( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::String:
                {
                    CreateEditorString( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Color:
                {
                    CreateEditorColor( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Float2:
                {
                    CreateEditorFloat2( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Float3:
                {
                    CreateEditorFloat3( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Float4:
                {
                    CreateEditorFloat4( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Vector:
                {
                    CreateEditorVector( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Quaternion:
                {
                    CreateEditorQuaternion( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Matrix:
                {
                    CreateEditorMatrix( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Transform:
                {
                    CreateEditorTransform( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Microseconds:
                {
                    CreateEditorMicroseconds( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Milliseconds:
                {
                    CreateEditorMilliseconds( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Seconds:
                {
                    CreateEditorSeconds( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Percentage:
                {
                    CreateEditorPercentage( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Degrees:
                {
                    CreateEditorDegrees( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Radians:
                {
                    CreateEditorRadians( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::EulerAngles:
                {
                    CreateEditorEulerAngles( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::ResourcePath:
                {
                    CreateEditorResourcePath( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::IntRange:
                {
                    CreateEditorIntRange( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::FloatRange:
                {
                    CreateEditorFloatRange( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::ResourceID:
                {
                    CreateEditorResourceID( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::ResourceTypeID:
                {
                    CreateEditorResourceTypeID( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::TResourcePtr:
                {
                    CreateEditorResourcePtr( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::BitFlags:
                {
                    CreateEditorGenericFlags( propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::TBitFlags:
                {
                    CreateEditorSpecificFlags( propertyInfo, pPropertyInstance );
                }
                break;
            }
        }

        ImGui::PopID();
    }

    //-------------------------------------------------------------------------
    // Property Editors
    //-------------------------------------------------------------------------

    void PropertyGrid::CreateEditorBool( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<bool*>( pPropertyInstance );
        auto tmpValue = *pValue;

        if ( ImGui::Checkbox( g_emptyLabel, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorInt8( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<int8*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S8, &tmpValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorInt16( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<int16*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S16, &tmpValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorInt32( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<int32*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S32, &tmpValue );
        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorInt64( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<int64*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S64, &tmpValue );
        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorUInt8( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<uint8*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U8, &tmpValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorUInt16( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<uint16*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U16, &tmpValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorUInt32( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<uint32*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U32, &tmpValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorUInt64( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<uint64*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U64, &tmpValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::CreateEditorFloat( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<float*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputFloat( g_emptyLabel, &tmpValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorDouble( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<double*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputDouble( g_emptyLabel, &tmpValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    // TODO: This spams change notifiers!!!!!
    void PropertyGrid::CreateEditorColor( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Color*>( pPropertyInstance );
        ImVec4 color4 = pValue->ToFloat4();

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::ColorEdit4( g_emptyLabel, &color4.x ) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = Color( color4 );
        }
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::CreateEditorMicroseconds( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Microseconds*>( pPropertyInstance );
        float floatValue = pValue->ToFloat();

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputFloat( g_emptyLabel, &floatValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = Microseconds( floatValue );
        }
    }

    void PropertyGrid::CreateEditorMilliseconds( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Milliseconds*>( pPropertyInstance );
        float floatValue = pValue->ToFloat();

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputFloat( g_emptyLabel, &floatValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = Milliseconds( floatValue );
        }
    }

    void PropertyGrid::CreateEditorSeconds( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Seconds*>( pPropertyInstance );
        float floatValue = pValue->ToFloat();

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputFloat( g_emptyLabel, &floatValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = Seconds( floatValue );
        }
    }

    void PropertyGrid::CreateEditorPercentage( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Percentage*>( pPropertyInstance );
        float floatValue = pValue->ToFloat();

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputFloat( g_emptyLabel, &floatValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = Percentage( floatValue );
        }
    }

    void PropertyGrid::CreateEditorDegrees( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;

        //-------------------------------------------------------------------------

        auto pValue = reinterpret_cast<Degrees*>( pPropertyInstance );
        float tmpValue = pValue->ToFloat();

        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( textAreaWidth );
        ImGui::InputFloat( g_emptyLabel, &tmpValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = Degrees( tmpValue );
        }

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_COMPRESS "##Clamp" ) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = Degrees( tmpValue ).GetClampedToSmallest();
        }
    }

    void PropertyGrid::CreateEditorRadians( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;

        //-------------------------------------------------------------------------

        auto pValue = reinterpret_cast<Radians*>( pPropertyInstance );
        float tmpValue = pValue->ToDegrees().ToFloat();

        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( textAreaWidth );
        ImGui::InputFloat( g_emptyLabel, &tmpValue );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = Radians( Degrees( tmpValue ) );
        }

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_COMPRESS "##Clamp" ) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = Radians( Degrees( tmpValue ) ).GetClampedToSmallest();
        }
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::CreateEditorFloat2( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Float2*>( pPropertyInstance );
        auto tmpValue = *pValue;

        if ( ImGuiX::InputFloat2( "EF2", tmpValue) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorFloat3( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Float3*>( pPropertyInstance );
        auto tmpValue = *pValue;

        if ( ImGuiX::InputFloat3( "EF3", tmpValue ) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorFloat4( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Float4*>( pPropertyInstance );
        auto tmpValue = *pValue;

        if ( ImGuiX::InputFloat4( "EF4", tmpValue ) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorVector( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Vector*>( pPropertyInstance );
        auto tmpValue = pValue->ToFloat4();

        if ( ImGuiX::InputFloat4( "EV", tmpValue ) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = Vector( tmpValue );
        }
    }

    void PropertyGrid::CreateEditorEulerAngles( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<EulerAngles*>( pPropertyInstance );
        Float3 tmpValue = pValue->GetAsDegrees();

        if ( ImGuiX::InputFloat3( "EEA", tmpValue ) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            EulerAngles const newRotation( Degrees( tmpValue.m_x ).GetClampedToSmallest(), Degrees( tmpValue.m_y ).GetClampedToSmallest(), Degrees( tmpValue.m_z ).GetClampedToSmallest() );
            *pValue = newRotation;
        }
    }

    void PropertyGrid::CreateEditorQuaternion( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonWidth = 20;
        float const inputWidth = cellContentWidth - buttonWidth - itemSpacing;

        //-------------------------------------------------------------------------

        auto pValue = reinterpret_cast<Quaternion*>( pPropertyInstance );

        //-------------------------------------------------------------------------

        Float4 quatValues = pValue->ToFloat4();
        ImGuiX::InputFloat4( "EQ", quatValues, inputWidth, true );

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, itemSpacing );

        static Float3 degrees( 0 ); // This should be okay since we should never have more than one of these builders open at one time
        if ( ImGui::Button( KRG_ICON_WRENCH "##Construct" ) )
        {
            // Set the initial values to the best approximation of the current quat
            degrees = pValue->ToEulerAngles().ToFloat3();
            ImGui::OpenPopup( "RotationBuilder" );
        }

        // Rotation Builder
        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 8 ) );
        if ( ImGui::BeginPopupModal( "RotationBuilder", nullptr, ImGuiWindowFlags_AlwaysAutoResize ) )
        {
            ImGuiX::InputFloat3( "R3", degrees );

            // Set new rotation
            if ( ImGui::Button( "Apply", ImVec2( 143, 0 ) ) )
            {
                ScopedChangeNotifier notifier( this, &propertyInfo );
                EulerAngles const newRotation( Degrees( degrees.m_x ).GetClampedToSmallest(), Degrees( degrees.m_y ).GetClampedToSmallest(), Degrees( degrees.m_z ).GetClampedToSmallest() );
                *pValue = Quaternion( newRotation );
                ImGui::CloseCurrentPopup();
            }

            // Close the pop-up
            ImGui::SameLine( 0, 4 );
            if ( ImGui::Button( "Cancel", ImVec2( 143, 0 ) ) )
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        ImGui::PopStyleVar();
    }

    void PropertyGrid::CreateEditorMatrix( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Matrix*>( pPropertyInstance );
        Float3 translation = pValue->GetTranslation().ToFloat3();
        Float3* pRotation = GetCachedRotation( propertyInfo, pPropertyInstance );
        Float3 scale = pValue->GetScale().ToFloat3();

        bool transformUpdated = false;

        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 1, 2 ) );
        if ( ImGui::BeginTable( "Transform", 2, ImGuiTableFlags_None ) )
        {
            ImGui::TableSetupColumn( "Header", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 20 );
            ImGui::TableSetupColumn( "Values", ImGuiTableColumnFlags_NoHide );

            ImGui::TableNextRow();
            {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::Text( "Pos" );

                ImGui::TableNextColumn();
                if ( ImGuiX::InputFloat3( "EMT", translation ) )
                {
                    transformUpdated = true;
                }
            }

            ImGui::TableNextRow();
            {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::Text( "Rot" );

                ImGui::TableNextColumn();
                if ( ImGuiX::InputFloat3( "EMR", *pRotation ) )
                {
                    pRotation->m_x = Degrees( pRotation->m_x ).GetClampedToSmallest().ToFloat();
                    pRotation->m_y = Degrees( pRotation->m_y ).GetClampedToSmallest().ToFloat();
                    pRotation->m_z = Degrees( pRotation->m_z ).GetClampedToSmallest().ToFloat();
                    transformUpdated = true;
                }
            }

            ImGui::TableNextRow();
            {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::Text( "Scl" );

                ImGui::TableNextColumn();
                if ( ImGuiX::InputFloat3( "EMS", scale ) )
                {
                    transformUpdated = true;
                }
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        //-------------------------------------------------------------------------

        if ( transformUpdated )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = Matrix( Quaternion( EulerAngles( *pRotation ) ), translation, scale );
        }
    }

    void PropertyGrid::CreateEditorTransform( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Transform*>( pPropertyInstance );
        Float3 translation = pValue->GetTranslation().ToFloat3();
        Float3* pRotation = GetCachedRotation( propertyInfo, pPropertyInstance );
        Float3 scale = pValue->GetScale().ToFloat3();

        bool transformUpdated = false;

        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 1, 2 ) );
        if ( ImGui::BeginTable( "Transform", 2, ImGuiTableFlags_None ) )
        {
            ImGui::TableSetupColumn( "Header", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 20 );
            ImGui::TableSetupColumn( "Values", ImGuiTableColumnFlags_NoHide );

            ImGui::TableNextRow();
            {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::Text( "Pos" );

                ImGui::TableNextColumn();
                if ( ImGuiX::InputFloat3( "ETT", translation ) )
                {
                    transformUpdated = true;
                }
            }

            ImGui::TableNextRow();
            {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::Text( "Rot" );

                ImGui::TableNextColumn();
                if ( ImGuiX::InputFloat3( "ETR", *pRotation ) )
                {
                    pRotation->m_x = Degrees( pRotation->m_x ).GetClampedToSmallest().ToFloat();
                    pRotation->m_y = Degrees( pRotation->m_y ).GetClampedToSmallest().ToFloat();
                    pRotation->m_z = Degrees( pRotation->m_z ).GetClampedToSmallest().ToFloat();
                    transformUpdated = true;
                }
            }

            ImGui::TableNextRow();
            {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::Text( "Scl" );

                ImGui::TableNextColumn();
                if ( ImGuiX::InputFloat3( "ETS", scale ) )
                {
                    transformUpdated = true;
                }
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        //-------------------------------------------------------------------------

        if ( transformUpdated )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = Transform( Quaternion( EulerAngles( *pRotation ) ), translation, scale );
        }
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::CreateEditorIntRange( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        IntRange* pValue = reinterpret_cast<IntRange*>( pPropertyInstance );
        IntRange tmpValue = *pValue;

        float const contentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const inputWidth = ( contentWidth - ( itemSpacing * 1 ) ) / 2;

        //-------------------------------------------------------------------------

        bool valueUpdated = false;

        ImGui::PushID( pValue );
        {
            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputScalar( "##min", ImGuiDataType_S32, &tmpValue.m_start, 0, 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, itemSpacing );

            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputScalar( "##max", ImGuiDataType_S32, &tmpValue.m_end, 0, 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }
        }
        ImGui::PopID();

        //-------------------------------------------------------------------------

        if ( valueUpdated )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    void PropertyGrid::CreateEditorFloatRange( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        FloatRange* pValue = reinterpret_cast<FloatRange*>( pPropertyInstance );
        FloatRange tmpValue = *pValue;

        float const contentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const inputWidth = ( contentWidth - ( itemSpacing * 1 ) ) / 2;

        //-------------------------------------------------------------------------

        bool valueUpdated = false;

        ImGui::PushID( pValue );
        {
            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##min", &tmpValue.m_start, 0, 0, "%.3f", 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, itemSpacing );

            ImGui::SetNextItemWidth( inputWidth );
            ImGui::InputFloat( "##max", &tmpValue.m_end, 0, 0, "%.3f", 0 );

            if ( ImGui::IsItemDeactivatedAfterEdit() )
            {
                valueUpdated = true;
            }
        }
        ImGui::PopID();

        //-------------------------------------------------------------------------

        if ( valueUpdated )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = tmpValue;
        }
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::CreateEditorString( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        char buffer[256];
        auto pValue = reinterpret_cast<String*>( pPropertyInstance );
        strcpy_s( buffer, 256, pValue->c_str() );

        ImGui::SetNextItemWidth( -1 );
        ImGui::InputText( g_emptyLabel, buffer, 256 );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = String( buffer );
        }
    }

    void PropertyGrid::CreateEditorStringID( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const childWindowWidth = 86;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const textAreaWidth = cellContentWidth - childWindowWidth - itemSpacing;

        //-------------------------------------------------------------------------

        char buffer[256];
        auto pValue = reinterpret_cast<StringID*>( pPropertyInstance );
        strcpy_s( buffer, 256, pValue->c_str() );

        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 2 ) );
        ImGui::BeginChild( "IDLabel", ImVec2( childWindowWidth, 18 ), true, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
        {
            ImGui::TextColored( Colors::LightGreen.ToFloat4(), "%u", pValue->GetID() );
        }
        ImGui::EndChild();
        ImGui::PopStyleVar( 2 );

        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( textAreaWidth );
        ImGui::SameLine( 0, itemSpacing );
        ImGui::InputText( g_emptyLabel, buffer, 256 );

        if ( ImGui::IsItemDeactivatedAfterEdit() )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = StringID( buffer );
        }
    }

    void PropertyGrid::CreateEditorUUID( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;

        auto pValue = reinterpret_cast<UUID*>( pPropertyInstance );
        auto UUIDStr = pValue->ToString();

        ImGui::SetNextItemWidth( textAreaWidth );
        ImGui::InputText( g_emptyLabel, UUIDStr.data(), UUIDStr.length(), ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_REFRESH "##Generate" ) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = UUID::GenerateID();
        }
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::CreateEditorResourcePath( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const textAreaWidth = cellContentWidth - ( buttonAreaWidth * 2 ) - ( itemSpacing * 2 );

        //-------------------------------------------------------------------------

        auto pValue = reinterpret_cast<ResourcePath*>( pPropertyInstance );
        String pathStr = pValue->GetString();

        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( textAreaWidth );
        ImGui::InputText( g_emptyLabel, pathStr.data(), pathStr.length(), ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick", ImVec2( buttonAreaWidth - 1, 0 ) ) )
        {
            ResourcePath pickedResourcePath;
            if ( DataFilePicker::PickFile( m_rawResourceDirectoryPath, pickedResourcePath ) )
            {
                ScopedChangeNotifier notifier( this, &propertyInfo );
                *pValue = pickedResourcePath;
            }
        }

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_TIMES_CIRCLE "##Clear" ) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = ResourcePath();
        }
    }

    void PropertyGrid::CreateEditorResourceTypeID( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const childWindowWidth = 86;
        float const textAreaWidth = cellContentWidth - childWindowWidth - itemSpacing;

        //-------------------------------------------------------------------------

        char currentResourceTypeStr[5];
        ResourceTypeID* pValue = reinterpret_cast<ResourceTypeID*>( pPropertyInstance );
        pValue->GetString( currentResourceTypeStr );

        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 2 ) );
        ImGui::BeginChild( "IDLabel", ImVec2( childWindowWidth, 18 ), true, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
        {
            ImGui::TextColored( Colors::LightPink.ToFloat4(), "%u", pValue->m_ID );
        }
        ImGui::EndChild();
        ImGui::PopStyleVar( 2 );

        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( textAreaWidth );
        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::BeginCombo( g_emptyLabel, currentResourceTypeStr ) )
        {
            auto AddComboItem = [this, &propertyInfo, &currentResourceTypeStr, &pValue] ( ResourceInfo const& resourceInfo )
            {
                bool const isSelected = ( pValue->m_ID == resourceInfo.m_resourceTypeID );
                if ( ImGui::Selectable( resourceInfo.m_friendlyName.c_str(), isSelected ) )
                {
                    if ( resourceInfo.m_resourceTypeID != *pValue )
                    {
                        ScopedChangeNotifier notifier( this, &propertyInfo );
                        *pValue = resourceInfo.m_resourceTypeID;
                    }
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if ( isSelected )
                {
                    ImGui::SetItemDefaultFocus();
                }
            };

            //-------------------------------------------------------------------------

            AddComboItem( ResourceInfo() );

            //-------------------------------------------------------------------------

            auto const& registeredResourceTypes = m_typeRegistry.GetRegisteredResourceTypes();
            for ( auto& pair : registeredResourceTypes )
            {
                AddComboItem( pair.second );
            }

            ImGui::EndCombo();
        }
    }

    void PropertyGrid::CreateEditorResourceID( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const childWindowWidth = 50;

        //-------------------------------------------------------------------------

        char resourceTypeStr[5];
        auto pValue = reinterpret_cast<ResourceID*>( pPropertyInstance );
        pValue->GetResourceTypeID().GetString( resourceTypeStr );

        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 2 ) );
        ImGui::BeginChild( "IDLabel", ImVec2( childWindowWidth, 18 ), true, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
        {
            ImGui::TextColored( Colors::LightPink.ToFloat4(), resourceTypeStr );
        }
        ImGui::EndChild();
        ImGui::PopStyleVar( 2 );

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, itemSpacing );
        ImGui::SetNextItemWidth( cellContentWidth - ( itemSpacing * 3 ) - ( buttonAreaWidth * 2 ) - childWindowWidth );
        ImGui::InputText( g_emptyLabel, const_cast<char*>( pValue->c_str() ), pValue->GetResourcePath().GetString().length(), ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick" ) )
        {
            TVector<ResourceTypeID> allowedResourceTypes;
            for ( auto const& registeredResourceType : m_typeRegistry.GetRegisteredResourceTypes() )
            {
                allowedResourceTypes.emplace_back( registeredResourceType.second.m_resourceTypeID );
            }

            ResourcePath pickedResourcePath;
            if ( DataFilePicker::PickResourceFile( m_rawResourceDirectoryPath, allowedResourceTypes, pickedResourcePath ) )
            {
                ScopedChangeNotifier notifier( this, &propertyInfo );
                *pValue = ResourceID( pickedResourcePath );
            }
        }

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_TIMES_CIRCLE "##Clear" ) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = ResourceID();
        }
    }

    void PropertyGrid::CreateEditorResourcePtr( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const childWindowWidth = 50;

        //-------------------------------------------------------------------------

        auto pValue = reinterpret_cast<Resource::ResourcePtr*>( pPropertyInstance );

        char resourceTypeStr[5] = { 0 };
        ResourceTypeID const resourceTypeID = m_typeRegistry.GetResourceInfoForType( propertyInfo.m_templateArgumentTypeID )->m_resourceTypeID;
        resourceTypeID.GetString( resourceTypeStr );

        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 2 ) );
        ImGui::BeginChild( "IDLabel", ImVec2( childWindowWidth, 18 ), true, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
        {
            ImGui::TextColored( Colors::LightPink.ToFloat4(), resourceTypeStr );
        }
        ImGui::EndChild();
        ImGui::PopStyleVar( 2 );

        //-------------------------------------------------------------------------

        ImGui::SameLine( 0, itemSpacing );
        ImGui::SetNextItemWidth( cellContentWidth - ( itemSpacing * 3 ) - ( buttonAreaWidth * 2 ) - childWindowWidth );
        ImGui::InputText( g_emptyLabel, const_cast<char*>( pValue->GetResourceID().c_str() ), pValue->GetResourcePath().GetString().length(), ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick" ) )
        {
            ResourcePath pickedResourcePath;
            if ( DataFilePicker::PickResourceFile( m_rawResourceDirectoryPath, resourceTypeID, pickedResourcePath ) )
            {
                ScopedChangeNotifier notifier( this, &propertyInfo );
                *pValue = Resource::ResourcePtr( pickedResourcePath );
            }
        }

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_TIMES_CIRCLE "##Clear" ) )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );
            *pValue = ResourceID();
        }
    }

    //-------------------------------------------------------------------------

    void PropertyGrid::CreateEditorEnum( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pEnumInfo = m_typeRegistry.GetEnumInfo( propertyInfo.m_typeID );
        KRG_ASSERT( pEnumInfo != nullptr );

        // Get current value
        //-------------------------------------------------------------------------

        int64 currentEnumValue = 0;

        switch ( pEnumInfo->m_underlyingType )
        {
            case CoreTypes::Uint8:
            {
                currentEnumValue = ( int64 ) * reinterpret_cast<uint8 const*>( pPropertyInstance );
            }
            break;

            case CoreTypes::Int8:
            {
                currentEnumValue = ( int64 ) * reinterpret_cast<int8 const*>( pPropertyInstance );
            }
            break;

            case CoreTypes::Uint16:
            {
                currentEnumValue = ( int64 ) * reinterpret_cast<uint16 const*>( pPropertyInstance );
            }
            break;

            case CoreTypes::Int16:
            {
                currentEnumValue = ( int64 ) * reinterpret_cast<int16 const*>( pPropertyInstance );
            }
            break;

            case CoreTypes::Uint32:
            {
                currentEnumValue = ( int64 ) * reinterpret_cast<uint32 const*>( pPropertyInstance );
            }
            break;

            case CoreTypes::Int32:
            {
                currentEnumValue = ( int64 ) * reinterpret_cast<int32 const*>( pPropertyInstance );
            }
            break;

            default:
            {
                KRG_UNREACHABLE_CODE();
            }
            break;
        }

        // Draw UI
        //-------------------------------------------------------------------------

        int64 newEnumValue = currentEnumValue;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::BeginCombo( g_emptyLabel, pEnumInfo->GetConstantLabel( currentEnumValue ).c_str() ) )
        {
            for ( auto const& enumValue : pEnumInfo->m_constants )
            {
                bool const isSelected = enumValue.second == currentEnumValue;
                if ( ImGui::Selectable( enumValue.first.c_str(), isSelected ) )
                {
                    newEnumValue = enumValue.second;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if ( isSelected )
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        // Set new value
        //-------------------------------------------------------------------------

        if ( newEnumValue != currentEnumValue )
        {
            ScopedChangeNotifier notifier( this, &propertyInfo );

            switch ( pEnumInfo->m_underlyingType )
            {
                case CoreTypes::Uint8:
                {
                    *reinterpret_cast<uint8*>( pPropertyInstance ) = (uint8) newEnumValue;
                }
                break;

                case CoreTypes::Int8:
                {
                    *reinterpret_cast<int8*>( pPropertyInstance ) = (int8) newEnumValue;
                }
                break;

                case CoreTypes::Uint16:
                {
                    *reinterpret_cast<uint16*>( pPropertyInstance ) = (uint16) newEnumValue;
                }
                break;

                case CoreTypes::Int16:
                {
                    *reinterpret_cast<int16*>( pPropertyInstance ) = (int16) newEnumValue;
                }
                break;

                case CoreTypes::Uint32:
                {
                    *reinterpret_cast<uint32*>( pPropertyInstance ) = (uint32) newEnumValue;
                }
                break;

                case CoreTypes::Int32:
                {
                    *reinterpret_cast<int32*>( pPropertyInstance ) = (int32) newEnumValue;
                }
                break;
            }
        }
    }

    //------------------------------------------------------------------------- 

    void PropertyGrid::CreateEditorGenericFlags( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pFlags = reinterpret_cast<BitFlags*>( pPropertyInstance );

        bool v[32];
        for ( uint8 i = 0u; i < 32; i++ )
        {
            v[i] = pFlags->IsFlagSet( i );
        }

        //-------------------------------------------------------------------------

        constexpr static char const* const rowLabels[4] = { "00-07", "08-15", "16-23", "24-31" };

        if ( ImGui::BeginTable( "FlagsTable", 9, ImGuiTableFlags_SizingFixedFit ) )
        {
            for ( uint8 i = 0u; i < 4; i++ )
            {
                ImGui::TableNextRow();

                for ( uint8 j = 0u; j < 8; j++ )
                {
                    uint8 const flagIdx = i * 8 + j;
                    ImGui::TableNextColumn();

                    ImGui::PushID( &v[flagIdx] );
                    if ( ImGui::Checkbox( g_emptyLabel, &v[flagIdx] ) )
                    {
                        ScopedChangeNotifier notifier( this, &propertyInfo );
                        pFlags->SetFlag( flagIdx, v[flagIdx] );
                    }
                    ImGui::PopID();
                }

                ImGui::TableNextColumn();
                ImGui::Text( rowLabels[i] );
            }

            ImGui::EndTable();
        }
    }

    void PropertyGrid::CreateEditorSpecificFlags( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pFlags = reinterpret_cast<BitFlags*>( pPropertyInstance );

        bool v[32];
        for ( uint8 i = 0u; i < 32; i++ )
        {
            v[i] = pFlags->IsFlagSet( i );
        }

        //-------------------------------------------------------------------------

        // Get enum type for specific flags
        TypeID const enumTypeID = propertyInfo.m_templateArgumentTypeID;
        EnumInfo const* pEnumInfo = m_typeRegistry.GetEnumInfo( enumTypeID );
        KRG_ASSERT( pEnumInfo != nullptr );

        //-------------------------------------------------------------------------

        if ( ImGui::BeginTable( "FlagsTable", 2, ImGuiTableFlags_SizingFixedFit ) )
        {
            int32 flagCount = 0;

            // For each label 
            for ( auto const& constant : pEnumInfo->m_constants )
            {
                if ( ( flagCount % 2 ) == 0 )
                {
                    ImGui::TableNextRow();
                }

                ImGui::TableNextColumn();
                flagCount++;

                //-------------------------------------------------------------------------

                int64 const flagValue = constant.second;
                KRG_ASSERT( flagValue >= 0 && flagValue <= 31 );
                if ( ImGui::Checkbox( constant.first.c_str(), &v[flagValue] ) )
                {
                    ScopedChangeNotifier notifier( this, &propertyInfo );
                    pFlags->SetFlag( (uint8) flagValue, v[flagValue] );
                }
            }

            ImGui::EndTable();
        }
    }

    //-------------------------------------------------------------------------

    Float3* PropertyGrid::GetCachedRotation( PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        Float3* pCachedValue = nullptr;

        // Try find existing value
        for ( auto& cachedItem : m_rotationCache )
        {
            if ( cachedItem.first == pPropertyInstance )
            {
                pCachedValue = cachedItem.second;
            }
        }

        // Create new cached value
        if ( pCachedValue == nullptr )
        {
            pCachedValue = KRG::New<Float3>( InitToZero );
            m_rotationCache.emplace_back( TPair<void*, Float3*>( pPropertyInstance, pCachedValue ) );
        }

        // Check with actual instance value
        //-------------------------------------------------------------------------

        Quaternion currentQuat;

        CoreTypes const coreType = GetCoreType( propertyInfo.m_typeID );
        if ( coreType == CoreTypes::Transform )
        {
            currentQuat = reinterpret_cast<Transform*>( pPropertyInstance )->GetRotation();
        }
        else  if ( coreType == CoreTypes::Matrix )
        {
            currentQuat = reinterpret_cast<Matrix*>( pPropertyInstance )->GetRotation();
        }
        else
        {
            KRG_UNREACHABLE_CODE();
        }

        EulerAngles const cachedEulerAngles( *pCachedValue );
        Quaternion const cachedQuat = Quaternion( cachedEulerAngles );
        Radians const distance = Quaternion::Distance( cachedQuat, currentQuat );

        if ( distance > Degrees( 1.0f ) )
        {
            *pCachedValue = currentQuat.ToEulerAngles().GetAsDegrees();
        }

        //-------------------------------------------------------------------------

        return pCachedValue;
    }

    void PropertyGrid::ResetCachedRotation( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        for ( auto& cachedItem : m_rotationCache )
        {
            if ( cachedItem.first == pPropertyInstance )
            {
                CoreTypes const coreType = GetCoreType( propertyInfo.m_typeID );
                if ( coreType == CoreTypes::Transform )
                {
                    auto pValue = reinterpret_cast<Transform*>( pPropertyInstance );
                    *cachedItem.second = pValue->GetRotation().ToEulerAngles().GetAsDegrees();
                    return;
                }
                else  if ( coreType == CoreTypes::Matrix )
                {
                    auto pValue = reinterpret_cast<Matrix*>( pPropertyInstance );
                    *cachedItem.second = pValue->GetRotation().ToEulerAngles().GetAsDegrees();
                    return;
                }
                else
                {
                    KRG_UNREACHABLE_CODE();
                }
            }
        }

        KRG_UNREACHABLE_CODE();
    }

    void PropertyGrid::ClearCache()
    {
        for ( auto& cachedItem : m_rotationCache )
        {
            KRG::Delete( cachedItem.second );
        }

        m_rotationCache.clear();
    }
}