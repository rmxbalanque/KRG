#pragma once

#include "MapEditor/MapEditor.h"
#include "Tools/UI/Application/Application.h"
#include "Tools/UI/Application/Window.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Core/IntegralTypes.h"
#include <DockManager.h>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Editor
    {
        class EditorApplication : public Tools::Application
        {

        public:

            EditorApplication( int argc, char* argv[] );

        protected:

            virtual bool Initialize() override final;
            virtual void Shutdown() override final;


        private:

            ads::CDockManager*          m_pMainDockManager = nullptr;
            QMenuBar*                   m_pMainMenu = nullptr;

            TypeSystem::TypeRegistry    m_typeRegistry;
            MapEditor                   m_mapEditor;
        };
    }
}