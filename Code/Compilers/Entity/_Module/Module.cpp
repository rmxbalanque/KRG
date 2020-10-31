#if _WIN32
#include "Module.h"
#include "Compilers/Entity/MapCompiler/EntityMapCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace EntityModel
    {
        ResourceCompilerModule::ResourceCompilerModule()
            : KRG::ResourceCompilerModule( "Resource.Entity" )
        {
            AddCompiler( KRG::New<EntityMapCompiler>() );
        }
    }
}

#endif