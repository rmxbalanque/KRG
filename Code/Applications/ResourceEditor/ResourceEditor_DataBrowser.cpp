#include "ResourceEditor_DataBrowser.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG
{
    DataBrowser::DataBrowser( ResourceEditorModel& model )
        : m_model( model )
        , m_dataBrowserTreeView( &model )
        , m_dataFileInspector( &model )
    {
        Memory::MemsetZero( m_nameFilterBuffer, 256 * sizeof( char ) );

        m_onDoubleClickEventID = m_dataBrowserTreeView.OnItemDoubleClicked().Bind( [this] ( TreeViewItem* pItem ) { OnBrowserItemDoubleClicked( pItem ); } );
        m_dataBrowserTreeView.RebuildBrowserTree();
        UpdateVisibility();
    }

    DataBrowser::~DataBrowser()
    {
        m_dataBrowserTreeView.OnItemDoubleClicked().Unbind( m_onDoubleClickEventID );
    }

    //-------------------------------------------------------------------------

    void DataBrowser::Draw( UpdateContext const& context )
    {
        m_dataBrowserTreeView.Update( context );

        //-------------------------------------------------------------------------

        if ( ImGui::Begin( "Data Browser", nullptr ) )
        {
            // Filters and controls
            //-------------------------------------------------------------------------

            UpdateAndDrawBrowserFilters( context );

            // Data Browser
            //-------------------------------------------------------------------------

            if ( ImGui::BeginChild( "DataBrowser", ImVec2( 0.f, m_dataBrowserViewHeight ), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse ) )
            {
                m_dataBrowserTreeView.Draw();

                auto pSelectedItem = m_dataBrowserTreeView.GetSelectedItem<DataBrowserTreeItem>();
                if ( pSelectedItem != nullptr && pSelectedItem->IsFile() )
                {
                    if ( pSelectedItem->GetPath() != m_dataFileInspector.GetInspectedFilePath() )
                    {
                        m_dataFileInspector.SetFileToInspect( pSelectedItem->GetPath() );
                    }
                }
                else // Clear inspected file
                {
                    m_dataFileInspector.ClearFileToInspect();
                }
            }
            ImGui::EndChild();

            // Splitter
            //-------------------------------------------------------------------------

            ImGui::PushStyleColor( ImGuiCol_Button, ImGuiX::Theme::s_backgroundColorSemiDark );
            ImGui::PushStyleColor( ImGuiCol_ButtonHovered, ImGuiX::Theme::s_backgroundColorSemiLight );
            ImGui::PushStyleColor( ImGuiCol_ButtonActive, ImGuiX::Theme::s_backgroundColorSemiLight );
            ImGui::PushStyleVar( ImGuiStyleVar_FrameRounding, 0.0f );
            ImGui::Button( "##ViewSplitter", ImVec2( -1, 3 ) );
            ImGui::PopStyleColor( 3 );
            ImGui::PopStyleVar();

            if ( ImGui::IsItemHovered() )
            {
                ImGui::SetMouseCursor( ImGuiMouseCursor_ResizeNS );
            }

            if ( ImGui::IsItemActive() )
            {
                m_dataBrowserViewHeight += ImGui::GetIO().MouseDelta.y;
                m_dataBrowserViewHeight = Math::Max( 25.0f, m_dataBrowserViewHeight );
            }

            // Data Inspector
            //-------------------------------------------------------------------------

            if ( ImGui::BeginChild( "DataInspector", ImVec2( 0.f, 0.0f ), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollWithMouse ) )
            {
                m_dataFileInspector.Draw();
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    //-------------------------------------------------------------------------

    void DataBrowser::UpdateVisibility()
    {
        auto VisibilityFunc = [this] ( TreeViewItem const* pItem )
        {
            bool isVisible = true;

            // Type filter
            //-------------------------------------------------------------------------

            auto pDataFileItem = static_cast<DataBrowserTreeItem const*>( pItem );
            if ( pDataFileItem->IsFile() )
            {
                if ( pDataFileItem->IsRawFile() )
                {
                    isVisible = m_showRawFiles;
                }
                else // Resource file
                {
                    auto const& resourceTypeID = pDataFileItem->GetResourceTypeID();
                    isVisible = m_typeFilter.empty() || VectorContains( m_typeFilter, resourceTypeID );
                }
            }

            // Text filter
            //-------------------------------------------------------------------------

            if ( isVisible && m_nameFilterBuffer[0] != 0 )
            {
                String lowercaseLabel = pItem->GetLabel();
                lowercaseLabel.make_lower();

                char tempBuffer[256];
                strcpy( tempBuffer, m_nameFilterBuffer );

                char* token = strtok( tempBuffer, " " );
                while ( token )
                {
                    if ( lowercaseLabel.find( token ) == String::npos )
                    {
                        isVisible = false;
                        break;
                    }

                    token = strtok( NULL, " " );
                }
            }

            //-------------------------------------------------------------------------

            return isVisible;
        };

        //-------------------------------------------------------------------------

        m_dataBrowserTreeView.UpdateItemVisibility( VisibilityFunc );
    }

    void DataBrowser::UpdateAndDrawBrowserFilters( UpdateContext const& context )
    {
        KRG_PROFILE_FUNCTION();

        bool shouldUpdateVisibility = false;

        // Text Filter
        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 27 );
        if ( ImGui::InputText( "##Filter", m_nameFilterBuffer, 256 ) )
        {
            // Convert buffer to lower case
            int32 i = 0;
            while ( i < 256 && m_nameFilterBuffer[i] != 0 )
            {
                m_nameFilterBuffer[i] = eastl::CharToLower( m_nameFilterBuffer[i] );
                i++;
            }

            shouldUpdateVisibility = true;

            auto const SetExpansion = []( TreeViewItem* pItem )
            {
                if ( pItem->IsVisible() )
                {
                    pItem->SetExpanded( true );
                }
            };

            m_dataBrowserTreeView.ForEachItem( SetExpansion );
        }

        ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 20 );
        if ( ImGui::Button( KRG_ICON_TIMES_CIRCLE "##Clear Filter", ImVec2( 19, 0 ) ) )
        {
            m_nameFilterBuffer[0] = 0;
            shouldUpdateVisibility = true;
        }

        // Type Filter + Controls
        //-------------------------------------------------------------------------

        shouldUpdateVisibility |= DrawResourceTypeFilterMenu();

        ImGui::SameLine( 0, 4 );
        if ( ImGui::Checkbox( "Raw Files", &m_showRawFiles ) )
        {
            shouldUpdateVisibility = true;
        }

        ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 42 );
        if ( ImGui::Button( KRG_ICON_PLUS "##Expand All", ImVec2( 19, 0 ) ) )
        {
            m_dataBrowserTreeView.ForEachItem( [] ( TreeViewItem* pItem ) { pItem->SetExpanded( true ); } );
        }

        ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 20 );
        if ( ImGui::Button( KRG_ICON_MINUS "##Collapse ALL", ImVec2( 19, 0 ) ) )
        {
            m_dataBrowserTreeView.ForEachItem( [] ( TreeViewItem* pItem ) { pItem->SetExpanded( false ); } );
        }

        //-------------------------------------------------------------------------

        if ( shouldUpdateVisibility )
        {
            UpdateVisibility();
        }
    }

    bool DataBrowser::DrawResourceTypeFilterMenu()
    {
        bool requiresVisibilityUpdate = false;

        if ( ImGui::Button( KRG_ICON_ARROW_CIRCLE_DOWN " Resource Type Filters"  ) )
        {
            ImGui::OpenPopup( "ResourceFilters" );
        }

        if ( ImGui::BeginPopup( "ResourceFilters" ) )
        {
            InlineString<255> label;

            for ( auto const& resourceType : m_model.GetTypeRegistry()->GetRegisteredResourceTypes() )
            {
                label.sprintf( "%s - %s", resourceType.second.ToString().c_str(), resourceType.first.c_str() );

                bool isChecked = VectorContains( m_typeFilter, resourceType.second );
                if ( ImGui::Checkbox( label.c_str(), &isChecked ) )
                {
                    if ( isChecked )
                    {
                        m_typeFilter.emplace_back( resourceType.second );
                    }
                    else
                    {
                        m_typeFilter.erase_first_unsorted( resourceType.second );
                    }

                    requiresVisibilityUpdate = true;
                }
            }

            ImGui::EndPopup();
        }

        //-------------------------------------------------------------------------

        return requiresVisibilityUpdate;
    }

    //-------------------------------------------------------------------------

    void DataBrowser::OnBrowserItemDoubleClicked( TreeViewItem* pItem )
    {
        auto pDataFileItem = static_cast<DataBrowserTreeItem const*>( pItem );
        if ( pDataFileItem->IsDirectory() || pDataFileItem->IsRawFile() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        auto const resourceID = pDataFileItem->GetResourceID();
        if ( m_model.CanCreateWorkspaceForResourceType( resourceID.GetResourceTypeID() ) )
        {
            m_model.OpenWorkspace( resourceID );
        }
    }
}