#pragma once
#include "Engine/Animation/Graph/AnimationGraphResources.h"
#include "System/Core/Logging/Log.h"
#include "AnimationGraphTools_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    enum NodeCompilationState
    {
        NeedCompilation,
        AlreadyCompiled,
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_ANIMATION_API ToolsGraphCompilationContext : public AnimationGraphDefinition
    {
        friend class AnimationGraphCompiler;
        friend class ToolsAnimationGraph;

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

        ToolsGraphCompilationContext();
        ~ToolsGraphCompilationContext();

        inline TVector<LogEntry> const& GetLog() const { return m_log; }

        void LogMessage( GraphEditor::BaseNode const* pNode, String const& message ) { m_log.emplace_back( LogEntry( Log::Severity::Message, pNode->GetID(), message ) ); }
        void LogWarning( GraphEditor::BaseNode const* pNode, String const& message ) { m_log.emplace_back( LogEntry( Log::Severity::Warning, pNode->GetID(), message ) ); }
        void LogError( GraphEditor::BaseNode const* pNode, String const& message ) { m_log.emplace_back( LogEntry( Log::Severity::Error, pNode->GetID(), message ) ); }

        template<typename T>
        NodeCompilationState GetSettings( GraphEditor::BaseNode const* pNode, typename T::Settings*& pOutSettings )
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
            pOutSettings->m_nodeIdx = NodeIndex( m_nodeSettings.size() - 1 );

            // Add to map
            m_nodeToIndexMap.insert( TPair<UUID, NodeIndex>( pNode->GetID(), pOutSettings->m_nodeIdx ) );

            // Add to persistent nodes list
            auto pFlowNode = TryCast<FlowToolsNode>( pNode );
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

        void SerializeSettings( cereal::BinaryOutputArchive& settingsArchive );

    private:

        THashMap<UUID, NodeIndex>               m_nodeToIndexMap;
        uint32                                  m_currentNodeMemoryOffset = 0;
        TVector<LogEntry>                       m_log;
        TVector<UUID>                           m_registeredDataSlots;
    };
}