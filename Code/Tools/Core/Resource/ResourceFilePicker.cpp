#include "ResourceFilePicker.h"
#include "ResourceDatabase.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    ResourceFilePicker::PickerOption::PickerOption( ResourceID const& resourceID )
        : m_resourceID( resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );

        ResourcePath const& resourcePath = resourceID.GetResourcePath();
        m_filename = resourcePath.GetFileNameWithoutExtension();
        m_parentFolder = resourcePath.GetParentDirectory();
    }

    //-------------------------------------------------------------------------

    ResourceFilePicker::ResourceFilePicker( ResourceDatabase const& database )
        : m_database( database )
    {
        Memory::MemsetZero( m_filterBuffer, 256 * sizeof( char ) );
    }

    FileSystem::Path const& ResourceFilePicker::GetRawResourceDirectoryPath() const
    {
        return m_database.GetRawResourceDirectoryPath();
    }

    void ResourceFilePicker::RefreshResourceList( ResourceTypeID resourceTypeID )
    {
        m_knownResourceIDs.clear();

        if ( resourceTypeID.IsValid() )
        {
            for ( auto const& resourceRecord : m_database.GetAllResourcesOfType( resourceTypeID ) )
            {
                m_knownResourceIDs.emplace_back( resourceRecord->m_resourcePath );
            }
        }
        else
        {
            for ( auto const& resourceListPair : m_database.GetAllResources() )
            {
                for ( auto const& resourceRecord : resourceListPair.second )
                {
                    m_knownResourceIDs.emplace_back( resourceRecord->m_resourcePath );
                }
            }
        }

        //-------------------------------------------------------------------------

        if ( m_filterBuffer[0] == 0 )
        {
            m_filteredResourceIDs = m_knownResourceIDs;
        }
        else
        {
            m_filteredResourceIDs.clear();
            for ( auto const& resourceID : m_knownResourceIDs )
            {
                String lowercasePath = resourceID.m_resourceID.GetResourcePath().GetString();
                lowercasePath.make_lower();

                bool passesFilter = true;
                char* token = strtok( m_filterBuffer, " " );
                while ( token )
                {
                    if ( lowercasePath.find( token ) == String::npos )
                    {
                        passesFilter = false;
                        break;
                    }

                    token = strtok( NULL, " " );
                }

                if ( passesFilter )
                {
                    m_filteredResourceIDs.emplace_back( resourceID );
                }
            }
        }
    }

    bool ResourceFilePicker::DrawPicker( ResourceTypeID resourceTypeID, ResourceID const* pResourceID )
    {
        KRG_ASSERT( pResourceID != nullptr );
        if ( resourceTypeID.IsValid() && pResourceID->IsValid() )
        {
            KRG_ASSERT( pResourceID->GetResourceTypeID() == resourceTypeID );
        }
        bool valueUpdated = false;

        //-------------------------------------------------------------------------

        float const contentRegionAvailable = ImGui::GetContentRegionAvail().x;
        constexpr float const resourceTypeWindowWidth = 28;

        ImGui::PushID( pResourceID );
        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 2, 2 ) );
        ImGui::BeginChild( "IDLabel", ImVec2( resourceTypeWindowWidth, 18 ), true, ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
        {
            ImGuiX::ScopedFont const sf( ImGuiX::Font::TinyBold );
            if ( pResourceID->IsValid() )
            {
                ImVec2 const textSize = ImGui::CalcTextSize( pResourceID->GetResourceTypeID().ToString().c_str());
                ImGui::SameLine( 0, ( resourceTypeWindowWidth - textSize.x ) / 2 );
                ImGui::AlignTextToFramePadding();
                ImGui::TextColored( Colors::LightPink.ToFloat4(), pResourceID->GetResourceTypeID().ToString().c_str() );
            }
            else
            {
                ImGui::AlignTextToFramePadding();
                ImGui::TextColored( Colors::DarkGray.ToFloat4(), "EMPTY" );
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleVar( 2 );

        //-------------------------------------------------------------------------

        constexpr float const buttonWidth = 22;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;

        ImGui::SameLine( 0, itemSpacing );
        ImGui::SetNextItemWidth( contentRegionAvailable - ( itemSpacing * 3 ) - ( buttonWidth * 2 ) - resourceTypeWindowWidth );
        ImGui::InputText( "##DataPath", const_cast<char*>( pResourceID->GetResourcePath().c_str() ), pResourceID->GetResourcePath().GetString().length(), ImGuiInputTextFlags_ReadOnly );

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick", ImVec2( buttonWidth, 0 ) ) )
        {
            ImGui::OpenPopup( "Resource Picker" );
            m_filterBuffer[0] = 0;
            m_initializeFocus = true;
            RefreshResourceList( resourceTypeID );
            m_selectedID = *pResourceID;
        }

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_TIMES_CIRCLE "##Clear", ImVec2( buttonWidth, 0 ) ) )
        {
            m_selectedID.Clear();
            valueUpdated = true;
        }

        ImGui::PopID();

        //-------------------------------------------------------------------------

        valueUpdated |= DrawDialog( resourceTypeID, pResourceID );

        return valueUpdated;
    }

    bool ResourceFilePicker::DrawDialog( ResourceTypeID resourceTypeID, ResourceID const* pResourceID )
    {
        ImGui::PushID( pResourceID );

        bool selectionMade = false;
        bool isDialogOpen = true;
        ImGui::SetNextWindowSize( ImVec2( 1000, 400 ), ImGuiCond_FirstUseEver );
        ImGui::SetNextWindowSizeConstraints( ImVec2( 400, 400 ), ImVec2( FLT_MAX, FLT_MAX ) );
        if ( ImGui::BeginPopupModal( "Resource Picker", &isDialogOpen, ImGuiWindowFlags_NoSavedSettings ) )
        {
            ImVec2 const contentRegionAvailable = ImGui::GetContentRegionAvail();

            // Draw Filter
            //-------------------------------------------------------------------------

            bool filterUpdated = false;

            ImGui::SetNextItemWidth( contentRegionAvailable.x - ImGui::GetStyle().WindowPadding.x - 22 );
            InlineString filterCopy( m_filterBuffer );
            
            if ( m_initializeFocus )
            {
                ImGui::SetKeyboardFocusHere();
                m_initializeFocus = false;
            }

            if ( ImGui::InputText( "##Filter", filterCopy.data(), 256) )
            {
                if ( strcmp( filterCopy.data(), m_filterBuffer) != 0 )
                {
                    strcpy_s( m_filterBuffer, 256, filterCopy.data() );

                    // Convert buffer to lower case
                    int32 i = 0;
                    while ( i < 256 && m_filterBuffer[i] != 0 )
                    {
                        m_filterBuffer[i] = eastl::CharToLower( m_filterBuffer[i] );
                        i++;
                    }

                    filterUpdated = true;
                }
            }

            ImGui::SameLine();
            if ( ImGui::Button( KRG_ICON_TIMES_CIRCLE "##Clear Filter", ImVec2( 22, 0 ) ) )
            {
                m_filterBuffer[0] = 0;
                filterUpdated = true;
            }

            // Update filter options
            //-------------------------------------------------------------------------

            if ( filterUpdated )
            {
                RefreshResourceList( resourceTypeID );
            }

            // Draw results
            //-------------------------------------------------------------------------

            float const tableHeight = contentRegionAvailable.y - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y;
            ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Style::s_itemColorMedium.Value );
            if ( ImGui::BeginTable( "Resource List", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2( contentRegionAvailable.x, tableHeight ) ) )
            {
                ImGui::TableSetupColumn( "File", ImGuiTableColumnFlags_WidthStretch, 0.65f );
                ImGui::TableSetupColumn( "Path", ImGuiTableColumnFlags_WidthStretch );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                ImGuiListClipper clipper;
                clipper.Begin( (int32) m_filteredResourceIDs.size() );
                while ( clipper.Step() )
                {
                    for ( int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++ )
                    {
                        ImGui::TableNextRow();

                        ImGui::TableNextColumn();
                        bool isSelected = ( m_selectedID == m_filteredResourceIDs[i].m_resourceID );
                        if ( ImGui::Selectable( m_filteredResourceIDs[i].m_filename.c_str(), &isSelected, ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_SpanAllColumns ) )
                        {
                            m_selectedID = m_filteredResourceIDs[i].m_resourceID;

                            if ( ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
                            {
                                selectionMade = true;
                                ImGui::CloseCurrentPopup();
                            }
                        }

                        ImGui::TableNextColumn();
                        ImGui::Text( m_filteredResourceIDs[i].m_parentFolder.c_str() );
                    }
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleColor();

            ImGui::EndPopup();
        }
        ImGui::PopID();

        //-------------------------------------------------------------------------

        if ( !isDialogOpen )
        {
            m_selectedID = *pResourceID;
        }

        //-------------------------------------------------------------------------

        return selectionMade;
    }
}