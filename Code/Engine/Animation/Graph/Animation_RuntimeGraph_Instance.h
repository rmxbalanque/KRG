#pragma once
#include "Animation_RuntimeGraph_Resources.h"
#include "Animation_RuntimeGraph_Contexts.h"
#include "Animation_RuntimeGraph_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class GraphInstance
    {
    public:

        GraphInstance( GraphVariation const* pGraphVariation );
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
        GraphPoseNodeResult UpdateGraph( GraphContext& context );

        // General Node Info
        //-------------------------------------------------------------------------

        inline bool IsValidNodeIndex( GraphNodeIndex nodeIdx ) const 
        {
            return nodeIdx < m_pGraphVariation->m_pGraphDefinition->m_nodeSettings.size();
        }

        // Control Parameters
        //-------------------------------------------------------------------------

        inline int32 GetNumControlParameters() const { return m_pGraphVariation->m_pGraphDefinition->m_numControlParameters; }

        inline GraphNodeIndex GetControlParameterIndex( StringID parameterID ) const
        {
            for ( GraphNodeIndex i = 0; i < m_pGraphVariation->m_pGraphDefinition->m_numControlParameters; i++ )
            {
                if ( m_pGraphVariation->m_pGraphDefinition->m_controlParameterIDs[i] == parameterID )
                {
                    return i;
                }
            }

            return InvalidIndex;
        }

        inline StringID GetControlParameterID( GraphNodeIndex parameterNodeIdx )
        {
            KRG_ASSERT( IsControlParameter( parameterNodeIdx ) );
            return m_pGraphVariation->m_pGraphDefinition->m_controlParameterIDs[parameterNodeIdx];
        }

        inline GraphValueType GetControlParameterType( GraphNodeIndex parameterNodeIdx )
        {
            KRG_ASSERT( IsControlParameter( parameterNodeIdx ) );
            return static_cast<ValueNode*>( m_nodes[parameterNodeIdx] )->GetValueType();
        }

        template<typename T>
        inline void SetControlParameterValue( GraphContext& context, GraphNodeIndex parameterNodeIdx, T const& value )
        {
            KRG_ASSERT( IsControlParameter( parameterNodeIdx ) );
            static_cast<ValueNode*>( m_nodes[parameterNodeIdx] )->SetValue<T>( context, value );
        }

        template<typename T>
        inline T GetControlParameterValue( GraphContext& context, GraphNodeIndex parameterNodeIdx ) const
        {
            KRG_ASSERT( IsControlParameter( parameterNodeIdx ) );
            return static_cast<ValueNode*>( m_nodes[parameterNodeIdx] )->GetValue<T>( context );
        }

        // Debug Information
        //-------------------------------------------------------------------------
        
        #if KRG_DEVELOPMENT_TOOLS
        inline bool IsNodeActive( GraphContext& context, GraphNodeIndex nodeIdx ) const
        {
            KRG_ASSERT( IsValidNodeIndex( nodeIdx ) );
            auto pNode = m_nodes[nodeIdx];
            return pNode->IsNodeActive( context );
        }

        inline PoseNodeDebugInfo GetPoseNodeDebugInfo( GraphContext& context, GraphNodeIndex nodeIdx ) const
        {
            KRG_ASSERT( IsValidNodeIndex( nodeIdx ) );
            KRG_ASSERT( m_nodes[nodeIdx]->GetValueType() == GraphValueType::Pose );
            auto pNode = static_cast<PoseNode const*>( m_nodes[nodeIdx] );
            return pNode->GetDebugInfo();
        }

        template<typename T>
        inline T GetRuntimeNodeValue( GraphContext& context, GraphNodeIndex nodeIdx ) const
        {
            KRG_ASSERT( IsValidNodeIndex( nodeIdx ) );
            auto pValueNode = static_cast<ValueNode*>( const_cast<GraphNode*>( m_nodes[nodeIdx] ) );
            return pValueNode->GetValue<T>( context );
        }
        #endif

    private:

        KRG_FORCE_INLINE bool IsControlParameter( GraphNodeIndex nodeIdx ) const 
        {
            return nodeIdx < m_pGraphVariation->m_pGraphDefinition->m_numControlParameters;
        }

        GraphInstance( GraphInstance const& ) = delete;
        GraphInstance( GraphInstance&& ) = delete;
        GraphInstance& operator=( GraphInstance const& ) = delete;
        GraphInstance& operator=( GraphInstance&& ) = delete;

    private:

        GraphVariation const* const             m_pGraphVariation = nullptr;
        TVector<GraphNode*>                     m_nodes;
        Byte*                                   m_pAllocatedInstanceMemory = nullptr;
        PoseNode*                               m_pRootNode = nullptr;
    };
}
