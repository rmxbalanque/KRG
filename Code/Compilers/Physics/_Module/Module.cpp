#if _WIN32
#include "Module.h"
#include "Compilers/Physics/CollisionMeshCompiler/PhysicsGeometryCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        ResourceCompilerModule::ResourceCompilerModule()
            : KRG::ResourceCompilerModule( "Resource.Physics" )
        {
            AddCompiler( KRG::New<KRG::Physics::PhysicsGeometryCompiler>() );
        }
    }
}

#endif