#pragma once
#include "Engine/Navmesh/NavPower.h"
#include "System/Core/Math/Vector.h"
#include "System/Core/Types/Percentage.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    struct BehaviorContext;

    //-------------------------------------------------------------------------

    // Todo: think about more behavior centric name: MoveToChildBehavior, MoveToSubBehavior, MoveToActionBehavior??
    class MoveToAction
    {
    public:

        bool IsRunning() const;
        void Start( BehaviorContext const& ctx, Vector const& goalPosition );
        void Update( BehaviorContext const& ctx );

    private:

        #if KRG_ENABLE_NAVPOWER
        bfx::PolylinePathRCPtr      m_path;
        #endif

        int32                       m_currentPathSegmentIdx = InvalidIndex;
        Percentage                  m_progressAlongSegment = 0.0f;
    };
}