#pragma once
#include "Animation_EditorGraph_FlowGraph.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Resources.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    enum NodeCompilationState
    {
        NeedCompilation,
        AlreadyCompiled,
    };

    //-------------------------------------------------------------------------

    class EditorGraphCompilationContext : public GraphDefinition
    {
        friend class AnimationGraphCompiler;
        friend class EditorGraphDefinition;

    public:

        struct LogEntry
        {
            LogEntry( Log::Severity severity, UUID const& nodeID, String const& message )
                : m_message( message )
                , m_nodeID( nodeID )
                , m_severity( severity )
            {}

            String              m_message;
            UUID                m_nodeID;
            Log::Severity       m_severity;
        };

    public:

        EditorGraphCompilationContext();
        ~EditorGraphCompilationContext();

        inline TVector<LogEntry> const& GetLog() const { return m_log; }

        inline THashMap<UUID, GraphNodeIndex> GetIDToIndexMap() const { return m_nodeToIndexMap; }

        void LogMessage( VisualGraph::BaseNode const* pNode, String const& message ) { m_log.emplace_back( LogEntry( Log::Severity::Message, pNode->GetID(), message ) ); }
        void LogWarning( VisualGraph::BaseNode const* pNode, String const& message ) { m_log.emplace_back( LogEntry( Log::Severity::Warning, pNode->GetID(), message ) ); }
        void LogError( VisualGraph::BaseNode const* pNode, String const& message ) { m_log.emplace_back( LogEntry( Log::Severity::Error, pNode->GetID(), message ) ); }

        // General Compilation
        //-------------------------------------------------------------------------

        // Try to get the runtime settings for a node in the graph, will return whether this node was already compiled or still needs compilation
        template<typename T>
        NodeCompilationState GetSettings( VisualGraph::BaseNode const* pNode, typename T::Settings*& pOutSettings )
        {
            auto foundIter = m_nodeToIndexMap.find( pNode->GetID() );
            if ( foundIter != m_nodeToIndexMap.end() )
            {
                pOutSettings = (T::Settings*) m_nodeSettings[foundIter->second];
                return NodeCompilationState::AlreadyCompiled;
            }

            //-------------------------------------------------------------------------

            KRG_ASSERT( m_nodeSettings.size() < 0xFFFF );
            pOutSettings = KRG::New<T::Settings>();
            m_nodeSettings.emplace_back( pOutSettings );
            m_compiledNodePaths.emplace_back( pNode->GetPathFromRoot() );
            pOutSettings->m_nodeIdx = GraphNodeIndex( m_nodeSettings.size() - 1 );

            // Add to map
            m_nodeToIndexMap.insert( TPair<UUID, GraphNodeIndex>( pNode->GetID(), pOutSettings->m_nodeIdx ) );

            // Add to persistent nodes list
            auto pFlowNode = TryCast<GraphNodes::EditorGraphNode>( pNode );
            if ( pFlowNode != nullptr && pFlowNode->IsPersistentNode() )
            {
                m_persistentNodeIndices.emplace_back( pOutSettings->m_nodeIdx );
            }

            // Update instance requirements
            m_instanceRequiredAlignment = Math::Max( m_instanceRequiredAlignment, (uint32) alignof( T ) );
            size_t const requiredPadding = Memory::CalculatePaddingForAlignment( m_currentNodeMemoryOffset, alignof( T ) );
            m_currentNodeMemoryOffset += uint32( sizeof( T ) + requiredPadding );
            m_instanceNodeStartOffsets.emplace_back( m_currentNodeMemoryOffset );

            return NodeCompilationState::NeedCompilation;
        }

        // This will return an index that can be used to look up the data resource at runtime
        inline DataSetSlotIndex RegisterSlotNode( UUID const& nodeID )
        {
            KRG_ASSERT( !VectorContains( m_registeredDataSlots, nodeID ) );

            DataSetSlotIndex slotIdx = (DataSetSlotIndex) m_registeredDataSlots.size();
            m_registeredDataSlots.emplace_back( nodeID );
            return slotIdx;
        }

        // State Machine Compilation
        //-------------------------------------------------------------------------

        // Start compilation of a transition conduit
        inline void BeginConduitCompilation( GraphNodeIndex sourceStateNodeIdx )
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx == InvalidIndex );
            KRG_ASSERT( sourceStateNodeIdx != InvalidIndex );
            m_conduitSourceStateCompiledNodeIdx = sourceStateNodeIdx;
        }

        // End compilation of a transition conduit
        inline void EndConduitCompilation()
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx != InvalidIndex );
            m_conduitSourceStateCompiledNodeIdx = InvalidIndex;
        }

        // Some nodes optionally need the conduit index so we need to have a way to know what mode we are in
        inline GraphNodeIndex IsCompilingConduit() const
        {
            return m_conduitSourceStateCompiledNodeIdx != InvalidIndex;
        }

        inline GraphNodeIndex GetConduitSourceStateIndex() const 
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx != InvalidIndex );
            return m_conduitSourceStateCompiledNodeIdx; 
        }

        // Start compilation of a transition conduit
        inline void BeginTransitionConditionsCompilation( Seconds transitionDuration, GraphNodeIndex transitionDurationOverrideIdx )
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx != InvalidIndex );
            m_transitionDuration = transitionDuration;
            m_transitionDurationOverrideIdx = transitionDurationOverrideIdx;
        }

        // End compilation of a transition conduit
        inline void EndTransitionConditionsCompilation()
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx != InvalidIndex );
            m_transitionDuration = 0;
            m_transitionDurationOverrideIdx = InvalidIndex;
        }

        inline GraphNodeIndex GetCompiledTransitionDurationOverrideIdx() const
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx != InvalidIndex );
            return m_transitionDurationOverrideIdx;
        }

        inline Seconds GetCompiledTransitionDuration() const
        {
            KRG_ASSERT( m_conduitSourceStateCompiledNodeIdx != InvalidIndex );
            return m_transitionDuration;
        }

        // Serialization
        //-------------------------------------------------------------------------

        void SerializeSettings( cereal::BinaryOutputArchive& settingsArchive );

    private:

        THashMap<UUID, GraphNodeIndex>          m_nodeToIndexMap;
        TVector<String>                         m_compiledNodePaths;
        uint32                                  m_currentNodeMemoryOffset = 0;
        TVector<LogEntry>                       m_log;
        TVector<UUID>                           m_registeredDataSlots;
        GraphNodeIndex                          m_conduitSourceStateCompiledNodeIdx = InvalidIndex;
        Seconds                                 m_transitionDuration = 0;
        GraphNodeIndex                          m_transitionDurationOverrideIdx = InvalidIndex;
    };
}