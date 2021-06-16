#pragma once

#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    // Value type
    //-------------------------------------------------------------------------

    ImColor GetColorForValueType( NodeValueType type );
    char const* GetNameForValueType( NodeValueType type );

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