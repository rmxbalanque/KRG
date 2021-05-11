#pragma once

#include "API.h"
#include "Tools/Entity/ResourceCompilers/EntityMapCompiler.h"
#include "Tools/Resource/Compilers/ResourceCompilerRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_TOOLS_ENTITY_API ToolsModule
    {
        KRG_REGISTER_MODULE;

    public:

        void RegisterCompilers( Resource::CompilerRegistry& compilerRegistry )
        {
            compilerRegistry.RegisterCompiler( &m_mapCompiler );
        }

        void UnregisterCompilers( Resource::CompilerRegistry& compilerRegistry )
        {
            compilerRegistry.UnregisterCompiler( &m_mapCompiler );
        }

    private:

        EntityMapCompiler   m_mapCompiler;
    };
}
