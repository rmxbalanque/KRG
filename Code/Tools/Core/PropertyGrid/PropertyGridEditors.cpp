#include "PropertyGridEditors.h"
#include "Tools/Core/DataFileTools/DataFilePicker.h"
#include "System/DevTools/CommonWidgets/NumericEditors.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

using namespace KRG::TypeSystem;

//-------------------------------------------------------------------------

namespace KRG::PG
{
    constexpr static char const* const g_emptyLabel = "##";

    struct [[nodiscard]] ScopedChangeNotifier
    {
        ScopedChangeNotifier( Context& ctx )
            : m_ctx( ctx )
        {
            m_ctx.m_preChangeDelegate();
        }

        ~ScopedChangeNotifier()
        {
            m_ctx.m_postChangeDelegate();
        }

        Context&                    m_ctx;
    };

    //-------------------------------------------------------------------------

    void CreateEditorBool( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<bool*>( pPropertyInstance );
        auto tmpValue = *pValue;

        if ( ImGui::Checkbox( g_emptyLabel, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorInt8( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<int8*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S8, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorInt16( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<int16*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S16, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorInt32( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<int32*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S32, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorInt64( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<int64*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S64, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorUInt8( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<uint8*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U8, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorUInt16( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<uint16*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U16, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorUInt32( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<uint32*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U32, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorUInt64( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<uint64*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U64, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    //-------------------------------------------------------------------------

    void CreateEditorFloat( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<float*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorDouble( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<double*>( pPropertyInstance );
        auto tmpValue = *pValue;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputDouble( g_emptyLabel, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorColor( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Color*>( pPropertyInstance );
        ImVec4 color4 = pValue->ToFloat4();

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::ColorEdit4( g_emptyLabel, &color4.x ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = Color( color4 );
        }
    }

    //-------------------------------------------------------------------------

    void CreateEditorMicroseconds( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Microseconds*>( pPropertyInstance );
        float floatValue = pValue->ToFloat();

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &floatValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = Microseconds( floatValue );
        }
    }

    void CreateEditorMilliseconds( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Milliseconds*>( pPropertyInstance );
        float floatValue = pValue->ToFloat();

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &floatValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = Milliseconds( floatValue );
        }
    }

    void CreateEditorSeconds( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Seconds*>( pPropertyInstance );
        float floatValue = pValue->ToFloat();

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &floatValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = Seconds( floatValue );
        }
    }

    void CreateEditorPercentage( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Percentage*>( pPropertyInstance );
        float floatValue = pValue->ToFloat();

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &floatValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = Percentage( floatValue );
        }
    }

    void CreateEditorDegrees( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
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
        if ( ImGui::InputFloat( g_emptyLabel, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = Degrees( tmpValue );
        }

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_COMPRESS "##Clamp" ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = Degrees( tmpValue ).GetClampedToSmallest();
        }
    }

    void CreateEditorRadians( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
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
        if ( ImGui::InputFloat( g_emptyLabel, &tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = Radians( Degrees( tmpValue ) );
        }

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_COMPRESS "##Clamp" ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = Radians( Degrees( tmpValue ) ).GetClampedToSmallest();
        }
    }

    //-------------------------------------------------------------------------

    void CreateEditorFloat2( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Float2*>( pPropertyInstance );
        auto tmpValue = *pValue;

        if ( ImGuiX::InputFloat2( tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorFloat3( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Float3*>( pPropertyInstance );
        auto tmpValue = *pValue;

        if ( ImGuiX::InputFloat3( tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorFloat4( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Float4*>( pPropertyInstance );
        auto tmpValue = *pValue;

        if ( ImGuiX::InputFloat4( tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = tmpValue;
        }
    }

    void CreateEditorVector( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Vector*>( pPropertyInstance );
        auto tmpValue = pValue->ToFloat4();

        if ( ImGuiX::InputFloat4( tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = Vector( tmpValue );
        }
    }

    void CreateEditorEulerAngles( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<EulerAngles*>( pPropertyInstance );
        Float3 tmpValue = pValue->GetAsDegrees();

        if ( ImGuiX::InputFloat3( tmpValue ) )
        {
            ScopedChangeNotifier notifier( ctx );
            EulerAngles const newRotation( Degrees( tmpValue.m_x ).GetClampedToSmallest(), Degrees( tmpValue.m_y ).GetClampedToSmallest(), Degrees( tmpValue.m_z ).GetClampedToSmallest() );
            *pValue = newRotation;
        }
    }

    void CreateEditorQuaternion( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonWidth = 20;
        float const inputWidth = cellContentWidth - buttonWidth - itemSpacing;

        //-------------------------------------------------------------------------

        auto pValue = reinterpret_cast<Quaternion*>( pPropertyInstance );

        //-------------------------------------------------------------------------

        Float4 quatValues = pValue->ToFloat4();
        ImGuiX::InputFloat4( quatValues, inputWidth, true );

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
            ImGuiX::InputFloat3( degrees );

            // Set new rotation
            if ( ImGui::Button( "Apply", ImVec2( 143, 0 ) ) )
            {
                ScopedChangeNotifier notifier( ctx );
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

    void CreateEditorMatrix( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Matrix*>( pPropertyInstance );
        Float3 translation = pValue->GetTranslation().ToFloat3();
        Float3 rotation = Float3::Zero;// pValue->GetRotation().ToEulerAngles().GetAsDegrees();
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
                if ( ImGuiX::InputFloat3( translation ) )
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
                if ( ImGuiX::InputFloat3( rotation ) )
                {
                    rotation.m_x = Degrees( rotation.m_x ).GetClampedToSmallest().ToFloat();
                    rotation.m_y = Degrees( rotation.m_y ).GetClampedToSmallest().ToFloat();
                    rotation.m_z = Degrees( rotation.m_z ).GetClampedToSmallest().ToFloat();
                    transformUpdated = true;
                }
            }

            ImGui::TableNextRow();
            {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::Text( "Scl" );

                ImGui::TableNextColumn();
                if ( ImGuiX::InputFloat3( scale ) )
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
            ScopedChangeNotifier notifier( ctx );
            *pValue = Matrix( Quaternion( EulerAngles( rotation ) ), translation, scale );
        }
    }

    void CreateEditorTransform( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pValue = reinterpret_cast<Transform*>( pPropertyInstance );
        Float3 translation = pValue->GetTranslation().ToFloat3();
        Float3 rotation = pValue->GetRotation().ToEulerAngles().GetAsDegrees();
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
                if ( ImGuiX::InputFloat3( translation ) )
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
                if ( ImGuiX::InputFloat3( rotation ) )
                {
                    rotation.m_x = Degrees( rotation.m_x ).GetClampedToSmallest().ToFloat();
                    rotation.m_y = Degrees( rotation.m_y ).GetClampedToSmallest().ToFloat();
                    rotation.m_z = Degrees( rotation.m_z ).GetClampedToSmallest().ToFloat();
                    transformUpdated = true;
                }
            }

            ImGui::TableNextRow();
            {
                ImGui::TableNextColumn();
                ImGui::AlignTextToFramePadding();
                ImGui::Text( "Scl" );

                ImGui::TableNextColumn();
                if ( ImGuiX::InputFloat3( scale ) )
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
            ScopedChangeNotifier notifier( ctx );
            *pValue = Transform( Quaternion( EulerAngles( rotation ) ), translation, scale );
        }
    }

    //-------------------------------------------------------------------------

    void CreateEditorIntRange( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        KRG_UNIMPLEMENTED_FUNCTION();

        auto pValue = reinterpret_cast<IntRange*>( pPropertyInstance );
        int32 tmpValue = 0;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U32, &tmpValue ) )
        {
            //ScopedChangeNotifier notifier( ctx );
            //*pValue = tmpValue;
        }
    }

    void CreateEditorFloatRange( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        KRG_UNIMPLEMENTED_FUNCTION();

        auto pValue = reinterpret_cast<FloatRange*>( pPropertyInstance );
        float tmpValue = 0;

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &tmpValue ) )
        {
            //ScopedChangeNotifier notifier( ctx );
            //*pValue = tmpValue;
        }
    }

    //-------------------------------------------------------------------------

    void CreateEditorString( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        char buffer[256];
        auto pValue = reinterpret_cast<String*>( pPropertyInstance );
        strcpy_s( buffer, 256, pValue->c_str() );

        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputText( g_emptyLabel, buffer, 256 ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = String( buffer );
        }
    }

    void CreateEditorStringID( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
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
        if ( ImGui::InputText( g_emptyLabel, buffer, 256 ) )
        {
            ScopedChangeNotifier notifier( ctx );
            *pValue = StringID( buffer );
        }
    }

    void CreateEditorUUID( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
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
            ScopedChangeNotifier notifier( ctx );
            *pValue = UUID::GenerateID();
        }
    }

    //-------------------------------------------------------------------------

    void CreateEditorDataPath( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;

        //-------------------------------------------------------------------------

        auto pValue = reinterpret_cast<DataPath*>( pPropertyInstance );
        String dataPathStr = pValue->GetString();

        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( textAreaWidth );
        ImGui::InputText( g_emptyLabel, dataPathStr.data(), dataPathStr.length(), ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick", ImVec2( buttonAreaWidth - 1, 0 ) ) )
        {
            DataPath pickedDataPath;
            if ( DataFilePicker::PickFile( ctx.m_sourceDataPath, pickedDataPath ) )
            {
                ScopedChangeNotifier notifier( ctx );
                *pValue = pickedDataPath;
            }
        }
    }

    void CreateEditorResourceTypeID( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const childWindowWidth = 86;
        float const textAreaWidth = cellContentWidth - childWindowWidth - itemSpacing;

        //-------------------------------------------------------------------------

        char currentResourceTypeStr[5];
        auto pValue = reinterpret_cast<ResourceTypeID*>( pPropertyInstance );
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
            auto AddComboItem = [&ctx, &currentResourceTypeStr, &pValue] ( ResourceTypeID ID )
            {
                char resourceTypeStr[5];
                ID.GetString( resourceTypeStr );

                bool const isSelected = ( pValue->m_ID == ID.m_ID );
                if ( ImGui::Selectable( resourceTypeStr, isSelected ) )
                {
                    ScopedChangeNotifier notifier( ctx );
                    *pValue = ResourceTypeID( ID );
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if ( isSelected )
                {
                    ImGui::SetItemDefaultFocus();
                }
            };

            //-------------------------------------------------------------------------

            AddComboItem( ResourceTypeID::Unknown );

            //-------------------------------------------------------------------------

            auto const& registeredResourceTypes = ctx.m_typeRegistry.GetRegisteredResourceTypes();
            for ( auto& pair : registeredResourceTypes )
            {
                AddComboItem( pair.second );
            }

            ImGui::EndCombo();
        }
    }

    void CreateEditorResourceID( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
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
        ImGui::SetNextItemWidth( cellContentWidth - ( itemSpacing * 2 ) - buttonAreaWidth - childWindowWidth );
        ImGui::InputText( g_emptyLabel, const_cast<char*>( pValue->c_str() ), pValue->GetDataPath().GetString().length(), ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick" ) )
        {
            TVector<ResourceTypeID> allowedResourceTypes;
            for ( auto const& registeredResourceType : ctx.m_typeRegistry.GetRegisteredResourceTypes() )
            {
                allowedResourceTypes.emplace_back( registeredResourceType.second );
            }

            DataPath pickedDataPath;
            if ( DataFilePicker::PickResourceFile( ctx.m_sourceDataPath, allowedResourceTypes, pickedDataPath ) )
            {
                ScopedChangeNotifier notifier( ctx );
                *pValue = ResourceID( pickedDataPath );
            }
        }
    }

    void CreateEditorResourcePtr( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const childWindowWidth = 50;

        //-------------------------------------------------------------------------

        auto pValue = reinterpret_cast<Resource::ResourcePtr*>( pPropertyInstance );

        char resourceTypeStr[5] = { 0 };
        ResourceTypeID const resourceTypeID = ctx.m_typeRegistry.GetResourceTypeIDForType( propertyInfo.m_templateArgumentTypeID );
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
        ImGui::SetNextItemWidth( cellContentWidth - ( itemSpacing * 2 ) - buttonAreaWidth - childWindowWidth );
        ImGui::InputText( g_emptyLabel, const_cast<char*>( pValue->GetResourceID().c_str() ), pValue->GetResourceID().GetDataPath().GetString().length(), ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick" ) )
        {
            DataPath pickedDataPath;
            if ( DataFilePicker::PickResourceFile( ctx.m_sourceDataPath, resourceTypeID, pickedDataPath ) )
            {
                ScopedChangeNotifier notifier( ctx );
                *pValue = Resource::ResourcePtr( pickedDataPath );
            }
        }
    }

    //-------------------------------------------------------------------------

    void CreateEditorEnum( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        auto pEnumInfo = ctx.m_typeRegistry.GetEnumInfo( propertyInfo.m_typeID );
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
            for( auto const& enumValue : pEnumInfo->m_constants )
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
            ScopedChangeNotifier notifier( ctx );

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

    void CreateEditorGenericFlags( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
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
                        ScopedChangeNotifier notifier( ctx );
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

    void CreateEditorSpecificFlags( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
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
        EnumInfo const* pEnumInfo = ctx.m_typeRegistry.GetEnumInfo( enumTypeID );
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
                    ScopedChangeNotifier notifier( ctx );
                    pFlags->SetFlag( (uint8) flagValue, v[flagValue] );
                }
            }

            ImGui::EndTable();
        }
    }

    //-------------------------------------------------------------------------

    void CreatePropertyEditor( Context& ctx, PropertyInfo const& propertyInfo, Byte* pPropertyInstance )
    {
        ImGui::PushID( pPropertyInstance );

        if ( propertyInfo.IsEnumProperty() )
        {
            CreateEditorEnum( ctx, propertyInfo, pPropertyInstance );
        }
        else
        {
            CoreTypes const coreType = GetCoreType( propertyInfo.m_typeID );
            switch ( coreType )
            {
                case CoreTypes::Bool:
                {
                    CreateEditorBool( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Int8:
                {
                    CreateEditorInt8( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Int16:
                {
                    CreateEditorInt16( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Int32:
                {
                    CreateEditorInt32( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Int64:
                {
                    CreateEditorInt64( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Uint8:
                {
                    CreateEditorUInt8( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Uint16:
                {
                    CreateEditorUInt16( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Uint32:
                {
                    CreateEditorUInt32( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Uint64:
                {
                    CreateEditorUInt64( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Float:
                {
                    CreateEditorFloat( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Double:
                {
                    CreateEditorDouble( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::UUID:
                {
                    CreateEditorUUID( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::StringID:
                {
                    CreateEditorStringID( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::String:
                {
                    CreateEditorString( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Color:
                {
                    CreateEditorColor( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Float2:
                {
                    CreateEditorFloat2( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Float3:
                {
                    CreateEditorFloat3( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Float4:
                {
                    CreateEditorFloat4( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Vector:
                {
                    CreateEditorVector( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Quaternion:
                {
                    CreateEditorQuaternion( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Matrix:
                {
                    CreateEditorMatrix( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Transform:
                {
                    CreateEditorTransform( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Microseconds:
                {
                    CreateEditorMicroseconds( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Milliseconds:
                {
                    CreateEditorMilliseconds( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Seconds:
                {
                    CreateEditorSeconds( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Percentage:
                {
                    CreateEditorPercentage( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Degrees:
                {
                    CreateEditorDegrees( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::Radians:
                {
                    CreateEditorRadians( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::EulerAngles:
                {
                    CreateEditorEulerAngles( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::DataPath:
                {
                    CreateEditorDataPath( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::IntRange:
                {
                    CreateEditorIntRange( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::FloatRange:
                {
                    CreateEditorFloatRange( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::ResourceID:
                {
                    CreateEditorResourceID( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::ResourceTypeID:
                {
                    CreateEditorResourceTypeID( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::TResourcePtr:
                {
                    CreateEditorResourcePtr( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::BitFlags:
                {
                    CreateEditorGenericFlags( ctx, propertyInfo, pPropertyInstance );
                }
                break;

                case CoreTypes::TBitFlags:
                {
                    CreateEditorSpecificFlags( ctx, propertyInfo, pPropertyInstance );
                }
                break;
            }
        }

        ImGui::PopID();
    }
}