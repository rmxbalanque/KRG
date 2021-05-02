#include "SkeletonResourceFile.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Entity/EntityWorld.h"
#include "System/Core/Update/UpdateContext.h"

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

        auto drawingCtx = context.GetDrawingContext();
        drawingCtx.Draw( *m_pResource.GetPtr(), Transform::Identity );

        ImGui::Text( "Skeleton" );
    }
}