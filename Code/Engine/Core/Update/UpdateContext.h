#pragma once
#include "Engine/Core/_Module/API.h"
#include "UpdateStage.h"
#include "System/Core/Math/ViewVolume.h"
#include "System/Core/Time/Time.h"
#include "System/Core/Drawing/DebugDrawingSystem.h"
#include "System/Core/Systems/SystemRegistry.h"

//-------------------------------------------------------------------------
// The base update context for anything in the engine that needs to be updated
//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_ENGINE_CORE_API UpdateContext
    {
        friend class EngineDevUI;

    public:

        KRG_FORCE_INLINE Seconds GetDeltaTime() const { return m_deltaTime; }
        KRG_FORCE_INLINE uint64 GetFrameID() const { return m_frameID; }
        KRG_FORCE_INLINE UpdateStage GetUpdateStage() const { return m_stage; }
        
        template<typename T> 
        KRG_FORCE_INLINE T* GetSystem() const { return m_pSystemRegistry->GetSystem<T>(); }

    protected:

        // Set the time delta for this update
        inline void UpdateDeltaTime( Milliseconds deltaTime )
        {
            KRG_ASSERT( deltaTime >= 0 );

            // Update internal time
            m_deltaTime = deltaTime.ToSeconds();
            m_frameID++;
        }

    protected:

        Seconds                                     m_deltaTime = 1.0f / 60.0f;
        uint64                                      m_frameID = 0;
        UpdateStage                                 m_stage = UpdateStage::FrameStart;
        SystemRegistry*                             m_pSystemRegistry = nullptr;
    };
}
