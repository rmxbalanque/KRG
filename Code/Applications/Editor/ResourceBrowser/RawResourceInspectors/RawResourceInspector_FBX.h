#pragma once
#include "RawResourceInspector.h"
#include "Tools/Core/Resource/RawAssets/Fbx/FbxSceneContext.h"

#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceInspectorFBX : public RawResourceInspector
    {
        enum class InfoType
        {
            SkeletalMesh,
            StaticMesh,
            Skeleton,
            Animation
        };

        struct MeshInfo
        {
            StringID                        m_nameID;
            bool                            m_isSkinned = false;
        };

        struct BoneSkeletonRootInfo
        {
            StringID                        m_nameID;
        };

        struct NullSkeletonRootInfo
        {
            StringID                        m_nameID;
            TVector<BoneSkeletonRootInfo>   m_childSkeletons;
        };

        struct AnimInfo
        {
            StringID                        m_nameID;
            Seconds                         m_duration = 0.0f;
            float                           m_frameRate = 0.0f;
        };

    public:

        ResourceInspectorFBX( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDatabase, FileSystem::Path const& filePath );

    private:

        virtual char const* GetInspectorTitle() const override { return  "FBX Inspector"; }
        virtual void DrawFileInfoAndContents() override;
        virtual void DrawResourceDescriptorCreator() override;

        void ReadFileContents();
        void DrawMeshDescriptorCreator();
        void DrawSkeletonDescriptorCreator();
        void DrawAnimationDescriptorCreator();

        void OnSwitchSelectedItem();

    private:

        Fbx::FbxSceneContext                m_sceneContext;
        TVector<MeshInfo>                   m_meshes;
        TVector<NullSkeletonRootInfo>       m_nullSkeletonRoots;
        TVector<BoneSkeletonRootInfo>       m_skeletonRoots;
        TVector<AnimInfo>                   m_animations;

        InfoType                            m_selectedItemType;
        StringID                            m_selectedItemID;

    };
}