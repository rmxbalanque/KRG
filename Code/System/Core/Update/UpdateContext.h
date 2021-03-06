#pragma once
#include "UpdateStage.h"
#include "System/Core/Math/ViewVolume.h"
#include "System/Core/Time/Time.h"
#include "System/Core/Debug/DebugDrawingSystem.h"
#include "System/Core/Systems/SystemRegistry.h"

//-------------------------------------------------------------------------
// The base update context for anything in the engine that needs to be updated
//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_CORE_API UpdateContext
    {

    public:

        inline Seconds GetDeltaTime() const { return m_deltaTime * m_timeScale; }
        inline Seconds GetRawDeltaTime() const { return m_deltaTime; }
        inline uint64 GetFrameID() const { return m_frameID; }

        inline UpdateStage GetUpdateStage() const { return m_stage; }
        template<typename T> inline T* GetSystem() const { return m_pSystemRegistry->GetSystem<T>(); }

        #if KRG_DEVELOPMENT_TOOLS
        inline Debug::DrawingContext GetDrawingContext() const { return m_pDebugDrawingSystem->GetDrawingContext(); }
        #endif

    protected:

        // Set the time scale for this update ( only positive values under 100 allowed )
        void SetTimeScale( float scale );

        // Set the time delta for this update
        void UpdateDeltaTime( Milliseconds deltaTime );

    protected:

        Seconds                                     m_deltaTime = 1.0f / 60.0f;
        float                                         m_timeScale = 1.0f;
        uint64                                         m_frameID = 0;
        UpdateStage                                 m_stage = UpdateStage::FrameStart;
        SystemRegistry*                             m_pSystemRegistry = nullptr;

        #if KRG_DEVELOPMENT_TOOLS
        Debug::DrawingSystem*                       m_pDebugDrawingSystem = nullptr;
        #endif
    };
}
