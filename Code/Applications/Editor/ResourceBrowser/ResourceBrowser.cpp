#include "ResourceBrowser.h"
#include "System/Core/Profiling/Profiling.h"
#include "RawFileInspectors/RawFileInspector.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceBrowser::ResourceBrowser( EditorModel& model )
        : m_model( model )
        , m_treeView( &model )
    {
        Memory::MemsetZero( m_nameFilterBuffer, 256 * sizeof( char ) );

        m_onDoubleClickEventID = m_treeView.OnItemDoubleClicked().Bind( [this] ( TreeViewItem* pItem ) { OnBrowserItemDoubleClicked( pItem ); } );
        m_treeView.RebuildBrowserTree();
        UpdateVisibility();
    }

    ResourceBrowser::~ResourceBrowser()
    {
        m_treeView.OnItemDoubleClicked().Unbind( m_onDoubleClickEventID );

        KRG::Delete( m_pRawFileInspector );
    }

    //-------------------------------------------------------------------------

    bool ResourceBrowser::Draw( UpdateContext const& context )
    {
        // Tree View
        //-------------------------------------------------------------------------

        bool isOpen = true;
        if ( ImGui::Begin( "Data Browser", &isOpen ) )
        {
            UpdateAndDrawBrowserFilters( context );
            m_treeView.Draw();
        }
        ImGui::End();

        // File Inspector dialog
        //-------------------------------------------------------------------------

        if ( m_pRawFileInspector != nullptr )
        {
            if ( !m_pRawFileInspector->DrawDialog() )
            {
                KRG::Delete( m_pRawFileInspector );
            }
        }

        //-------------------------------------------------------------------------

        return isOpen;
    }

    void ResourceBrowser::RebuildBrowserTree()
    {
        m_treeView.RebuildBrowserTree();
        UpdateVisibility();
    }

    //-------------------------------------------------------------------------

    void ResourceBrowser::UpdateVisibility()
    {
        auto VisibilityFunc = [this] ( TreeViewItem const* pItem )
        {
            bool isVisible = true;

            // Type filter
            //-------------------------------------------------------------------------

            auto pDataFileItem = static_cast<ResourceBrowserTreeItem const*>( pItem );
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

        m_treeView.UpdateItemVisibility( VisibilityFunc );
    }

    void ResourceBrowser::UpdateAndDrawBrowserFilters( UpdateContext const& context )
    {
        KRG_PROFILE_FUNCTION();

        bool shouldUpdateVisibility = false;
        Int2 const windowContentRegion = ( ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin() );

        // Text Filter
        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( windowContentRegion.m_x + ImGui::GetStyle().WindowPadding.x - 27 );
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

            m_treeView.ForEachItem( SetExpansion );
        }

        ImGui::SameLine( windowContentRegion.m_x + ImGui::GetStyle().WindowPadding.x - 20 );
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

        ImGui::SameLine( windowContentRegion.m_x + ImGui::GetStyle().WindowPadding.x - 42 );
        if ( ImGui::Button( KRG_ICON_PLUS "##Expand All", ImVec2( 19, 0 ) ) )
        {
            m_treeView.ForEachItem( [] ( TreeViewItem* pItem ) { pItem->SetExpanded( true ); } );
        }

        ImGui::SameLine( windowContentRegion.m_x + ImGui::GetStyle().WindowPadding.x - 20 );
        if ( ImGui::Button( KRG_ICON_MINUS "##Collapse ALL", ImVec2( 19, 0 ) ) )
        {
            m_treeView.ForEachItem( [] ( TreeViewItem* pItem ) { pItem->SetExpanded( false ); } );
        }

        //-------------------------------------------------------------------------

        if ( shouldUpdateVisibility )
        {
            UpdateVisibility();
        }
    }

    bool ResourceBrowser::DrawResourceTypeFilterMenu()
    {
        bool requiresVisibilityUpdate = false;

        if ( ImGui::Button( KRG_ICON_ARROW_CIRCLE_DOWN " Resource Type Filters"  ) )
        {
            ImGui::OpenPopup( "ResourceFilters" );
        }

        if ( ImGui::BeginPopup( "ResourceFilters" ) )
        {
            for ( auto const& resourceInfo : m_model.GetTypeRegistry()->GetRegisteredResourceTypes() )
            {
                bool isChecked = VectorContains( m_typeFilter, resourceInfo.second.m_resourceTypeID );
                if ( ImGui::Checkbox( resourceInfo.second.m_friendlyName.c_str(), &isChecked ) )
                {
                    if ( isChecked )
                    {
                        m_typeFilter.emplace_back( resourceInfo.second.m_resourceTypeID );
                    }
                    else
                    {
                        m_typeFilter.erase_first_unsorted( resourceInfo.second.m_resourceTypeID );
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

    void ResourceBrowser::OnBrowserItemDoubleClicked( TreeViewItem* pItem )
    {
        auto pResourceFileItem = static_cast<ResourceBrowserTreeItem const*>( pItem );
        if ( pResourceFileItem->IsDirectory() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( pResourceFileItem->IsResourceFile() )
        {
            m_model.TryCreateWorkspace( pResourceFileItem->GetResourceID() );
        }
        else // Try create file inspector
        {
            if ( RawFileInspectorFactory::CanCreateInspector( pResourceFileItem->GetFilePath() ) )
            {
                m_pRawFileInspector = RawFileInspectorFactory::TryCreateInspector( &m_model, pResourceFileItem->GetFilePath() );
            }
        }
    }
}