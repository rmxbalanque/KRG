#include "AnimationClipEditor_DataBrowser.h"
#include "FileTabs/SkeletonFileTab.h"
#include "FileTabs/AnimationFileTab.h"
#include "Tools/Core/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Animation/ResourceCompilers/AnimationClipCompiler.h"
#include "Tools/Animation/ResourceCompilers/AnimationSkeletonCompiler.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    static Color const g_rawFileColor = Colors::LightGray;
    static Color const g_skeletonColor = Colors::HotPink;
    static Color const g_animationColor = Colors::GreenYellow;

    //-------------------------------------------------------------------------

    void AnimationDataFileInspector::OnStartInspectingRawFile()
    {
        if ( RawAssets::ReadFileInfo( m_inspectedFile, m_assetInfo ) )
        {
            m_validAssetInfo = true;
        }
        else
        {
            m_validAssetInfo = false;
            KRG_LOG_ERROR( "Resource", "Failed to read raw resource file: %s", m_inspectedFile.c_str() );
        }
    }

    void AnimationDataFileInspector::OnStopInspectingRawFile()
    {
        m_assetInfo.Reset();
        m_validAssetInfo = false;
    }

    void AnimationDataFileInspector::DrawRawFileInfo()
    {
        KRG_ASSERT( m_inspectedFile.IsFilePath() );

        ImGui::Text( "Raw File: %s", m_inspectedFile.c_str() );
        ImGui::Separator();

        if ( m_validAssetInfo )
        {
            // Asset Info
            //-------------------------------------------------------------------------

            ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg;
            ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 2, 4 ) );
            if ( ImGui::BeginTable( "Raw File Contents", 3, 0 ) )
            {
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed, 120 );
                ImGui::TableSetupColumn( "Animation Name", ImGuiTableColumnFlags_NoHide );
                ImGui::TableSetupColumn( "Controls", ImGuiTableColumnFlags_WidthFixed, 26 );

                for ( auto const& skeleton : m_assetInfo.m_skeletons )
                {
                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextColored( g_skeletonColor.ToFloat4(), "Skeleton:" );

                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGuiX::SelectableText( skeleton.m_name, -1 );

                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGui::PushID( &skeleton );
                    if ( ImGui::Button( KRG_ICON_MALE "##CreateSkeleton", ImVec2( 24, 0 ) ) )
                    {
                        SkeletonResourceDescriptor resourceDesc;
                        resourceDesc.m_skeletonDataPath = DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile );
                        resourceDesc.m_skeletonRootBoneName = skeleton.m_name;
                        CreateNewDescriptor( Skeleton::GetStaticResourceTypeID(), resourceDesc );
                    }
                    ImGui::PopID();

                    ImGuiX::ItemTooltip( "Create Skeleton" );
                }

                //-------------------------------------------------------------------------

                for ( auto const& anim : m_assetInfo.m_animations )
                {
                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    ImGui::TextColored( g_animationColor.ToFloat4(), "Animation:" );

                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGuiX::SelectableText( anim.m_name, -1 );

                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGui::PushID( &anim );
                    if ( ImGui::Button( KRG_ICON_FILM "##CreateAnimation", ImVec2( 24, 0 ) ) )
                    {
                        AnimationClipResourceDescriptor resourceDesc;
                        resourceDesc.m_animationDataPath = DataPath::FromFileSystemPath( m_sourceDataPath, m_inspectedFile );
                        resourceDesc.m_animationName = anim.m_name;
                        CreateNewDescriptor( AnimationClip::GetStaticResourceTypeID(), resourceDesc );
                    }
                    ImGui::PopID();

                    ImGuiX::ItemTooltip( "Create Animation" );
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();
        }
    }

    //-------------------------------------------------------------------------

    void DataBrowser::AnimationBrowserTreeItem::DrawControls()
    {
        if ( IsDirectory() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( IsRawFile() )
        {
            ImGui::TextColored( g_rawFileColor.ToFloat4(), "Raw" );
        }
        else if ( IsResourceOfType<Skeleton>() )
        {
            ImGui::TextColored( g_skeletonColor.ToFloat4(), "Skeleton" );
        }
        else if ( IsResourceOfType<AnimationClip>() )
        {
            ImGui::TextColored( g_animationColor.ToFloat4(), "Animation" );
        }
    }

    //-------------------------------------------------------------------------

    DataBrowser::DataBrowser( EditorModel* pModel )
        : TEditorTool<EditorModel>( pModel )
        , m_dataBrowserTreeView( pModel->GetTypeRegistry(), pModel->GetSourceDataDirectory(), { ".skel", ".anim", ".fbx", ".gltf" } )
        , m_dataFileInspector( pModel->GetTypeRegistry(), pModel->GetSourceDataDirectory() )
    {
        m_onDoubleClickEventID = m_dataBrowserTreeView.OnItemDoubleClicked().Bind( [this] ( TreeViewItem* pItem ) { OnBrowserItemDoubleClicked( pItem ); } );
        m_dataBrowserTreeView.RebuildBrowserTree();
        UpdateVisibility();
    }

    DataBrowser::~DataBrowser()
    {
        m_dataBrowserTreeView.OnItemDoubleClicked().Unbind( m_onDoubleClickEventID );
    }

    //-------------------------------------------------------------------------

    void DataBrowser::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( !IsOpen() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        m_dataBrowserTreeView.Update( context );

        //-------------------------------------------------------------------------

        DrawBrowser( context );

        //-------------------------------------------------------------------------

        auto pSelectedItem = m_dataBrowserTreeView.GetSelectedItem<AnimationBrowserTreeItem>();
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

        //-------------------------------------------------------------------------

        if ( ImGui::Begin( "Data File Info" ) )
        {
            m_dataFileInspector.Draw();
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

    void DataBrowser::DrawBrowser( UpdateContext const& context )
    {
        KRG_PROFILE_FUNCTION();

        if ( ImGui::Begin( "Data Browser", nullptr ) )
        {
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
            }

            ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 20 );
            if ( ImGui::Button( KRG_ICON_ERASER "##Clear Filter" ) )
            {
                m_filterBuffer[0] = 0;
                UpdateVisibility();
            }

            // Type Filter + Controls
            //-------------------------------------------------------------------------

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

            //-------------------------------------------------------------------------

            ImGui::Separator();

            // Browser
            //-------------------------------------------------------------------------

            m_dataBrowserTreeView.Draw();
        }
        ImGui::End();
    }

    //-------------------------------------------------------------------------

    void DataBrowser::OnBrowserItemDoubleClicked( TreeViewItem* pItem )
    {
        auto pDataFileItem = static_cast<DataBrowserTreeItem const*>( pItem );
        if ( pDataFileItem->IsDirectory() || pDataFileItem->IsRawFile() )
        {
            return;
        }

        if ( pDataFileItem->IsResourceOfType<Skeleton>() )
        {
            ResourceID const resourceID( pDataFileItem->GetDataPath() );
            m_model.OpenResourceFile<SkeletonFileTab>( resourceID );
        }
        else if ( pDataFileItem->IsResourceOfType<AnimationClip>() )
        {
            ResourceID const resourceID( pDataFileItem->GetDataPath() );
            m_model.OpenResourceFile<AnimationFileTab>( resourceID );
        }
    }
}