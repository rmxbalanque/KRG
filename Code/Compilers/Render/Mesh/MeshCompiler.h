#ifdef _WIN32
#pragma once

#include "Tools/Resource/Compilers/ResourceCompiler.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace RawAssets { class RawMesh; }

    //-------------------------------------------------------------------------

    namespace Render
    {
        class MeshGeometry;

        //-------------------------------------------------------------------------

        class MeshCompiler : public Resource::Compiler
        {
        protected:

            using Resource::Compiler::Compiler;

        protected:

            void TransferMeshGeometry( RawAssets::RawMesh const& rawMesh, MeshGeometry& mesh ) const;
            void OptimizeMeshGeometry( MeshGeometry& mesh ) const;
        };
    }
}

#endif