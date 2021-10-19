#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "System/Render/Imgui/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor { struct DrawingContext; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationGraphComponent;

    //-------------------------------------------------------------------------

    namespace Graph
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

        public:

            AnimationGraphComponent*        m_pGraphComponent = nullptr;
            THashMap<UUID, NodeIndex>       m_nodeIDtoIndexMap;
        };

        void DrawPoseNodeDebugInfo( GraphEditor::DrawingContext const& ctx, float width, PoseNodeDebugInfo const& debugInfo );
        void DrawEmptyPoseNodeDebugInfo( GraphEditor::DrawingContext const& ctx, float width );
    }
}