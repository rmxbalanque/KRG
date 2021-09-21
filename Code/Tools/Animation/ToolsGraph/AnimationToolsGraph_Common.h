#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    // Value type
    //-------------------------------------------------------------------------

    ImColor GetColorForValueType( ValueType type );
    char const* GetNameForValueType( ValueType type );

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