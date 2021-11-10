#include "RawFileInspector_ImageFormats.h"
#include "Tools/Core/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Render/ResourceDescriptors/ResourceDescriptor_RenderTexture.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Core/Math/MathStringHelpers.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    KRG_RAW_FILE_INSPECTOR_FACTORY( InspectorFactoryPNG, "png", FileInspectorImageFormats );
    KRG_RAW_FILE_INSPECTOR_FACTORY( InspectorFactoryBMP, "bmp", FileInspectorImageFormats );
    KRG_RAW_FILE_INSPECTOR_FACTORY( InspectorFactoryTGA, "tga", FileInspectorImageFormats );
    KRG_RAW_FILE_INSPECTOR_FACTORY( InspectorFactoryJPG, "jpg", FileInspectorImageFormats );

    //-------------------------------------------------------------------------

    FileInspectorImageFormats::FileInspectorImageFormats( EditorModel* pModel, FileSystem::Path const& filePath )
        : RawFileInspector( pModel, filePath )
    {
        KRG_ASSERT( FileSystem::Exists( filePath ) );
    }

    bool FileInspectorImageFormats::DrawDialog()
    {
        if ( !ImGui::IsPopupOpen( s_title ) )
        {
            ImGui::OpenPopup( s_title );
        }

        //-------------------------------------------------------------------------

        bool isOpen = true;
        ImGui::SetNextWindowSize( ImVec2( 600, 800 ), ImGuiCond_FirstUseEver );
        if ( ImGui::BeginPopupModal( s_title, &isOpen ) )
        {
            ImGui::Text( "Raw File: %s", m_filePath.c_str() );

            if ( ImGui::Button( KRG_ICON_PLUS " Create Texture", ImVec2( -1, 0 ) ) )
            {
                Render::TextureResourceDescriptor resourceDesc;
                resourceDesc.m_path = ResourcePath::FromFileSystemPath( m_pModel->GetSourceResourceDirectory(), m_filePath );
                CreateNewDescriptor( Render::Texture::GetStaticResourceTypeID(), resourceDesc );
            }

            ImGui::EndPopup();
        }

        return isOpen && ImGui::IsPopupOpen( s_title );
    }
}