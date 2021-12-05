#pragma once
#include "RawResourceInspector.h"
#include "Tools/Core/Resource/RawAssets/gltf/gltfSceneContext.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceInspectorGLTF : public RawResourceInspector
    {
    public:

        ResourceInspectorGLTF( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& rawResourceDirectoryPath, FileSystem::Path const& filePath );

    private:

        virtual char const* GetInspectorTitle() const override { return  "GLTF Inspector"; }
        virtual void DrawFileInfoAndContents() override;
        virtual void DrawResourceDescriptorCreator() override;

    private:

        gltf::gltfSceneContext        m_sceneContext;
    };
}