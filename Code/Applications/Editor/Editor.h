#pragma once

#include "Applications/EngineShared/Engine.h"
#include "EditorTools.h"

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
        virtual void InitializeDevelopmentTools() override final;
        virtual void ShutdownDevelopmentTools() override final;
        #endif

    private:

        EditorTools    m_editorTools;
    };
}