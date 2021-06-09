#include "AnimationGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    GraphNode::~GraphNode()
    {
        KRG_ASSERT( m_initializationCount == 0 );
    }

    void GraphNode::Initialize( GraphContext& Context )
    {
        if ( IsInitialized() )
        {
            ++m_initializationCount;
        }
        else
        {
            InitializeInternal( Context );
        }
    }

    void GraphNode::Shutdown( GraphContext& Context )
    {
        KRG_ASSERT( IsInitialized() );
        if ( --m_initializationCount == 0 )
        {
            ShutdownInternal( Context );
        }
    }

    void GraphNode::MarkNodeActive( GraphContext& context )
    {
        m_lastUpdateID = context.m_updateID;

        #if KRG_DEVELOPMENT_TOOLS
        context.TrackActiveNode( GetNodeIdx() );
        #endif
    }

    void GraphNode::InitializeInternal( GraphContext& context )
    {
        KRG_ASSERT( !IsInitialized() );
        ++m_initializationCount;
    }

    void GraphNode::ShutdownInternal( GraphContext& context )
    {
        KRG_ASSERT( !IsInitialized() );
        m_lastUpdateID = 0xFFFFFFFF;
    }

    //-------------------------------------------------------------------------

    void PoseNode::Initialize( GraphContext& context, SyncTrackTime const& initialTime )
    {
        if ( IsInitialized() )
        {
            ++m_initializationCount;
        }
        else
        {
            InitializeInternal( context, initialTime );
        }
    }

    void PoseNode::InitializeInternal( GraphContext& context, SyncTrackTime const& initialTime )
    {
        GraphNode::InitializeInternal( context );

        // Reset node state
        m_loopCount = 0;
        m_duration = 0.0f;
        m_previousTime = 0.0f;
        m_currentTime = m_previousTime;
    }
}