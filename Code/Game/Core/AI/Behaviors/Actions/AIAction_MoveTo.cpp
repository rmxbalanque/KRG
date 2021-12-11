#include "AIAction_MoveTo.h"
#include "Game/Core/AI/Behaviors/AIBehavior.h"
#include "Game/Core/AI/GraphControllers/AIGraphController_Locomotion.h"
#include "Engine/Navmesh/Systems/WorldSystem_Navmesh.h"
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "Engine/Navmesh/NavPower.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    bool MoveToAction::IsRunning() const
    {
        return m_path.IsValid();
    }

    void MoveToAction::Start( BehaviorContext const& ctx, Vector const& goalPosition )
    {
        auto spaceHandle = ctx.m_pNavmeshSystem->GetSpaceHandle();

        bfx::PathSpec pathSpec;
        pathSpec.m_snapMode = bfx::SNAP_CLOSEST;

        bfx::PathCreationOptions pathOptions;
        pathOptions.m_forceFirstPosOntoNavGraph = true;

        m_path = bfx::CreatePolylinePath( spaceHandle, Navmesh::ToBfx( ctx.m_pCharacter->GetPosition() ), Navmesh::ToBfx( goalPosition ), 0, pathSpec, pathOptions );
        if ( m_path.IsValid() )
        {
            m_currentPathSegmentIdx = 0;
            m_progressAlongSegment = 0.0f;
        }
        else
        {
            m_currentPathSegmentIdx = InvalidIndex;
        }
    }

    void MoveToAction::Update( BehaviorContext const& ctx )
    {
        if ( !m_path.IsValid() )
        {
            return;
        }


        float const moveSpeed = 5.5f;
        float distanceToMove = moveSpeed * ctx.GetDeltaTime();

        //-------------------------------------------------------------------------

        Vector facingDir = ctx.m_pCharacter->GetForwardVector();
        Vector goalPosition;

        bool atEndOfPath = false;
        while ( distanceToMove > 0 )
        {
            bool const isLastSegment = m_currentPathSegmentIdx == ( m_path.GetNumSegments() - 1 );

            bfx::SurfaceSegment const* pSegment = m_path.GetSurfaceSegment( m_currentPathSegmentIdx );
            Vector segmentStart( Navmesh::FromBfx( pSegment->GetStartPos() ) );
            Vector segmentEnd( Navmesh::FromBfx( pSegment->GetEndPos() ) );

            // Handle zero length segments
            Vector const segmentVector( segmentEnd - segmentStart );
            if ( segmentVector.IsZero3() )
            {
                distanceToMove = 0.0f;
                m_progressAlongSegment = 1.0f;
                goalPosition = segmentEnd;
                break;
            }

            //-------------------------------------------------------------------------

            Vector segmentDir;
            float segmentLength;
            ( segmentEnd - segmentStart ).ToDirectionAndLength3( segmentDir, segmentLength );

            KRG_ASSERT( segmentDir.IsNormalized3() );

            float currentDistanceAlongSegment = segmentLength * m_progressAlongSegment;
            float remainingDistance = segmentLength - currentDistanceAlongSegment;

            //-------------------------------------------------------------------------

            float newDistanceAlongSegment = currentDistanceAlongSegment + distanceToMove;
            if ( !isLastSegment && newDistanceAlongSegment > segmentLength )
            {
                distanceToMove -= remainingDistance;
                m_progressAlongSegment = 0.0f;
                m_currentPathSegmentIdx++;
            }
            else // Perform full move
            {
                distanceToMove = 0.0f;
                m_progressAlongSegment = Math::Min( newDistanceAlongSegment / segmentLength, 1.0f );
                goalPosition = Vector::Lerp( segmentStart, segmentEnd, m_progressAlongSegment );
                facingDir = segmentDir;

                if ( isLastSegment && m_progressAlongSegment == 1.0f )
                {
                    atEndOfPath = true;
                }
            }
        }

        // Calculate goal pos
        //-------------------------------------------------------------------------

        Vector const currentPosition = ctx.m_pCharacter->GetPosition();
        Vector const desiredDelta = ( goalPosition - currentPosition );
        Vector const headingVelocity = desiredDelta / ctx.GetDeltaTime();

        auto pLocomotionController = ctx.m_pAnimationController->GetSubGraphController<LocomotionGraphController>();
        pLocomotionController->SetLocomotionDesires( ctx.GetDeltaTime(), headingVelocity, facingDir.GetNormalized2() );

        // Check if we are at the end of the path
        //-------------------------------------------------------------------------

        if ( atEndOfPath )
        {
            m_path.Release();
        }
    }
}
