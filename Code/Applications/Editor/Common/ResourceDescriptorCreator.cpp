#include "ResourceDescriptorCreator.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceDescriptorCreator::ResourceDescriptorCreator( EditorModel* pModel )
        : m_pModel( pModel )
    {
        KRG_ASSERT( m_pModel != nullptr );
    }

    bool ResourceDescriptorCreator::ShowCreatorDialog( FileSystem::Path const& rawFile )
    {
        KRG_ASSERT( !m_decriptorTypes.empty() );

        //-------------------------------------------------------------------------

        if ( m_isDialogClosed )
        {
            Printf( m_filenameBuffer, 255, rawFile.GetFileNameWithoutExtension().c_str() );
            m_isDialogClosed = false;
        }

        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 0, 2 ) );
        if ( ImGui::BeginTable( "NewDescriptorDetailsTable", 2, 0, ImVec2( -1, 0 ) ) )
        {
            ImGui::TableSetupColumn( "Labels", ImGuiTableColumnFlags_WidthFixed, 50 );
            ImGui::TableSetupColumn( "Values", ImGuiTableColumnFlags_WidthStretch );

            //-------------------------------------------------------------------------

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text( "Name:" );

            ImGui::TableNextColumn();

            ImGui::SetNextItemWidth( -1 );
            ImGui::InputText( "##Descriptor", m_filenameBuffer, 255 );

            //-------------------------------------------------------------------------

            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text( "Type:" );

            ImGui::TableNextColumn();

            ImGui::SetNextItemWidth( -1 );
            if ( ImGui::BeginCombo( "##Descriptor Type", m_decriptorTypes[m_selectedTypeIdx].m_name.c_str() ) )
            {
                for ( size_t i = 0; i < m_decriptorTypes.size(); i++ )
                {
                    bool const isSelected = ( m_selectedTypeIdx == i );
                    if ( ImGui::Selectable( m_decriptorTypes[i].m_name.c_str(), isSelected ) )
                    {
                        m_selectedTypeIdx = (uint32) i;
                    }

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if ( isSelected )
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }

                ImGui::EndCombo();
            }

            ImGui::EndTable();
        }
        ImGui::PopStyleVar();

        // Buttons
        //-------------------------------------------------------------------------

        constexpr static float const buttonWidth = 200;

        if ( ImGui::Button( "Create", ImVec2( buttonWidth, 0 ) ) )
        {
            // TODO: add further validation
            if ( strlen( m_filenameBuffer ) > 0 )
            {
                CreateNewDescriptor( rawFile );
                m_isDialogClosed = true;
                return true;
            }
        }

        ImGui::SameLine( 0, ImGui::GetStyle().ItemSpacing.x / 2 );

        if ( ImGui::Button( "Cancel", ImVec2( buttonWidth, 0 ) ) )
        {
            m_isDialogClosed = true;
            return true;
        }

        return false;
    }

    FileSystem::Path ResourceDescriptorCreator::CreateDescriptorFilePath( FileSystem::Path const& rawFile ) const
    {
        KRG_ASSERT( m_selectedTypeIdx < m_decriptorTypes.size() );

        InlineString<5> extension = m_decriptorTypes[m_selectedTypeIdx].m_typeID.ToString();
        extension.make_lower();

        InlineString<100> fileName;
        fileName.sprintf( "%s.%s", m_filenameBuffer, extension.c_str() );

        FileSystem::Path newDescriptorPath = rawFile.GetParentDirectory();
        newDescriptorPath += fileName.c_str();
        return newDescriptorPath;
    }
}