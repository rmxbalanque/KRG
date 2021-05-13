#pragma once

#include "Tools/Render/_Module/API.h"
#include "Tools/Resource/Compilers/ResourceCompiler.h"
#include "Engine/Render/Material/RenderMaterial.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets { class RawMesh; }

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class Mesh;

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_RENDER_API MeshResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( MeshResourceDescriptor );

        // The path to the mesh source file
        EXPOSE DataPath                             m_meshDataPath;

        // Default materials - TODO: extract from FBX
        EXPOSE TVector<TResourcePtr<Material>>      m_materials;

        // Optional value that specifies the specific sub-mesh to compile, if this is not set, all sub-meshes contained in the source will be combined into a single mesh object
        EXPOSE String                               m_meshName;
    };

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
}