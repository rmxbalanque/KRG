#pragma once
#include "Engine/Animation/Graph/AnimationGraphResources.h"
#include "System/Core/Logging/Log.h"
#include "AnimationGraphTools_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    enum NodeCompilationState
    {
        NeedCompilation,
        AlreadyCompiled,
    };

    //-------------------------------------------------------------------------

    class ToolsGraphCompilationContext : public AnimationGraphDefinition
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

        ~ToolsGraphCompilationContext();

        inline TVector<LogEntry> const& GetLog() const { return m_log; }

        void LogMessage( ToolsNode const* pNode, String const& message ) { m_log.emplace_back( LogEntry( Log::Severity::Message, pNode->GetID(), message ) ); }
        void LogWarning( ToolsNode const* pNode, String const& message ) { m_log.emplace_back( LogEntry( Log::Severity::Warning, pNode->GetID(), message ) ); }
        void LogError( ToolsNode const* pNode, String const& message ) { m_log.emplace_back( LogEntry( Log::Severity::Error, pNode->GetID(), message ) ); }

        template<typename T>
        NodeCompilationState GetSettings( ToolsNode const* pNode, typename T::Settings*& pOutSettings )
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
            if ( pNode->IsPersistentNode() )
            {
                m_persistentNodeIndices.emplace_back( pOutSettings->m_nodeIdx );
            }

            // Update instance requirements
            m_instanceRequiredAlignment = Math::Max( m_instanceRequiredAlignment, (uint32) alignof( T ) );
            size_t const requiredPadding = Memory::CalculatePaddingForAlignment( m_currentNodeMemoryOffset, alignof( T ) );
            m_currentNodeMemoryOffset += uint32( sizeof( T ) + requiredPadding );

            return NodeCompilationState::NeedCompilation;
        }

        // This will return an index that can be used to look up the animation clip at runtime
        inline DataSetSourceIndex GetAnimationClipSourceIndex( UUID const& nodeID )
        {
            KRG_ASSERT( m_nodeToAnimationClipSourceIdxMap.find( nodeID ) == m_nodeToAnimationClipSourceIdxMap.end() );

            DataSetSourceIndex newIdx = m_animationClipCount;
            m_nodeToAnimationClipSourceIdxMap.insert( TPair<UUID, DataSetSourceIndex>( nodeID, newIdx ) );
            m_animationClipCount++;
            return newIdx;
        }

        void SerializeSettings( cereal::BinaryOutputArchive& settingsArchive );

    private:

        THashMap<UUID, NodeIndex>               m_nodeToIndexMap;
        uint32                                  m_currentNodeMemoryOffset = 0;
        TVector<LogEntry>                       m_log;

        THashMap<UUID, DataSetSourceIndex>      m_nodeToAnimationClipSourceIdxMap;
        DataSetSourceIndex                      m_animationClipCount = 0;
    };
}