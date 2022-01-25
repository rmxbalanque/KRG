#include "Animation_RuntimeGraph_RootMotionRecorder.h"
#include "System/Animation/AnimationSkeleton.h"
#include "System/Core/Drawing/DebugDrawing.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Animation
{
    RootMotionRecorder::RootMotionRecorder()
    {
        m_recordedRootTransforms.reserve( s_recordingBufferSize );
    }

    void RootMotionRecorder::SetDebugMode( DebugMode mode )
    {
        // If we started recording, reset the transform buffers
        if ( m_debugMode < DebugMode::DrawRecordedRootMotion && mode >= DebugMode::DrawRecordedRootMotion )
        {
            m_freeBufferIdx = 0;
            m_recordedRootTransforms.clear();
        }

        m_debugMode = mode;
    }

    void RootMotionRecorder::StartCharacterUpdate( Transform const& characterWorldTransform )
    {
        m_startWorldTransform = characterWorldTransform;
        m_recordedActions.clear();
    }

    void RootMotionRecorder::EndCharacterUpdate( Transform const& characterWorldTransform )
    {
        m_endWorldTransform = characterWorldTransform;

        //-------------------------------------------------------------------------

        if ( m_debugMode >= DebugMode::DrawRecordedRootMotion )
        {
            KRG_ASSERT( m_freeBufferIdx < s_recordingBufferSize );

            // If we havent filled the entire buffer, add a new element
            if ( m_freeBufferIdx == m_recordedRootTransforms.size() )
            {
                m_recordedRootTransforms.push_back();
            }

            // Record root transforms
            if ( m_recordedActions.empty() )
            {
                m_recordedRootTransforms[m_freeBufferIdx].m_actualTransform = m_endWorldTransform;
                m_recordedRootTransforms[m_freeBufferIdx].m_expectedTransform = m_endWorldTransform;
            }
            else
            {
                m_recordedRootTransforms[m_freeBufferIdx].m_actualTransform = m_endWorldTransform;
                m_recordedRootTransforms[m_freeBufferIdx].m_expectedTransform = m_recordedActions.back().m_rootMotionDelta * m_startWorldTransform;
            }

            // Update circular buffer indices
            //-------------------------------------------------------------------------

            m_freeBufferIdx++;
            if ( m_freeBufferIdx >= s_recordingBufferSize )
            {
                m_freeBufferIdx = 0;
            }
        }
    }

    void RootMotionRecorder::DrawDebug( Drawing::DrawContext& drawingContext )
    {
        if ( m_debugMode == DebugMode::DrawRoot )
        {
            DrawRootBone( drawingContext, m_endWorldTransform );
        }
        else if ( m_debugMode == DebugMode::DrawRecordedRootMotion || m_debugMode == DebugMode::DrawRecordedRootMotionAdvanced )
        {
            bool const isAdvancedDisplayEnabled = m_debugMode == DebugMode::DrawRecordedRootMotionAdvanced;

            auto DrawSegment = [this, &drawingContext, isAdvancedDisplayEnabled] ( int32 idx0, int32 idx1, int32 c )
            {
                static Vector const verticalOffset( 0, 0, 0.2f );
                static Color const actualColors[2] = { Colors::Blue, Colors::Red };
                static Color const expectedColors[2] = { Colors::Cyan, Colors::Magenta };

                drawingContext.DrawLine( m_recordedRootTransforms[idx0].m_actualTransform.GetTranslation(), m_recordedRootTransforms[idx1].m_actualTransform.GetTranslation(), actualColors[c], 3.0f );
                drawingContext.DrawAxis( m_recordedRootTransforms[idx1].m_actualTransform, 0.1f, 2.0f );

                if ( isAdvancedDisplayEnabled )
                {
                    drawingContext.DrawLine( m_recordedRootTransforms[idx0].m_expectedTransform.GetTranslation() + verticalOffset, m_recordedRootTransforms[idx1].m_expectedTransform.GetTranslation() + verticalOffset, expectedColors[c], 3.0f );

                    Transform axisTransform = m_recordedRootTransforms[idx1].m_expectedTransform;
                    axisTransform.AddTranslation( verticalOffset );
                    drawingContext.DrawAxis( axisTransform, 0.1f, 2.0f );
                }
            };

            //-------------------------------------------------------------------------

            int32 const numRecordedTransforms = (int32) m_recordedRootTransforms.size();
            bool const isBufferFull = ( numRecordedTransforms == s_recordingBufferSize );
            if ( !isBufferFull )
            {
                for ( int32 i = 1; i < m_freeBufferIdx; i++ )
                {
                    int32 const c = Math::IsEven( i ) ? 0 : 1;
                    DrawSegment( i - 1, i, c );
                }
            }
            else // Draw all transforms
            {
                // Draw all the transforms from the start buffer to either the end of the array or the first free idx
                for ( int32 i = m_freeBufferIdx + 1; i < numRecordedTransforms; i++ )
                {
                    int32 const c = Math::IsEven( i ) ? 0 : 1;
                    DrawSegment( i - 1, i, c );
                }

                // Draw looped range
                for ( int32 i = 1; i < m_freeBufferIdx; i++ )
                {
                    int32 const c = Math::IsEven( i ) ? 0 : 1;
                    DrawSegment( i - 1, i, c );
                }

                // Connect the two ranges
                if ( m_freeBufferIdx != 0 )
                {
                    DrawSegment( 0, s_recordingBufferSize - 1, 0 );
                }
            }
        }
    }
}
#endif