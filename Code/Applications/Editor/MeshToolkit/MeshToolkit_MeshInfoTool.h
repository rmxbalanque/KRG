#pragma once
#include "MeshToolkit_Model.h"
#include "Applications/Editor/EditorTool.h"
#include "Engine/Render/Components/StaticMeshComponent.h"
#include "Engine/Render/Components/SkeletalMeshComponent.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
{
    struct BoneInfo;

    //-------------------------------------------------------------------------

    class MeshInfo : public TEditorTool<Model>
    {

    public:

        MeshInfo( EditorModel* pModel ) : TEditorTool<Model>( pModel ) {}
        ~MeshInfo();

    private:

        virtual char const * const GetName() { return "Mesh Info"; }
        virtual void Shutdown( UpdateContext const& context ) override;
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

        void DrawMeshInfo( UpdateContext const& context );
        void DrawSkeletalMeshInfo( UpdateContext const& context );

        void CreateSkeletonTree();
        void DestroySkeletonTree();
        ImRect RenderSkeletonTree( BoneInfo* pBone ) const;

    private:

        ResourceID                      m_skeletonTreeResource;
        BoneInfo*                       m_pSkeletonTreeRoot = nullptr;
    };
}