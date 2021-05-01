#include "SkeletonResourceFile.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    void SkeletonResourceFile::SetActive( EntityWorld* pPreviewWorld )
    {
    }

    void SkeletonResourceFile::SetInactive( EntityWorld* pPreviewWorld )
    {
    }

    void SkeletonResourceFile::DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( IsLoading() )
        {
            ImGui::Text( "Loading:" );
            ImGui::SameLine();
            ImGuiX::DrawSpinner( "Loading" );
            return;
        }

        if ( HasLoadingFailed() )
        {
            ImGui::Text( "Loading Failed: %s", m_pResource.GetResourceID().c_str() );
            return;
        }

        //-------------------------------------------------------------------------

        ImGui::Text( "Skeleton" );
    }
}