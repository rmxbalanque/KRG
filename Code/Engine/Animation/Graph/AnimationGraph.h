#pragma once
#include "System/Resource/IResource.h"
#include "AnimationGraphContexts.h"
#include "AnimationGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    using namespace Graph;

    //-------------------------------------------------------------------------

    class AnimationGraph : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'AG' );
        KRG_SERIALIZE_NONE();

        friend class AnimationGraphCompiler;
        friend class AnimationGraphLoader;
        friend class AnimationGraphInstance;

    public:

        virtual bool IsValid() const override { return true; }

    private:

        TVector<Graph::GraphNode::Settings*>        m_nodeSettings;
        TVector<Graph::NodeIndex>                   m_persistentNodes;
        int32                                       m_numControlParameters = 0;
        NodeIndex                                   m_rootNodeIdx = InvalidIndex;
    };

    //-------------------------------------------------------------------------

    class AnimationGraphInstance
    {

    public:

        AnimationGraphInstance( AnimationGraph const* pGraph );
        ~AnimationGraphInstance();

        void Initialize( GraphContext& context );
        void Shutdown( GraphContext& context );

        // Graph State
        //-------------------------------------------------------------------------

        // Is this a valid instance that has been correctly initialized
        bool IsInitialized() const { return m_pRootNode != nullptr && m_pRootNode->IsValid(); }

        // Fully reset all nodes in the graph
        void Reset( GraphContext& context );

        // Run the graph logic
        UpdateResult UpdateGraph( GraphContext& context );

        // General Node Info
        //-------------------------------------------------------------------------

        inline bool IsValidNodeIndex( NodeIndex nodeIdx ) const 
        {
            return nodeIdx < m_pGraph->m_nodeSettings.size();
        }

        // Control Parameters
        //-------------------------------------------------------------------------

        inline int32 GetNumControlParameters() const { return m_pGraph->m_numControlParameters; }

        inline NodeIndex GetControlParameterIndex( StringID parameterID ) const
        {
            for ( NodeIndex i = 0; i < m_pGraph->m_numControlParameters; i++ )
            {
                if ( m_pGraph->m_nodeSettings[i]->m_nodeID == parameterID )
                {
                    return i;
                }
            }

            return InvalidIndex;
        }

        inline ValueNodeType GetControlParameterType( NodeIndex parameterNodeIdx )
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

    private:

        KRG_FORCE_INLINE bool IsControlParameter( NodeIndex nodeIdx ) const { return nodeIdx < m_pGraph->m_numControlParameters; }

        AnimationGraphInstance( AnimationGraphInstance const& ) = delete;
        AnimationGraphInstance( AnimationGraphInstance&& ) = delete;
        AnimationGraphInstance& operator=( AnimationGraphInstance const& ) = delete;
        AnimationGraphInstance& operator=( AnimationGraphInstance&& ) = delete;

    private:

        AnimationGraph const* const             m_pGraph = nullptr;
        TVector<GraphNode*>                     m_nodes;
        void*                                   m_pAllocatedInstanceMemory = nullptr;
        AnimationNode*                          m_pRootNode = nullptr;
    };
}
