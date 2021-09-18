#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    // Value type
    //-------------------------------------------------------------------------

    KRG_TOOLS_ANIMATION_API ImColor GetColorForValueType( NodeValueType type );
    KRG_TOOLS_ANIMATION_API char const* GetNameForValueType( NodeValueType type );

    // Graph Types
    //-------------------------------------------------------------------------

    enum class GraphType
    {
        KRG_REGISTER_ENUM

        BlendTree,
        ValueTree,
        TransitionTree,
    };
}