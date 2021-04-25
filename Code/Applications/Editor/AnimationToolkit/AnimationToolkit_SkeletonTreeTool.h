#pragma once
#include "AnimationToolkit_Model.h"
#include "Applications/Editor/EditorTool.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    struct BoneInfo;

    //-------------------------------------------------------------------------

    class SkeletonTree : public TEditorTool<Model>
    {

    public:

        SkeletonTree( EditorModel* pModel ) : TEditorTool<Model>( pModel ) {}
        ~SkeletonTree();

    private:

        virtual char const * const GetName() { return "Animation Info"; }
        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;
    };
}