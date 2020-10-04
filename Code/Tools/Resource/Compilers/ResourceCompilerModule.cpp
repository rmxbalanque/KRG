#ifdef _WIN32
#include "ResourceCompilerModule.h"
#include "System/Core/Math/Math.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ResourceCompilerModule::ResourceCompilerModule( char const* pModuleName )
        : m_moduleName( pModuleName )
    {}

    ResourceCompilerModule::~ResourceCompilerModule()
    {
        for ( auto& pCompiler : m_registeredCompilers )
        {
            KRG::Delete<Resource::Compiler>( pCompiler );
        }
    }
}
#endif