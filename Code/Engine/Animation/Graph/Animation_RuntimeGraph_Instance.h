#pragma once
#include "Animation_RuntimeGraph_Resources.h"
#include "Animation_RuntimeGraph_Contexts.h"
#include "Animation_RuntimeGraph_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphInstance
    {
    public:

        GraphInstance( AnimationGraphVariation const* pGraphVariation );
        ~GraphInstance();

        void Initialize( GraphContext& context );
        void Shutdown( GraphContext& context );

        // Graph State
        //-------------------------------------------------------------------------

        // Is this a valid instance that has been correctly initialized
        bool IsInitialized() const { return m_pRootNode != nullptr && m_pRootNode->IsValid(); }

        // Fully reset all nodes in the graph
        void Reset( GraphContext& context );

        // Run the graph logic
        PoseNodeResult UpdateGraph( GraphContext& context );

        // General Node Info
        //-------------------------------------------------------------------------

        inline bool IsValidNodeIndex( NodeIndex nodeIdx ) const 
        {
            return nodeIdx < m_pGraphVariation->m_pGraphDefinition->m_nodeSettings.size();
        }

        // Control Parameters
        //-------------------------------------------------------------------------

        inline int32 GetNumControlParameters() const { return m_pGraphVariation->m_pGraphDefinition->m_numControlParameters; }

        inline NodeIndex GetControlParameterIndex( StringID parameterID ) const
        {
            for ( NodeIndex i = 0; i < m_pGraphVariation->m_pGraphDefinition->m_numControlParameters; i++ )
            {
                if ( m_pGraphVariation->m_pGraphDefinition->m_nodeSettings[i]->m_nodeID == parameterID )
                {
                    return i;
                }
            }

            return InvalidIndex;
        }

        inline StringID GetControlParameterID( NodeIndex parameterNodeIdx )
        {
            KRG_ASSERT( IsControlParameter( parameterNodeIdx ) );
            return m_nodes[parameterNodeIdx]->GetNodeID();
        }

        inline ValueType GetControlParameterType( NodeIndex parameterNodeIdx )
        {
            KRG_ASSERT( IsControlParameter( parameterNodeIdx ) );
            return static_cast<ValueNode*>( m_nodes[parameterNodeIdx] )->GetValueType();
        }

        template<typename T>
        inline void SetControlParameterValue( GraphContext& context, NodeIndex parameterNodeIdx, T const& value )
        {
            KRG_ASSERT( IsControlParameter( parameterNodeIdx ) );
            static_cast<ValueNode*>( m_nodes[parameterNodeIdx] )->SetValue<T>( context, value );
        }

        template<typename T>
        inline T GetControlParameterValue( GraphContext& context, NodeIndex parameterNodeIdx ) const
        {
            KRG_ASSERT( IsControlParameter( parameterNodeIdx ) );
            return static_cast<ValueNode*>( m_nodes[parameterNodeIdx] )->GetValue<T>( context );
        }

        // Debug Information
        //-------------------------------------------------------------------------
        
        #if KRG_DEVELOPMENT_TOOLS
        inline bool IsNodeActive( GraphContext& context, NodeIndex nodeIdx ) const
        {
            KRG_ASSERT( IsValidNodeIndex( nodeIdx ) );
            auto pNode = m_nodes[nodeIdx];
            return pNode->IsNodeActive( context );
        }

        inline PoseNodeDebugInfo GetPoseNodeDebugInfo( GraphContext& context, NodeIndex nodeIdx ) const
        {
            KRG_ASSERT( IsValidNodeIndex( nodeIdx ) );
            auto pNode = static_cast<PoseNode const*>( m_nodes[nodeIdx] );
            return pNode->GetDebugInfo();
        }
        #endif

    private:

        KRG_FORCE_INLINE bool IsControlParameter( NodeIndex nodeIdx ) const 
        {
            return nodeIdx < m_pGraphVariation->m_pGraphDefinition->m_numControlParameters;
        }

        GraphInstance( GraphInstance const& ) = delete;
        GraphInstance( GraphInstance&& ) = delete;
        GraphInstance& operator=( GraphInstance const& ) = delete;
        GraphInstance& operator=( GraphInstance&& ) = delete;

    private:

        AnimationGraphVariation const* const    m_pGraphVariation = nullptr;
        TVector<GraphNode*>                     m_nodes;
        Byte*                                   m_pAllocatedInstanceMemory = nullptr;
        PoseNode*                               m_pRootNode = nullptr;
    };
}
