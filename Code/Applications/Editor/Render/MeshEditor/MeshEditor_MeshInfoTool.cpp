#include "MeshEditor_MeshInfoTool.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    void MeshInfo::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( ImGui::Begin( "Mesh Info" ) )
        {
            ImGui::Text( "TODO" );
        }
        ImGui::End();
    }
}