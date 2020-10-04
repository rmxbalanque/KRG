#if _WIN32
#include "Module.h"

#include "Compilers/Render/Shaders/ShaderCompiler.h"
#include "Compilers/Render/Mesh/StaticMeshCompiler.h"
#include "Compilers/Render/Mesh/SkeletalMeshCompiler.h"
#include "Compilers/Render/Textures/TextureCompiler.h"
#include "Compilers/Render/Materials/MaterialCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        ResourceCompilerModule::ResourceCompilerModule()
            : KRG::ResourceCompilerModule( "Resource.Render" )
        {
            AddCompiler( KRG::New<KRG::Render::ShaderCompiler>() );
            AddCompiler( KRG::New<KRG::Render::StaticMeshCompiler>() );
            AddCompiler( KRG::New<KRG::Render::SkeletalMeshCompiler>() );
            AddCompiler( KRG::New<KRG::Render::TextureCompiler>() );
            AddCompiler( KRG::New<KRG::Render::MaterialCompiler>() );
        }
    }
}

#endif