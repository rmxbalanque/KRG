#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Node.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::VisualGraph { struct DrawContext; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphComponent;

    //-------------------------------------------------------------------------

    namespace Graph
    {
        // Graph Types
        //-------------------------------------------------------------------------

        enum class GraphType
        {
            KRG_REGISTER_ENUM

            BlendTree,
            ValueTree,
            TransitionTree,
        };

        // Debug
        //-------------------------------------------------------------------------

        struct DebugContext
        {
            inline NodeIndex GetRuntimeNodeIndex( UUID const& nodeID ) const
            {
                auto const foundIter = m_nodeIDtoIndexMap.find( nodeID );
                if ( foundIter != m_nodeIDtoIndexMap.end() )
                {
                    return foundIter->second;
                }
                return InvalidIndex;
            }

            bool IsNodeActive( NodeIndex nodeIdx ) const;
            PoseNodeDebugInfo GetPoseNodeDebugInfo( NodeIndex runtimeNodeIdx ) const;

        public:

            AnimationGraphComponent*        m_pGraphComponent = nullptr;
            THashMap<UUID, NodeIndex>       m_nodeIDtoIndexMap;
        };

        //-------------------------------------------------------------------------

        void DrawPoseNodeDebugInfo( VisualGraph::DrawContext const& ctx, float width, PoseNodeDebugInfo const& debugInfo );
        void DrawEmptyPoseNodeDebugInfo( VisualGraph::DrawContext const& ctx, float width );
    }
}