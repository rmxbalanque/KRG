#include "PropertyEditors.h"
#include "Applications/Editor/Widgets/DataFilePicker.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "System/DevTools/ImguiX.h"
#include "System/DevTools/CommonWidgets/NumericEditors.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    constexpr static char const* const g_emptyLabel = "##";

    //-------------------------------------------------------------------------

    bool CreateEditorBool( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        bool v = propertyModel.GetValue<bool>();

        ImGui::PushID( &propertyModel );
        if ( ImGui::Checkbox( g_emptyLabel, &v ) )
        {
            propertyModel.SetValue( v );
            propertyUpdated = true;
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorInt8( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        auto v = propertyModel.GetValue<int8>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S8, &v ) )
        {
            propertyModel.SetValue( v );
            propertyUpdated = true;
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorInt16( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        auto v = propertyModel.GetValue<int16>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S16, &v ) )
        {
            propertyModel.SetValue( v );
            propertyUpdated = true;
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorInt32( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        auto v = propertyModel.GetValue<int32>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S32, &v ) )
        {
            propertyModel.SetValue( v );
            propertyUpdated = true;
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorInt64( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        auto v = propertyModel.GetValue<int64>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S64, &v ) )
        {
            propertyModel.SetValue( v );
            propertyUpdated = true;
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorUInt8( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        auto v = propertyModel.GetValue<uint8>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U8, &v ) )
        {
            propertyModel.SetValue( v );
            propertyUpdated = true;
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorUInt16( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        auto v = propertyModel.GetValue<uint16>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U16, &v ) )
        {
            propertyModel.SetValue( v );
            propertyUpdated = true;
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorUInt32( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        auto v = propertyModel.GetValue<uint32>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U32, &v ) )
        {
            propertyModel.SetValue( v );
            propertyUpdated = true;
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorUInt64( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        auto v = propertyModel.GetValue<uint64>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U64, &v ) )
        {
            propertyModel.SetValue( v );
            propertyUpdated = true;
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    //-------------------------------------------------------------------------

    bool CreateEditorFloat( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float v = propertyModel.GetValue<float>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &v ) )
        {
            propertyModel.SetValue( v );
            propertyUpdated = true;
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorDouble( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        double v = propertyModel.GetValue<double>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputDouble( g_emptyLabel, &v ) )
        {
            propertyModel.SetValue( v );
            propertyUpdated = true;
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorColor( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        Color c = propertyModel.GetValue<Color>();
        ImVec4 v = c.ToFloat4();

        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::ColorEdit4( g_emptyLabel, &v.x ) )
        {
            propertyModel.SetValue( Color( v ) );
            propertyUpdated = true;
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    //-------------------------------------------------------------------------

    bool CreateEditorMicroseconds( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float v = propertyModel.GetValue<Microseconds>().ToFloat();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &v ) )
        {
            propertyModel.SetValue( Microseconds( v ) );
            propertyUpdated = true;
        }
        ImGui::PopID();

        //-------------------------------------------------------------------------

        return propertyUpdated;
    }

    bool CreateEditorMilliseconds( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float v = propertyModel.GetValue<Milliseconds>().ToFloat();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &v ) )
        {
            propertyModel.SetValue( Milliseconds( v ) );
            propertyUpdated = true;
        }
        ImGui::PopID();

        //-------------------------------------------------------------------------

        return propertyUpdated;
    }

    bool CreateEditorSeconds( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float v = propertyModel.GetValue<Seconds>().ToFloat();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &v ) )
        {
            propertyModel.SetValue( Seconds( v ) );
            propertyUpdated = true;
        }
        ImGui::PopID();

        //-------------------------------------------------------------------------

        return propertyUpdated;
    }

    bool CreateEditorPercentage( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float v = propertyModel.GetValue<Percentage>().ToFloat();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &v ) )
        {
            propertyModel.SetValue( Percentage( v ) );
            propertyUpdated = true;
        }
        ImGui::PopID();

        //-------------------------------------------------------------------------

        return propertyUpdated;
    }

    bool CreateEditorDegrees( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;

        //-------------------------------------------------------------------------
        
        float v = propertyModel.GetValue<Degrees>().ToFloat();

        //-------------------------------------------------------------------------

        ImGui::PushID( &propertyModel );
        {
            ImGui::SetNextItemWidth( textAreaWidth );
            if ( ImGui::InputFloat( g_emptyLabel, &v ) )
            {
                propertyModel.SetValue( Degrees( v ) );
                propertyUpdated = true;
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( ImGui::Button( KRG_ICON_COMPRESS "##Clamp" ) )
            {
                propertyModel.SetValue( Degrees( v ).GetClampedToSmallest() );
                propertyUpdated = true;
            }
        }
        ImGui::PopID();

        //-------------------------------------------------------------------------

        return propertyUpdated;
    }

    bool CreateEditorRadians( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;

        //-------------------------------------------------------------------------

        float v = propertyModel.GetValue<Radians>().ToDegrees().ToFloat();

        //-------------------------------------------------------------------------

        ImGui::PushID( &propertyModel );
        {
            ImGui::SetNextItemWidth( textAreaWidth );
            if ( ImGui::InputFloat( g_emptyLabel, &v ) )
            {
                propertyModel.SetValue( Radians( Degrees( v ) ) );
                propertyUpdated = true;
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( ImGui::Button( KRG_ICON_COMPRESS "##Clamp" ) )
            {
                propertyModel.SetValue( Radians( Degrees( v ) ).GetClampedToSmallest() );
                propertyUpdated = true;
            }
        }
        ImGui::PopID();

        //-------------------------------------------------------------------------

        return propertyUpdated;
    }

    //-------------------------------------------------------------------------

    bool CreateEditorFloat2( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        Float2 v = propertyModel.GetValue<Float2>();
        if ( ImGuiX::InputFloat2( v ) )
        {
            propertyModel.SetValue( Float2( v ) );
            propertyUpdated = true;
        }

        return propertyUpdated;
    }

    bool CreateEditorFloat3( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        Float3 v = propertyModel.GetValue<Float3>();
        if ( ImGuiX::InputFloat3( v ) )
        {
            propertyModel.SetValue( Float3( v ) );
            propertyUpdated = true;
        }

        return propertyUpdated;
    }

    bool CreateEditorFloat4( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        Float4 v = propertyModel.GetValue<Float4>();
        if ( ImGuiX::InputFloat4( v ) )
        {
            propertyModel.SetValue( Float4( v ) );
            propertyUpdated = true;
        }

        return propertyUpdated;
    }

    bool CreateEditorVector( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        Float4 v = propertyModel.GetValue<Vector>();
        if ( ImGuiX::InputFloat4( v ) )
        {
            propertyModel.SetValue( Vector( v ) );
            propertyUpdated = true;
        }

        return propertyUpdated;
    }

    bool CreateEditorEulerAngles( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        Float3 degrees = propertyModel.GetValue<EulerAngles>().GetAsDegrees();
        if ( ImGuiX::InputFloat3( degrees ) )
        {
            EulerAngles const newRotation( Degrees( degrees.m_x ).GetClampedToSmallest(), Degrees( degrees.m_y ).GetClampedToSmallest(), Degrees( degrees.m_z ).GetClampedToSmallest() );
            propertyModel.SetValue( newRotation );
            propertyUpdated = true;
        }

        return propertyUpdated;
    }

    bool CreateEditorQuaternion( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonWidth = 20;
        float const inputWidth = cellContentWidth - buttonWidth - itemSpacing;

        //-------------------------------------------------------------------------

        static Float3 degrees( 0 ); // This should be okay since we should never have more than one of these builders open at one time
        Quaternion const quat = propertyModel.GetValue<Quaternion>();

        //-------------------------------------------------------------------------

        ImGui::PushID( &propertyModel );
        {
            Float4 quatValues = quat.ToFloat4();
            ImGuiX::InputFloat4( quatValues, inputWidth, true );

            //-------------------------------------------------------------------------

            ImGui::SameLine( 0, itemSpacing );
            if ( ImGui::Button( KRG_ICON_WRENCH "##Construct" ) )
            {
                // Set the initial values to the best approximation of the current quat
                degrees = quat.ToEulerAngles().ToFloat3();
                ImGui::OpenPopup( "RotationBuilder" );
            }

            // Rotation Builder
            //-------------------------------------------------------------------------

            if ( ImGui::BeginPopupModal( "RotationBuilder", nullptr, ImGuiWindowFlags_AlwaysAutoResize ) )
            {
                ImGuiX::InputFloat3( degrees );

                // Set new rotation
                if ( ImGui::Button( "Apply", ImVec2( 143, 0 ) ) )
                {
                    EulerAngles const newRotation( Degrees( degrees.m_x ).GetClampedToSmallest(), Degrees( degrees.m_y ).GetClampedToSmallest(), Degrees( degrees.m_z ).GetClampedToSmallest() );
                    propertyModel.SetValue( Quaternion( newRotation ) );
                    propertyUpdated = true;
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
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorTransform( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        union TransformData
        {
            TransformData() {}

            Float3  m_float3[3];
            float   m_floats[9];
        };

        TransformData transformData;
        bool transformUpdated = false;

        Conversion::StringToFloatArray( propertyModel.GetStringValue(), 9, transformData.m_floats );

        //-------------------------------------------------------------------------

        ImGui::PushID( &propertyModel );
        {
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
                    if ( ImGuiX::InputFloat3( transformData.m_float3[1] ) )
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
                    if ( ImGuiX::InputFloat3( transformData.m_float3[0] ) )
                    {
                        Float3& eulerAngles = transformData.m_float3[0];
                        eulerAngles.m_x = Degrees( eulerAngles.m_x ).GetClampedToSmallest().ToFloat();
                        eulerAngles.m_y = Degrees( eulerAngles.m_y ).GetClampedToSmallest().ToFloat();
                        eulerAngles.m_z = Degrees( eulerAngles.m_z ).GetClampedToSmallest().ToFloat();
                        transformUpdated = true;
                    }
                }

                ImGui::TableNextRow();
                {
                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Scl" );

                    ImGui::TableNextColumn();
                    if ( ImGuiX::InputFloat3( transformData.m_float3[2] ) )
                    {
                        transformUpdated = true;
                    }
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();
        }
        ImGui::PopID();

        //-------------------------------------------------------------------------

        if ( transformUpdated )
        {
            String valueString;
            Conversion::FloatArrayToString( transformData.m_floats, 9, valueString );
            propertyModel.SetStringValue( valueString );
        }

        return transformUpdated;
    }

    //-------------------------------------------------------------------------

    bool CreateEditorString( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        char buffer[256];
        KRG_ASSERT( propertyModel.GetStringValue().length() < 256 );
        strcpy( buffer, propertyModel.GetStringValue().c_str() );

        ImGui::PushID( &propertyModel );
        {
            ImGui::SetNextItemWidth( -1 );
            if ( ImGui::InputText( g_emptyLabel, buffer, 256 ) )
            {
                propertyModel.SetValue<String>( buffer );
                propertyUpdated = true;
            }
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorStringID( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const childWindowWidth = 86;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const textAreaWidth = cellContentWidth - childWindowWidth - itemSpacing;

        //-------------------------------------------------------------------------

        char buffer[256];
        KRG_ASSERT( propertyModel.GetStringValue().length() < 256 );
        strcpy( buffer, propertyModel.GetStringValue().c_str() );

        //-------------------------------------------------------------------------

        ImGui::PushID( &propertyModel );
        {
            ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
            ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 2 ) );
            ImGui::BeginChild( "IDLabel", ImVec2( childWindowWidth, 18 ), true, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
            {
                ImGui::TextColored( Colors::LightGreen.ToFloat4(), "%u", StringID( propertyModel.GetStringValue() ).GetID() );
            }
            ImGui::EndChild();
            ImGui::PopStyleVar( 2 );

            //-------------------------------------------------------------------------

            ImGui::SetNextItemWidth( textAreaWidth );
            ImGui::SameLine( 0, itemSpacing );
            if ( ImGui::InputText( g_emptyLabel, buffer, 256 ) )
            {
                StringID const newID( buffer );
                propertyModel.SetValue( newID );
                propertyUpdated = true;
            }
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorUUID( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;

        ImGui::PushID( &propertyModel );
        {
            ImGui::SetNextItemWidth( textAreaWidth );
            ImGui::InputText( g_emptyLabel, const_cast<char*>( propertyModel.GetStringValue().c_str() ), propertyModel.GetStringValue().length(), ImGuiInputTextFlags_ReadOnly );

            ImGui::SameLine( 0, itemSpacing );
            if ( ImGui::Button( KRG_ICON_REFRESH "##Generate" ) )
            {
                propertyModel.SetValue( UUID::GenerateID() );
                propertyUpdated = true;
            }
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    //-------------------------------------------------------------------------

    bool CreateEditorDataPath( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float const fullCellPaddingWidth = ( ImGui::GetStyle().CellPadding.x * 2 );
        float const availableWidth = ImGui::GetContentRegionAvail().x + fullCellPaddingWidth;
        float const buttonWidth = 21;
        float const textWidth = availableWidth - buttonWidth - fullCellPaddingWidth - 4;

        ImGui::PushID( &propertyModel );
        {
            ImGui::SetNextItemWidth( textWidth );
            ImGui::InputText( g_emptyLabel, const_cast<char*>( propertyModel.GetStringValue().c_str() ), propertyModel.GetStringValue().length(), ImGuiInputTextFlags_ReadOnly );

            ImGui::SameLine( availableWidth - buttonWidth );
            if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick" ) )
            {
                DataPath const pickedDataPath = DataFilePicker::PickFile( ctx.m_sourceDataPath );
                propertyModel.SetValue<DataPath>( pickedDataPath );
                propertyUpdated = true;
            }
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorResourceTypeID( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        ImGui::PushID( &propertyModel );
        {
            float const cellContentWidth = ImGui::GetContentRegionAvail().x;
            float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
            float const childWindowWidth = 86;
            float const textAreaWidth = cellContentWidth - childWindowWidth - itemSpacing;

            //-------------------------------------------------------------------------

            ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
            ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 2 ) );
            ImGui::BeginChild( "IDLabel", ImVec2( childWindowWidth, 18 ), true, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
            {
                ImGui::TextColored( Colors::LightPink.ToFloat4(), "%u", ResourceTypeID( propertyModel.GetStringValue().c_str() ).m_ID );
            }
            ImGui::EndChild();
            ImGui::PopStyleVar( 2 );

            //-------------------------------------------------------------------------

            ImGui::SetNextItemWidth( textAreaWidth );
            ImGui::SameLine( 0, itemSpacing  );
            if ( ImGui::BeginCombo( g_emptyLabel, propertyModel.GetStringValue().c_str() ) )
            {
                auto AddComboItem = [&propertyUpdated, &propertyModel] ( ResourceTypeID ID )
                {
                    char resourceTypeStr[5];
                    ID.GetString( resourceTypeStr );

                    bool const isSelected = ( propertyModel.GetStringValue() == resourceTypeStr );
                    if ( ImGui::Selectable( resourceTypeStr, isSelected ) )
                    {
                        propertyModel.SetValue( ID );
                        propertyUpdated = true;
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
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorResourceID( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const childWindowWidth = 50;

        //-------------------------------------------------------------------------

        char resourceTypeStr[5] = "NONE";
        if ( !propertyModel.GetStringValue().empty() )
        {
            ResourceTypeID const resourceTypeID = ResourceID( propertyModel.GetStringValue() ).GetResourceTypeID();
            resourceTypeID.GetString( resourceTypeStr );
        }

        //-------------------------------------------------------------------------

        ImGui::PushID( &propertyModel );
        {
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
            ImGui::InputText( g_emptyLabel, const_cast<char*>( propertyModel.GetStringValue().c_str() ), propertyModel.GetStringValue().length(), ImGuiInputTextFlags_ReadOnly );

            ImGui::SameLine( 0, itemSpacing );
            if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick" ) )
            {
                TVector<ResourceTypeID> allowedResourceTypes;
                for ( auto const& registeredResourceType : ctx.m_typeRegistry.GetRegisteredResourceTypes() )
                {
                    allowedResourceTypes.emplace_back( registeredResourceType.second );
                }

                DataPath const pickedDataPath = DataFilePicker::PickResourceFile( ctx.m_sourceDataPath, allowedResourceTypes );
                propertyModel.SetValue<ResourceID>( pickedDataPath.IsValid() ? ResourceID( pickedDataPath ) : ResourceID() );
                propertyUpdated = true;
            }
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    bool CreateEditorResourcePtr( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        float const buttonAreaWidth = 21;
        float const childWindowWidth = 50;

        //-------------------------------------------------------------------------

        char resourceTypeStr[5] = { 0 };
        ResourceTypeID const resourceTypeID = ctx.m_typeRegistry.GetResourceTypeIDForType( propertyModel.GetTemplatedArgumentTypeID() );
        resourceTypeID.GetString( resourceTypeStr );

        //-------------------------------------------------------------------------

        ImGui::PushID( &propertyModel );
        {
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
            ImGui::InputText( g_emptyLabel, const_cast<char*>( propertyModel.GetStringValue().c_str() ), propertyModel.GetStringValue().length(), ImGuiInputTextFlags_ReadOnly );

            ImGui::SameLine( 0, itemSpacing );
            if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick" ) )
            {
                DataPath const pickedDataPath = DataFilePicker::PickResourceFile( ctx.m_sourceDataPath, resourceTypeID );
                propertyModel.SetValue<Resource::ResourcePtr>( pickedDataPath.IsValid() ? Resource::ResourcePtr( pickedDataPath ) : Resource::ResourcePtr() );
                propertyUpdated = true;
            }
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    //-------------------------------------------------------------------------

    bool CreateEditorEnum( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;

        ImGui::SetNextItemWidth( -1 );
        ImGui::PushID( &propertyModel );
        if ( ImGui::BeginCombo( g_emptyLabel, propertyModel.GetStringValue().c_str() ) )
        {
            TVector<char const*> const enumLabels = propertyModel.GetEnumValues();
            int32 const numEnumValues = propertyModel.GetNumEnumValues();
            for ( int32 i = 0; i < numEnumValues; i++ )
            {
                bool const isSelected = propertyModel.GetStringValue() == enumLabels[i];
                if ( ImGui::Selectable( enumLabels[i], isSelected ) )
                {
                    propertyModel.SetStringValue( enumLabels[i] );
                    propertyUpdated = true;
                }

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if ( isSelected )
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }
        ImGui::PopID();

        return propertyUpdated;
    }

    //------------------------------------------------------------------------- 

    bool CreateEditorGenericFlags( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;
        BitFlags flags = propertyModel.GetValue<BitFlags>();

        bool v[32];
        for ( uint8 i = 0u; i < 32; i++ )
        {
            v[i] = flags.IsFlagSet( i );
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
                        flags.SetFlag( flagIdx, v[flagIdx] );
                        propertyModel.SetValue( flags );
                        propertyUpdated = true;
                    }
                    ImGui::PopID();
                }

                ImGui::TableNextColumn();
                ImGui::Text( rowLabels[i] );
            }

            ImGui::EndTable();
        }

        //-------------------------------------------------------------------------

        return propertyUpdated;
    }

    bool CreateEditorSpecificFlags( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        bool propertyUpdated = false;
        BitFlags flags = propertyModel.GetValue<BitFlags>();

        bool v[32];
        for ( uint8 i = 0u; i < 32; i++ )
        {
            v[i] = flags.IsFlagSet( i );
        }

        //-------------------------------------------------------------------------

        // Get enum type for specific flags
        TypeID const enumTypeID = propertyModel.GetTemplatedArgumentTypeID();
        EnumInfo const* pEnumInfo = ctx.m_typeRegistry.GetEnumInfo( enumTypeID );
        KRG_ASSERT( pEnumInfo != nullptr );

        //-------------------------------------------------------------------------

        ImGui::PushID( &propertyModel );

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
                    flags.SetFlag( (uint8) flagValue, v[flagValue] );
                    propertyModel.SetValue( flags );
                    propertyUpdated = true;
                }
            }

            ImGui::EndTable();
        }

        ImGui::PopID();

        //-------------------------------------------------------------------------

        return propertyUpdated;
    }
}

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    bool CreatePropertyEditor( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        KRG_ASSERT( !propertyModel.IsStructure() && !propertyModel.IsArray() );

        if ( propertyModel.IsEnum() )
        {
            return CreateEditorEnum( ctx, propertyModel );
        }
        else
        {
            CoreTypes const coreType = GetCoreType( propertyModel.GetTypeID() );

            switch ( coreType )
            {
                case CoreTypes::Bool:
                {
                    return CreateEditorBool( ctx, propertyModel );
                }
                break;

                case CoreTypes::Int8:
                {
                    return CreateEditorInt8( ctx, propertyModel );
                }
                break;

                case CoreTypes::Int16:
                {
                    return CreateEditorInt16( ctx, propertyModel );
                }
                break;

                case CoreTypes::Int32:
                {
                    return CreateEditorInt32( ctx, propertyModel );
                }
                break;

                case CoreTypes::Int64:
                {
                    return CreateEditorInt64( ctx, propertyModel );
                }
                break;

                case CoreTypes::Uint8:
                {
                    return CreateEditorUInt8( ctx, propertyModel );
                }
                break;

                case CoreTypes::Uint16:
                {
                    return CreateEditorUInt16( ctx, propertyModel );
                }
                break;

                case CoreTypes::Uint32:
                {
                    return CreateEditorUInt32( ctx, propertyModel );
                }
                break;

                case CoreTypes::Uint64:
                {
                    return CreateEditorUInt64( ctx, propertyModel );
                }
                break;

                case CoreTypes::Float:
                {
                    return CreateEditorFloat( ctx, propertyModel );
                }
                break;

                case CoreTypes::Double:
                {
                    return CreateEditorDouble( ctx, propertyModel );
                }
                break;

                case CoreTypes::UUID:
                {
                    return CreateEditorUUID( ctx, propertyModel );
                }
                break;

                case CoreTypes::StringID:
                {
                    return CreateEditorStringID( ctx, propertyModel );
                }
                break;

                case CoreTypes::String:
                {
                    return CreateEditorString( ctx, propertyModel );
                }
                break;

                case CoreTypes::Color:
                {
                    return CreateEditorColor( ctx, propertyModel );
                }
                break;

                case CoreTypes::Float2:
                {
                    return CreateEditorFloat2( ctx, propertyModel );
                }
                break;

                case CoreTypes::Float3:
                {
                    return CreateEditorFloat3( ctx, propertyModel );
                }
                break;

                case CoreTypes::Float4:
                {
                    return CreateEditorFloat4( ctx, propertyModel );
                }
                break;

                case CoreTypes::Vector:
                {
                    return CreateEditorVector( ctx, propertyModel );
                }
                break;

                case CoreTypes::Quaternion:
                {
                    return CreateEditorQuaternion( ctx, propertyModel );
                }
                break;

                case CoreTypes::Matrix:
                case CoreTypes::Transform:
                {
                    return CreateEditorTransform( ctx, propertyModel );
                }
                break;

                case CoreTypes::Microseconds:
                {
                    return CreateEditorMicroseconds( ctx, propertyModel );
                }
                break;

                case CoreTypes::Milliseconds:
                {
                    return CreateEditorMilliseconds( ctx, propertyModel );
                }
                break;

                case CoreTypes::Seconds:
                {
                    return CreateEditorSeconds( ctx, propertyModel );
                }
                break;

                case CoreTypes::Percentage:
                {
                    return CreateEditorPercentage( ctx, propertyModel );
                }
                break;

                case CoreTypes::Degrees:
                {
                    return CreateEditorDegrees( ctx, propertyModel );
                }
                break;

                case CoreTypes::Radians:
                {
                    return CreateEditorRadians( ctx, propertyModel );
                }
                break;

                case CoreTypes::EulerAngles:
                {
                    return CreateEditorEulerAngles( ctx, propertyModel );
                }
                break;

                case CoreTypes::DataPath:
                {
                    return CreateEditorDataPath( ctx, propertyModel );
                }
                break;

                case CoreTypes::ResourceID:
                {
                    return CreateEditorResourceID( ctx, propertyModel );
                }
                break;

                case CoreTypes::ResourceTypeID:
                {
                    return CreateEditorResourceTypeID( ctx, propertyModel );
                }
                break;

                case CoreTypes::TResourcePtr:
                {
                    return CreateEditorResourcePtr( ctx, propertyModel );
                }
                break;

                case CoreTypes::BitFlags:
                {
                    return CreateEditorGenericFlags( ctx, propertyModel );
                }
                break;

                case CoreTypes::TBitFlags:
                {
                    return CreateEditorSpecificFlags( ctx, propertyModel );
                }
                break;
            }
        }

        //-------------------------------------------------------------------------

        KRG_UNREACHABLE_CODE();
        return false;
    }
}