#ifdef _WIN32
#pragma  once

#include "_Module/API.h"
#include "Tools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets { class RawMesh; }

    //-------------------------------------------------------------------------

    namespace Physics
    {
        struct KRG_RESOURCECOMPILERS_PHYSICS_API PhysicsMeshResourceDescriptor : public Resource::ResourceDescriptor
        {
            KRG_REGISTER_TYPE;

            EXPOSE DataPath         m_meshDataPath;

            // Optional value that specifies the specific sub-mesh to compile, if this is not set, all sub-meshes contained in the source will be combined into a single mesh object
            EXPOSE String           m_meshName;
        };

        //-------------------------------------------------------------------------

        class PhysicsMeshCompiler : public Resource::Compiler
        {
            static const S32 VERSION = 2;

        public:

            PhysicsMeshCompiler();
            virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;

        private:

            bool CookTriangleMeshData( RawAssets::RawMesh const& rawMesh, TVector<Byte>& outCookedData ) const;
        };
    }
}

#endif