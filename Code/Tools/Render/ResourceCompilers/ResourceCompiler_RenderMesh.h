#pragma once

#include "Tools/Render/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets { class RawMesh; }

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class Mesh;
    class SkeletalMesh;
    struct MeshResourceDescriptor;

    //-------------------------------------------------------------------------

    class MeshCompiler : public Resource::Compiler
    {
    protected:

        using Resource::Compiler::Compiler;

    protected:

        void TransferMeshGeometry( RawAssets::RawMesh const& rawMesh, Mesh& mesh ) const;
        void OptimizeMeshGeometry( Mesh& mesh ) const;
        void SetMeshDefaultMaterials( MeshResourceDescriptor const& descriptor, Mesh& mesh ) const;
        void SetMeshInstallDependencies( Mesh const& mesh, Resource::ResourceHeader& hdr ) const;
    };

    //-------------------------------------------------------------------------

    class StaticMeshCompiler : public MeshCompiler
    {
        static const int32 s_version = 6;

    public:

        StaticMeshCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };

    //-------------------------------------------------------------------------

    class SkeletalMeshCompiler : public MeshCompiler
    {
        static const int32 s_version = 6;

    public:

        SkeletalMeshCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;

    private:

        void TransferSkeletalMeshData( RawAssets::RawMesh const& rawMesh, SkeletalMesh& mesh ) const;
    };
}