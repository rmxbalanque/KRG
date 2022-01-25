#pragma once

#include "Animation_RuntimeGraph_Common.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Time/Time.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Types/Pool.h"

//-------------------------------------------------------------------------

namespace KRG::Drawing { class DrawContext; }

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Animation
{
    class RootMotionRecorder
    {
        constexpr static int32 const s_recordingBufferSize = 300;

    public:

        enum class ActionType : uint16
        {
            Unknown = 0,
            Sample,
            Modification,
            Blend,
        };

        struct RecordedAction
        {
            RecordedAction( GraphNodeIndex nodeIdx, ActionType actionType, Transform const& rootMotionDelta )
                : m_rootMotionDelta( rootMotionDelta )
                , m_nodeIdx( nodeIdx )
                , m_actionType( actionType )
            {}

            Transform                   m_rootMotionDelta;
            GraphNodeIndex              m_nodeIdx;
            ActionType                  m_actionType;
            TInlineVector<int16, 2>     m_dependencies;
        };

        struct RecordedPosition
        {
            Transform                   m_expectedTransform;
            Transform                   m_actualTransform;
        };

        enum class DebugMode
        {
            Off = 0,
            DrawRoot,
            DrawRecordedRootMotion,
            DrawRecordedRootMotionAdvanced,
        };

    public:

        RootMotionRecorder();

        inline TVector<RecordedAction> const& GetRecordedActions() const { return m_recordedActions; }
        void StartCharacterUpdate( Transform const& characterWorldTransform );
        void EndCharacterUpdate( Transform const& characterWorldTransform );

        void SetDebugMode( DebugMode mode );
        DebugMode GetDebugMode() const { return m_debugMode; }
        void DrawDebug( Drawing::DrawContext& drawingContext );

        //-------------------------------------------------------------------------

        inline bool HasRecordedActions() const { return !m_recordedActions.empty(); }

        KRG_FORCE_INLINE int16 GetLastActionIndex() const { return (int16) m_recordedActions.size() - 1; }

        KRG_FORCE_INLINE int16 RecordSampling( GraphNodeIndex nodeIdx, Transform const& rootMotionDelta )
        {
            KRG_ASSERT( nodeIdx != InvalidIndex );
            int16 const idx = (int16) m_recordedActions.size();
            m_recordedActions.emplace_back( nodeIdx, ActionType::Sample, rootMotionDelta);
            return idx;
        }

        KRG_FORCE_INLINE int16 RecordModification( GraphNodeIndex nodeIdx, Transform const& rootMotionDelta )
        {
            KRG_ASSERT( nodeIdx != InvalidIndex );
            int16 const previousIdx = GetLastActionIndex();
            KRG_ASSERT( previousIdx >= 0 );
            int16 const idx = (int16) m_recordedActions.size();
            auto& action = m_recordedActions.emplace_back( nodeIdx, ActionType::Modification, rootMotionDelta );
            action.m_dependencies.emplace_back( previousIdx );
            return idx;
        }

        // Blend operations automatically pop a blend context
        KRG_FORCE_INLINE int16 RecordBlend( GraphNodeIndex nodeIdx, int16 originalRootMotionActionIdx0, int16 originalRootMotionActionIdx1, Transform const& rootMotionDelta )
        {
            KRG_ASSERT( nodeIdx != InvalidIndex );
            int16 const idx = (int16) m_recordedActions.size();
            auto& action = m_recordedActions.emplace_back( nodeIdx, ActionType::Blend, rootMotionDelta );
            action.m_dependencies.emplace_back( originalRootMotionActionIdx0 );

            // It's possible for the use to only have a single source for a blend since not all nodes register root motion actions
            if ( originalRootMotionActionIdx0 == originalRootMotionActionIdx1 )
            {
                action.m_dependencies.emplace_back( InvalidIndex );
            }
            else
            {
                KRG_ASSERT( originalRootMotionActionIdx1 > originalRootMotionActionIdx0 );
                action.m_dependencies.emplace_back( originalRootMotionActionIdx1 );
            }

            return idx;
        }

    private:

        TVector<RecordedAction>         m_recordedActions;
        Transform                       m_startWorldTransform;
        Transform                       m_endWorldTransform;
        TVector<RecordedPosition>       m_recordedRootTransforms; // Circular buffer
        int32                           m_freeBufferIdx = 0;
        DebugMode                       m_debugMode = DebugMode::Off;
    };
}
#endif