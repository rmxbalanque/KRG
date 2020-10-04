#pragma once
#include "UpdateStage.h"
#include "System/Core/Math/ViewVolume.h"
#include "System/Core/Types/Time.h"
#include "System/Core/Debug/DebugDrawingSystem.h"
#include "System/Core/Systems/SystemRegistry.h"

//-------------------------------------------------------------------------
// The base update context for anything in the engine that needs to be updated
//-------------------------------------------------------------------------

namespace KRG
{
    class UpdateContext
    {

    public:

        inline Seconds GetDeltaTime() const { return m_deltaTime * m_timeScale; }
        inline Seconds GetRawDeltaTime() const { return m_deltaTime; }
        inline U64 GetFrameID() const { return m_frameID; }

        inline UpdateStage GetUpdateStage() const { return m_stage; }
        template<typename T> inline T* GetSystem() const { return m_pSystemRegistry->GetSystem<T>(); }

        #if KRG_DEBUG_INSTRUMENTATION
        inline Debug::DrawingContext GetDrawingContext() const { return m_pDebugDrawingSystem->GetDrawingContext(); }
        #endif

    protected:

        // Set the time scale for this update ( only positive values under 100 allowed )
        inline void SetTimeScale( F32 scale )
        {
            KRG_ASSERT( scale > 0 && scale < 100 );
            m_timeScale = scale;
        }

        // Set the time delta for this update
        inline void SetTimeDelta( Seconds deltaTime )
        {
            m_timeScale = 1.0f;
            m_deltaTime = deltaTime;
            m_frameID++;

            // Ensure we dont get crazy time delta's when we hit breakpoints
            #if !KRG_CONFIGURATION_FINAL
            if ( m_deltaTime > 1.0f )
            {
                m_deltaTime = 0.03333f; // 30 FPS
            }
            #endif
        }

    protected:

        Seconds                                     m_deltaTime = 1.0f / 60.0f;
        F32                                         m_timeScale = 1.0f;
        U64                                         m_frameID = 0;
        UpdateStage                                 m_stage = UpdateStage::FrameStart;
        SystemRegistry*                             m_pSystemRegistry = nullptr;

        #if KRG_DEBUG_INSTRUMENTATION
        Debug::DrawingSystem*                       m_pDebugDrawingSystem = nullptr;
        #endif
    };
}
