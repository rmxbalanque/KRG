#pragma once
#include "_Module/API.h"

//-------------------------------------------------------------------------

struct Renderer;

//-------------------------------------------------------------------------

namespace KRG::Render
{
    KRG_SYSTEM_RENDER_API void InitializeRenderSystem();
    KRG_SYSTEM_RENDER_API void ShutdownRenderSystem();

    //-------------------------------------------------------------------------

    class Settings;

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_RENDER_API RenderDeviceForge
    {
    public:

        RenderDeviceForge() = default;
        ~RenderDeviceForge();

        bool IsInitialized() const { return m_pRenderer != nullptr; }
        bool Initialize( Settings const& settings );
        void Shutdown();

        inline Renderer* GetRenderer() { return m_pRenderer; }

    private:

        Renderer* m_pRenderer = nullptr;
    };
}