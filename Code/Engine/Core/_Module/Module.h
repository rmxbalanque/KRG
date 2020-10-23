#pragma once

#include "API.h"
#include "Engine/Core/Modules/IEngineModule.h"
#include "Engine/Core/DebugUI/DebugViews/EntityDebugViewController.h"
#include "Engine/Core/DebugUI/DebugViews/InputDebugViewController.h"
#include "Engine/Core/DebugUI/DebugViews/SystemDebugViewController.h"
#include "Engine/Core/Camera/CameraDebugViewController.h"
#include "System/Imgui/Renderer/ImguiRenderer.h"
#include "System/Entity/Map/EntityMapLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Engine
    {
        class KRG_ENGINE_CORE_API EngineModule : public IModule
        {
            KRG_REGISTER_MODULE;

        public:

            virtual bool Initialize( ModuleContext& context ) final;
            virtual void Shutdown( ModuleContext& context ) final;

        private:

            EntityModel::EntityMapLoader                    m_mapLoader;

            ImGuiX::ImguiRenderer                           m_imguiRenderer;

            //-------------------------------------------------------------------------

            #if KRG_DEBUG_INSTRUMENTATION
            Debug::SystemDebugViewController                m_systemDebugViewController;
            CameraDebugViewController                       m_cameraDebugViewController;
            EntityDebugViewController                       m_entityDebugViewController;
            Input::InputDebugViewController                 m_inputDebugViewController;
            #endif

            bool                                            m_initialized = false;
        };
    }
}