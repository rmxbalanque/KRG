#include "SkeletonFileTab.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    void SkeletonFileTab::Activate( EntityWorld* pPreviewWorld )
    {
    }

    void SkeletonFileTab::Deactivate( EntityWorld* pPreviewWorld )
    {
    }

    void SkeletonFileTab::DrawTools( UpdateContext const& context, Render::ViewportManager& viewportManager )
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