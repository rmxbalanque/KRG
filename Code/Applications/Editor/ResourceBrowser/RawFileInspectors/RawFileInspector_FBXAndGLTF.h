#pragma once
#include "RawFileInspector.h"
#include "Tools/Core/Resource/RawAssets/RawAssetInfo.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class FileInspectorFBXAndGLTF : public RawFileInspector
    {
        constexpr static char const* s_title = "FBX/GLTF Inspector";

    public:

        FileInspectorFBXAndGLTF( EditorModel* pModel, FileSystem::Path const& filePath );

    private:

        virtual bool DrawDialog() override;

        void DrawStaticMeshControls( RawAssets::RawMeshInfo const& mesh );
        void DrawSkeletalMeshControls( RawAssets::RawMeshInfo const& mesh );
        void DrawSkeletonControls( RawAssets::RawSkeletonInfo const& skeleton );
        void DrawAnimationControls( RawAssets::RawAnimationInfo const& animation );

    private:

        RawAssets::RawAssetInfo                     m_assetInfo;
        bool                                        m_validAssetInfo = false;
    };
}