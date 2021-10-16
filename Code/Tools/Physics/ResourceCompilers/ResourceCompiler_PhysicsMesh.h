#pragma  once

#include "Tools/Physics/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets { class RawMesh; }

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsMeshCompiler : public Resource::Compiler
    {
        static const int32 s_version = 4;

    public:

        PhysicsMeshCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;

    private:

        bool CookTriangleMeshData( RawAssets::RawMesh const& rawMesh, TVector<Byte>& outCookedData ) const;
        bool CookConvexMeshData( RawAssets::RawMesh const& rawMesh, TVector<Byte>& outCookedData ) const;
    };
}