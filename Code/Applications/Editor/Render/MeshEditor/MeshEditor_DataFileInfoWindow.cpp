#include "MeshEditor.h"
#include "System/DevTools/CommonWidgets/CommonWidgets.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    void MeshEditor::DrawDataFileInfoWindow()
    {
        if ( ImGui::Begin( "Data File Info" ) )
        {
            ImGui::Text( "TODO" );
            ImGuiX::DrawSpinner( "Blah" );
        }
        ImGui::End();

    }
}