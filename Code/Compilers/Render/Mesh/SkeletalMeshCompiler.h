namespace KRG {  }
#ifdef _WIN32
#pragma once

#include "MeshCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets { class RawMesh; }

    //-------------------------------------------------------------------------

    namespace Render
    {
        class MeshGeometry;
        class SkeletalMesh;

        //-------------------------------------------------------------------------

        struct SkeletalMeshResourceDescriptor : public Resource::ResourceDescriptor
        {
            KRG_REGISTER_TYPE;

            // The path to the mesh source file
            EXPOSE DataPath         m_meshDataPath;

            // Optional value that specifies the specific sub-mesh to compile, if this is not set, all sub-meshes contained in the source will be combined into a single mesh object
            EXPOSE String           m_meshName;
        };

        //-------------------------------------------------------------------------

        class SkeletalMeshCompiler : public MeshCompiler
        {
            static const int32 VERSION = 2;

        public:

            SkeletalMeshCompiler();
            virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;

        private:

            void TransferSkeletalMeshData( RawAssets::RawMesh const& rawMesh, SkeletalMesh& mesh ) const;
        };
    }
}

#endif