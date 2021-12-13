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
        m_databaseUpdateEventBindingID = m_database.OnDatabaseUpdated().Bind( [this] () { RefreshResourceList(); } );
        RefreshResourceList();
    }

    ResourceFilePicker::~ResourceFilePicker()
    {
        if ( m_databaseUpdateEventBindingID.IsValid() )
        {
            m_database.OnDatabaseUpdated().Unbind( m_databaseUpdateEventBindingID );
        }
    }

    void ResourceFilePicker::SetInitialResourceID( ResourceID const& initialID )
    {
        m_resourceID = initialID;
        m_selectedID = initialID;
    }

    void ResourceFilePicker::SetTypeFilter( ResourceTypeID allowedResourceType )
    {
        KRG_ASSERT( allowedResourceType.IsValid() );
        m_allowedResourceTypeID = allowedResourceType;
        RefreshResourceList();
    }

    void ResourceFilePicker::ClearTypeFilter()
    {
        m_allowedResourceTypeID.Clear();
        RefreshResourceList();
    }

    void ResourceFilePicker::RefreshResourceList()
    {
        m_knownResourceIDs.clear();

        if ( m_allowedResourceTypeID.IsValid() )
        {
            for ( auto const& resourceRecord : m_database.GetAllResourcesOfType( m_allowedResourceTypeID ) )
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

    //-------------------------------------------------------------------------

    bool ResourceFilePicker::Draw()
    {
        float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        float const itemSpacing = ImGui::GetStyle().ItemSpacing.x;
        constexpr float const buttonWidth = 22;
        constexpr float const resourceTypeWindowWidth = 36;

        //-------------------------------------------------------------------------

        bool valueUpdated = false;

        //-------------------------------------------------------------------------

        ImGui::PushID( this );
        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 3.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 2 ) );
        ImGui::BeginChild( "IDLabel", ImVec2( resourceTypeWindowWidth, 18 ), true, ImGuiWindowFlags_None | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse );
        {
            if ( m_resourceID.IsValid() )
            {
                ImVec2 const textSize = ImGui::CalcTextSize( m_resourceID.GetResourceTypeID().ToString().c_str() );
                ImGui::SameLine( 0, ( resourceTypeWindowWidth - textSize.x ) / 2 );
                ImGui::AlignTextToFramePadding();
                ImGui::TextColored( Colors::LightPink.ToFloat4(), m_resourceID.GetResourceTypeID().ToString().c_str() );
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

        ImGui::SameLine( 0, itemSpacing );
        ImGui::SetNextItemWidth( cellContentWidth - ( itemSpacing * 3 ) - ( buttonWidth * 2 ) - resourceTypeWindowWidth );
        ImGui::InputText( "##DataPath", const_cast<char*>( m_resourceID.GetResourcePath().c_str() ), m_resourceID.GetResourcePath().GetString().length(), ImGuiInputTextFlags_ReadOnly);

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_CROSSHAIRS "##Pick", ImVec2( buttonWidth, 0 ) ) )
        {
            ImGui::OpenPopup( "Picker" );
        }

        ImGui::SameLine( 0, itemSpacing );
        if ( ImGui::Button( KRG_ICON_TIMES_CIRCLE "##Clear", ImVec2( buttonWidth, 0 ) ) )
        {
            m_resourceID.Clear();
            valueUpdated = true;
        }

        ImGui::PopID();

        //-------------------------------------------------------------------------

        valueUpdated |= DrawDialog();

        return valueUpdated;
    }

    bool ResourceFilePicker::DrawDialog()
    {
        ImGui::PushID( this );

        bool selectionMade = false;
        bool isDialogOpen = true;
        ImGui::SetNextWindowSize( ImVec2( 800, 400 ), ImGuiCond_FirstUseEver );
        ImGui::SetNextWindowSizeConstraints( ImVec2( 400, 400 ), ImVec2( FLT_MAX, FLT_MAX ) );
        if ( ImGui::BeginPopupModal( "Picker", &isDialogOpen, ImGuiWindowFlags_NoSavedSettings ) )
        {
            ImVec2 const contentRegionAvailable = ImGui::GetContentRegionAvail();

            // Draw Filter
            //-------------------------------------------------------------------------

            bool filterUpdated = false;

            ImGui::SetNextItemWidth( contentRegionAvailable.x - ImGui::GetStyle().WindowPadding.x - 22 );
            if ( ImGui::InputText( "##Filter", m_filterBuffer, 256 ) )
            {
                // Convert buffer to lower case
                int32 i = 0;
                while ( i < 256 && m_filterBuffer[i] != 0 )
                {
                    m_filterBuffer[i] = eastl::CharToLower( m_filterBuffer[i] );
                    i++;
                }

                filterUpdated = true;
            }

            ImGui::SameLine();
            if ( ImGui::Button( KRG_ICON_TIMES_CIRCLE "##Clear Filter", ImVec2( 22, 0 ) ) )
            {
                m_filterBuffer[0] = 0;
                filterUpdated = true;
            }

            // Update available options
            //-------------------------------------------------------------------------

            if ( filterUpdated )
            {
                RefreshResourceList();
            }

            // Draw results
            //-------------------------------------------------------------------------

            float const tableHeight = contentRegionAvailable.y - ImGui::GetFrameHeightWithSpacing() - ImGui::GetStyle().ItemSpacing.y;
            ImGui::PushStyleColor( ImGuiCol_Header, ImGuiX::Style::s_itemColorMedium.Value );
            if ( ImGui::BeginTable( "Resource List", 2, ImGuiTableFlags_NoSavedSettings | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY, ImVec2( contentRegionAvailable.x, tableHeight ) ) )
            {
                ImGui::TableSetupColumn( "File", ImGuiTableColumnFlags_WidthStretch, 0.45 );
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
                                m_resourceID = m_filteredResourceIDs[i].m_resourceID;
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

            ImGui::EndPopup();
        }
        else // Reset filter
        {
            if ( m_filterBuffer[0] != 0 )
            {
                m_filterBuffer[0] = 0;
                RefreshResourceList();
            }
        }
        ImGui::PopID();

        //-------------------------------------------------------------------------

        return selectionMade;
    }
}