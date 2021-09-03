#include "ResourceEditor_DataBrowser.h"
#include "Tools/Render/ResourceCompilers/Mesh/StaticMeshCompiler.h"
#include "Tools/Render/ResourceCompilers/Mesh/SkeletalMeshCompiler.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Math/MathStringHelpers.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    DataBrowser::DataBrowser( ResourceEditorModel& model )
        : m_model( model )
        , m_dataBrowserTreeView( model.GetTypeRegistry(), model.GetSourceDataDirectory() )
        , m_dataFileInspector( model.GetTypeRegistry(), model.GetSourceDataDirectory() )
    {
        Memory::MemsetZero( m_filterBuffer, 256 * sizeof( char ) );

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
                else
                {
                    isVisible = true;
                }
            }

            // Text filter
            //-------------------------------------------------------------------------

            if ( isVisible && m_filterBuffer[0] != 0 )
            {
                String lowercaseLabel = pItem->GetLabel();
                lowercaseLabel.make_lower();

                char tempBuffer[256];
                strcpy( tempBuffer, m_filterBuffer );

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

        // Text Filter
        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 31 );
        if ( ImGui::InputText( "##Filter", m_filterBuffer, 256 ) )
        {
            // Convert buffer to lower case
            int32 i = 0;
            while ( i < 256 && m_filterBuffer[i] != 0 )
            {
                m_filterBuffer[i] = eastl::CharToLower( m_filterBuffer[i] );
                i++;
            }

            UpdateVisibility();

            auto const SetExpansion = []( TreeViewItem* pItem )
            {
                if ( pItem->IsVisible() )
                {
                    pItem->SetExpanded( true );
                }
            };

            m_dataBrowserTreeView.ForEachItem( SetExpansion );
        }

        ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 24 );
        if ( ImGui::Button( KRG_ICON_ERASER "##Clear Filter" ) )
        {
            m_filterBuffer[0] = 0;
            UpdateVisibility();
        }

        // Type Filter + Controls
        //-------------------------------------------------------------------------

        if ( ImGui::Checkbox( "Static Meshes", &m_showStaticMeshes ) )
        {
            UpdateVisibility();
        }

        ImGui::SameLine();
        if ( ImGui::Checkbox( "Skeletal Meshes", &m_showSkeletalMeshes ) )
        {
            UpdateVisibility();
        }

        ImGui::SameLine();
        if ( ImGui::Checkbox( "Raw Files", &m_showRawFiles ) )
        {
            UpdateVisibility();
        }

        ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 42 );
        if ( ImGui::Button( KRG_ICON_PLUS "##Expand All" ) )
        {
            m_dataBrowserTreeView.ForEachItem( [] ( TreeViewItem* pItem ) { pItem->SetExpanded( true ); } );
        }

        ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 20 );
        if ( ImGui::Button( KRG_ICON_MINUS "##Collapse ALL" ) )
        {
            m_dataBrowserTreeView.ForEachItem( [] ( TreeViewItem* pItem ) { pItem->SetExpanded( false ); } );
        }
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