#include "PropertyEditors.h"
#include "Applications/Editor/Common/DataFilePicker.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "System/DevTools/ImguiX.h"
#include "System/DevTools/CommonWidgets/NumericEditors.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    constexpr static char const* const g_emptyLabel = "##";

    //-------------------------------------------------------------------------

    struct [[nodiscard]] ScopedChangeNotifier
    {
        ScopedChangeNotifier( Context& ctx, PropertyInstanceModel& propertyModel )
            : m_ctx( ctx )
            , m_propertyModel( propertyModel )
        {
            m_ctx.m_preChangeDelegate( m_propertyModel );
        }

        ~ScopedChangeNotifier()
        {
            m_ctx.m_postChangeDelegate( m_propertyModel );
        }

        Context&                    m_ctx;
        PropertyInstanceModel&      m_propertyModel;
    };

    //-------------------------------------------------------------------------

    void CreateEditorBool( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        auto v = propertyModel.GetValue<bool>();
        ImGui::PushID( &propertyModel );
        if ( ImGui::Checkbox( g_emptyLabel, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( v );
        }
        ImGui::PopID();
    }

    void CreateEditorInt8( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        auto v = propertyModel.GetValue<int8>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S8, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( v );
        }
        ImGui::PopID();
    }

    void CreateEditorInt16( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        auto v = propertyModel.GetValue<int16>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S16, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( v );
        }
        ImGui::PopID();
    }

    void CreateEditorInt32( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        auto v = propertyModel.GetValue<int32>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S32, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( v );
        }
        ImGui::PopID();
    }

    void CreateEditorInt64( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        auto v = propertyModel.GetValue<int64>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_S64, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( v );
        }
        ImGui::PopID();
    }

    void CreateEditorUInt8( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        auto v = propertyModel.GetValue<uint8>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U8, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( v );
        }
        ImGui::PopID();
    }

    void CreateEditorUInt16( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        auto v = propertyModel.GetValue<uint16>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U16, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( v );
        }
        ImGui::PopID();
    }

    void CreateEditorUInt32( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        auto v = propertyModel.GetValue<uint32>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U32, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( v );
        }
        ImGui::PopID();
    }

    void CreateEditorUInt64( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        auto v = propertyModel.GetValue<uint64>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputScalar( g_emptyLabel, ImGuiDataType_U64, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( v );
        }
        ImGui::PopID();
    }

    //-------------------------------------------------------------------------

    void CreateEditorFloat( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        float v = propertyModel.GetValue<float>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( v );
        }
        ImGui::PopID();
    }

    void CreateEditorDouble( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        double v = propertyModel.GetValue<double>();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputDouble( g_emptyLabel, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( v );
        }
        ImGui::PopID();
    }

    void CreateEditorColor( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        Color c = propertyModel.GetValue<Color>();
        ImVec4 v = c.ToFloat4();

        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::ColorEdit4( g_emptyLabel, &v.x ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( Color( v ) );
        }
        ImGui::PopID();
    }

    //-------------------------------------------------------------------------

    void CreateEditorMicroseconds( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        float v = propertyModel.GetValue<Microseconds>().ToFloat();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( Microseconds( v ) );
        }
        ImGui::PopID();
    }

    void CreateEditorMilliseconds( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        float v = propertyModel.GetValue<Milliseconds>().ToFloat();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( Milliseconds( v ) );
        }
        ImGui::PopID();
    }

    void CreateEditorSeconds( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        float v = propertyModel.GetValue<Seconds>().ToFloat();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( Seconds( v ) );
        }
        ImGui::PopID();
    }

    void CreateEditorPercentage( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        float v = propertyModel.GetValue<Percentage>().ToFloat();
        ImGui::PushID( &propertyModel );
        ImGui::SetNextItemWidth( -1 );
        if ( ImGui::InputFloat( g_emptyLabel, &v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( Percentage( v ) );
        }
        ImGui::PopID();
    }

    void CreateEditorDegrees( Context& ctx, PropertyInstanceModel& propertyModel )
    {
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
                ScopedChangeNotifier notifier( ctx, propertyModel );
                propertyModel.SetValue( Degrees( v ) );
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( ImGui::Button( KRG_ICON_COMPRESS "##Clamp" ) )
            {
                ScopedChangeNotifier notifier( ctx, propertyModel );
                propertyModel.SetValue( Degrees( v ).GetClampedToSmallest() );
            }
        }
        ImGui::PopID();
    }

    void CreateEditorRadians( Context& ctx, PropertyInstanceModel& propertyModel )
    {
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
                ScopedChangeNotifier notifier( ctx, propertyModel );
                propertyModel.SetValue( Radians( Degrees( v ) ) );
            }

            ImGui::SameLine( 0, itemSpacing );
            if ( ImGui::Button( KRG_ICON_COMPRESS "##Clamp" ) )
            {
                ScopedChangeNotifier notifier( ctx, propertyModel );
                propertyModel.SetValue( Radians( Degrees( v ) ).GetClampedToSmallest() );
            }
        }
        ImGui::PopID();
    }

    //-------------------------------------------------------------------------

    void CreateEditorFloat2( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        Float2 v = propertyModel.GetValue<Float2>();
        if ( ImGuiX::InputFloat2( v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( Float2( v ) );
        }
    }

    void CreateEditorFloat3( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        Float3 v = propertyModel.GetValue<Float3>();
        if ( ImGuiX::InputFloat3( v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( Float3( v ) );
        }
    }

    void CreateEditorFloat4( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        Float4 v = propertyModel.GetValue<Float4>();
        if ( ImGuiX::InputFloat4( v ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            propertyModel.SetValue( Float4( v ) );
        }
    }

    void CreateEditorVector( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        Float4 v = propertyModel.GetValue<Vector>();
        if ( ImGuiX::InputFloat4( v ) )
        {
            propertyModel.SetValue( Vector( v ) );
        }
    }

    void CreateEditorEulerAngles( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        Float3 degrees = propertyModel.GetValue<EulerAngles>().GetAsDegrees();
        if ( ImGuiX::InputFloat3( degrees ) )
        {
            ScopedChangeNotifier notifier( ctx, propertyModel );
            EulerAngles const newRotation( Degrees( degrees.m_x ).GetClampedToSmallest(), Degrees( degrees.m_y ).GetClampedToSmallest(), Degrees( degrees.m_z ).GetClampedToSmallest() );
            propertyModel.SetValue( newRotation );
        }
    }

    void CreateEditorQuaternion( Context& ctx, PropertyInstanceModel& propertyModel )
    {
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
                    ScopedChangeNotifier notifier( ctx, propertyModel );
                    EulerAngles const newRotation( Degrees( degrees.m_x ).GetClampedToSmallest(), Degrees( degrees.m_y ).GetClampedToSmallest(), Degrees( degrees.m_z ).GetClampedToSmallest() );
                    propertyModel.SetValue( Quaternion( newRotation ) );
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
    }

    void CreateEditorTransform( Context& ctx, PropertyInstanceModel& propertyModel )
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
            ScopedChangeNotifier notifier( ctx, propertyModel );
            String valueString;
            Conversion::FloatArrayToString( transformData.m_floats, 9, valueString );
            propertyModel.SetStringValue( valueString );
        }
    }

    //-------------------------------------------------------------------------

    void CreateEditorString( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        char buffer[256];
        KRG_ASSERT( propertyModel.GetStringValue().length() < 256 );
        strcpy( buffer, propertyModel.GetStringValue().c_str() );

        ImGui::PushID( &propertyModel );
        {
            ImGui::SetNextItemWidth( -1 );
            if ( ImGui::InputText( g_emptyLabel, buffer, 256 ) )
            {
                ScopedChangeNotifier notifier( ctx, propertyModel );
                propertyModel.SetValue<String>( buffer );
            }
        }
        ImGui::PopID();
    }

    void CreateEditorStringID( Context& ctx, PropertyInstanceModel& propertyModel )
    {
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
                ScopedChangeNotifier notifier( ctx, propertyModel );
                StringID const newID( buffer );
                propertyModel.SetValue( newID );
            }
        }
        ImGui::PopID();
    }

    void CreateEditorUUID( Context& ctx, PropertyInstanceModel& propertyModel )
    {
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
                ScopedChangeNotifier notifier( ctx, propertyModel );
                propertyModel.SetValue( UUID::GenerateID() );
            }
        }
        ImGui::PopID();
    }

    //-------------------------------------------------------------------------

    void CreateEditorDataPath( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        float const fullCellPaddingWidth = ( ImGui::GetStyle().CellPadding.x * 2 );
        float const availableWidth = ImGui::GetContentRegionAvail().x + fullCellPaddingWidth;
        float const buttonWidth = 21;
        float const textWidth = availableWidth - buttonWidth - fullCellPaddingWidth - 4;

        //-------------------------------------------------------------------------

        DataPath const dataPath = propertyModel.GetValue<DataPath>();
        String dataPathStr = dataPath.ToString();

        //-------------------------------------------------------------------------

        ImGui::PushID( &propertyModel );
        {
            ImGui::SetNextItemWidth( textWidth );
            ImGui::InputText( g_emptyLabel, dataPathStr.data(), dataPathStr.length(), ImGuiInputTextFlags_ReadOnly );

            ImGui::SameLine( availableWidth - buttonWidth );
            if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick" ) )
            {
                DataPath pickedDataPath;
                if ( DataFilePicker::PickFile( ctx.m_sourceDataPath, pickedDataPath ) )
                {
                    ScopedChangeNotifier notifier( ctx, propertyModel );
                    propertyModel.SetValue<DataPath>( pickedDataPath );
                }
            }
        }
        ImGui::PopID();
    }

    void CreateEditorResourceTypeID( Context& ctx, PropertyInstanceModel& propertyModel )
    {
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
                auto AddComboItem = [&ctx, &propertyModel] ( ResourceTypeID ID )
                {
                    char resourceTypeStr[5];
                    ID.GetString( resourceTypeStr );

                    bool const isSelected = ( propertyModel.GetStringValue() == resourceTypeStr );
                    if ( ImGui::Selectable( resourceTypeStr, isSelected ) )
                    {
                        ScopedChangeNotifier notifier( ctx, propertyModel );
                        propertyModel.SetValue( ID );
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
    }

    void CreateEditorResourceID( Context& ctx, PropertyInstanceModel& propertyModel )
    {
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

                DataPath pickedDataPath;
                if ( DataFilePicker::PickResourceFile( ctx.m_sourceDataPath, allowedResourceTypes, pickedDataPath ) )
                {
                    ScopedChangeNotifier notifier( ctx, propertyModel );
                    propertyModel.SetValue<ResourceID>( ResourceID( pickedDataPath ) );
                }
            }
        }
        ImGui::PopID();
    }

    void CreateEditorResourcePtr( Context& ctx, PropertyInstanceModel& propertyModel )
    {
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
                DataPath pickedDataPath;
                if ( DataFilePicker::PickResourceFile( ctx.m_sourceDataPath, resourceTypeID, pickedDataPath ) )
                {
                    ScopedChangeNotifier notifier( ctx, propertyModel );
                    propertyModel.SetValue<Resource::ResourcePtr>( Resource::ResourcePtr( pickedDataPath ) );
                }
            }
        }
        ImGui::PopID();
    }

    //-------------------------------------------------------------------------

    void CreateEditorEnum( Context& ctx, PropertyInstanceModel& propertyModel )
    {
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
                    ScopedChangeNotifier notifier( ctx, propertyModel );
                    propertyModel.SetStringValue( enumLabels[i] );
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
    }

    //------------------------------------------------------------------------- 

    void CreateEditorGenericFlags( Context& ctx, PropertyInstanceModel& propertyModel )
    {
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
                        ScopedChangeNotifier notifier( ctx, propertyModel );
                        flags.SetFlag( flagIdx, v[flagIdx] );
                        propertyModel.SetValue( flags );
                    }
                    ImGui::PopID();
                }

                ImGui::TableNextColumn();
                ImGui::Text( rowLabels[i] );
            }

            ImGui::EndTable();
        }
    }

    void CreateEditorSpecificFlags( Context& ctx, PropertyInstanceModel& propertyModel )
    {
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
                    ScopedChangeNotifier notifier( ctx, propertyModel );
                    flags.SetFlag( (uint8) flagValue, v[flagValue] );
                    propertyModel.SetValue( flags );
                }
            }

            ImGui::EndTable();
        }

        ImGui::PopID();
    }
}

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::PG
{
    void CreatePropertyEditor( Context& ctx, PropertyInstanceModel& propertyModel )
    {
        KRG_ASSERT( !propertyModel.IsStructure() && !propertyModel.IsArray() );

        if ( propertyModel.IsEnum() )
        {
            CreateEditorEnum( ctx, propertyModel );
        }
        else
        {
            CoreTypes const coreType = GetCoreType( propertyModel.GetTypeID() );

            switch ( coreType )
            {
                case CoreTypes::Bool:
                {
                    CreateEditorBool( ctx, propertyModel );
                }
                break;

                case CoreTypes::Int8:
                {
                    CreateEditorInt8( ctx, propertyModel );
                }
                break;

                case CoreTypes::Int16:
                {
                    CreateEditorInt16( ctx, propertyModel );
                }
                break;

                case CoreTypes::Int32:
                {
                    CreateEditorInt32( ctx, propertyModel );
                }
                break;

                case CoreTypes::Int64:
                {
                    CreateEditorInt64( ctx, propertyModel );
                }
                break;

                case CoreTypes::Uint8:
                {
                    CreateEditorUInt8( ctx, propertyModel );
                }
                break;

                case CoreTypes::Uint16:
                {
                    CreateEditorUInt16( ctx, propertyModel );
                }
                break;

                case CoreTypes::Uint32:
                {
                    CreateEditorUInt32( ctx, propertyModel );
                }
                break;

                case CoreTypes::Uint64:
                {
                    CreateEditorUInt64( ctx, propertyModel );
                }
                break;

                case CoreTypes::Float:
                {
                    CreateEditorFloat( ctx, propertyModel );
                }
                break;

                case CoreTypes::Double:
                {
                    CreateEditorDouble( ctx, propertyModel );
                }
                break;

                case CoreTypes::UUID:
                {
                    CreateEditorUUID( ctx, propertyModel );
                }
                break;

                case CoreTypes::StringID:
                {
                    CreateEditorStringID( ctx, propertyModel );
                }
                break;

                case CoreTypes::String:
                {
                    CreateEditorString( ctx, propertyModel );
                }
                break;

                case CoreTypes::Color:
                {
                    CreateEditorColor( ctx, propertyModel );
                }
                break;

                case CoreTypes::Float2:
                {
                    CreateEditorFloat2( ctx, propertyModel );
                }
                break;

                case CoreTypes::Float3:
                {
                    CreateEditorFloat3( ctx, propertyModel );
                }
                break;

                case CoreTypes::Float4:
                {
                    CreateEditorFloat4( ctx, propertyModel );
                }
                break;

                case CoreTypes::Vector:
                {
                    CreateEditorVector( ctx, propertyModel );
                }
                break;

                case CoreTypes::Quaternion:
                {
                    CreateEditorQuaternion( ctx, propertyModel );
                }
                break;

                case CoreTypes::Matrix:
                case CoreTypes::Transform:
                {
                    CreateEditorTransform( ctx, propertyModel );
                }
                break;

                case CoreTypes::Microseconds:
                {
                    CreateEditorMicroseconds( ctx, propertyModel );
                }
                break;

                case CoreTypes::Milliseconds:
                {
                    CreateEditorMilliseconds( ctx, propertyModel );
                }
                break;

                case CoreTypes::Seconds:
                {
                    CreateEditorSeconds( ctx, propertyModel );
                }
                break;

                case CoreTypes::Percentage:
                {
                    CreateEditorPercentage( ctx, propertyModel );
                }
                break;

                case CoreTypes::Degrees:
                {
                    CreateEditorDegrees( ctx, propertyModel );
                }
                break;

                case CoreTypes::Radians:
                {
                    CreateEditorRadians( ctx, propertyModel );
                }
                break;

                case CoreTypes::EulerAngles:
                {
                    CreateEditorEulerAngles( ctx, propertyModel );
                }
                break;

                case CoreTypes::DataPath:
                {
                    CreateEditorDataPath( ctx, propertyModel );
                }
                break;

                case CoreTypes::ResourceID:
                {
                    CreateEditorResourceID( ctx, propertyModel );
                }
                break;

                case CoreTypes::ResourceTypeID:
                {
                    CreateEditorResourceTypeID( ctx, propertyModel );
                }
                break;

                case CoreTypes::TResourcePtr:
                {
                    CreateEditorResourcePtr( ctx, propertyModel );
                }
                break;

                case CoreTypes::BitFlags:
                {
                    CreateEditorGenericFlags( ctx, propertyModel );
                }
                break;

                case CoreTypes::TBitFlags:
                {
                    CreateEditorSpecificFlags( ctx, propertyModel );
                }
                break;
            }
        }
    }
}