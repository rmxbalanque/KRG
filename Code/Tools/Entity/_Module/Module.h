#pragma once

#include "API.h"
#include "Tools/Entity/ResourceCompilers/ResourceCompiler_EntityCollection.h"
#include "Tools/Core/Resource/Compilers/ResourceCompilerRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class KRG_TOOLS_ENTITY_API ToolsModule
    {
        KRG_REGISTER_MODULE;

    public:

        void RegisterCompilers( Resource::CompilerRegistry& compilerRegistry );
        void UnregisterCompilers( Resource::CompilerRegistry& compilerRegistry );

    private:

        EntityCollectionCompiler    m_collectionCompiler;
    };
}
