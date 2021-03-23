#pragma once

#include "Applications/EngineShared/Engine.h"
#include "EditorUI.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Editor : public Engine
    {
        friend class EditorApplication;

    public:

        using Engine::Engine;

    private:

        #if KRG_DEVELOPMENT_TOOLS
        virtual void InitializeDevelopmentUI() override final;
        virtual void ShutdownDevelopmentUI() override final;
        #endif

    private:

        EditorUI    m_editorUI;
    };
}