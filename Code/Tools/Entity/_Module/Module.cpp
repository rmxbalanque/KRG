#include "Module.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    void ToolsModule::RegisterCompilers( Resource::CompilerRegistry& compilerRegistry )
    {
        compilerRegistry.RegisterCompiler( &m_collectionCompiler );
    }

    void ToolsModule::UnregisterCompilers( Resource::CompilerRegistry& compilerRegistry )
    {
        compilerRegistry.UnregisterCompiler( &m_collectionCompiler );
    }
}
