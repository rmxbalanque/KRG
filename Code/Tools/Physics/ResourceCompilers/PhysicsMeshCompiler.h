#pragma  once

#include "Tools/Physics/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets { class RawMesh; }

    //-------------------------------------------------------------------------

    namespace Physics
    {
        struct KRG_TOOLS_PHYSICS_API PhysicsMeshResourceDescriptor : public Resource::ResourceDescriptor
        {
            KRG_REGISTER_TYPE( PhysicsMeshResourceDescriptor );

            EXPOSE DataPath         m_meshDataPath;

            // Optional value: Specifies a single sub-mesh to use to generated the physics collision, if this is not set, all sub-meshes contained in the source will be combined into a single mesh object
            EXPOSE String           m_meshName;

            // Optional value: Specifies if the mesh is a convex mesh (meshes are considered triangle meshes by default)
            EXPOSE bool             m_isConvexMesh = false;
        };

        //-------------------------------------------------------------------------

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
}