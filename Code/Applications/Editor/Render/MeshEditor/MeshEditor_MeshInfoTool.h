#pragma once
#include "MeshEditor_Model.h"
#include "Applications/Editor/EditorTool.h"
#include "Engine/Render/Components/StaticMeshComponent.h"
#include "Engine/Render/Components/SkeletalMeshComponent.h"
#include "System/DevTools/ThirdParty/imgui/imgui_internal.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
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
        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

        void UnloadAnyLoadedResources( UpdateContext const& context );

        inline bool IsPreviewingStaticMesh() const { return m_pStaticMesh.IsValid(); }
        inline bool IsPreviewingSkeletalMesh() const { return m_pSkeletalMesh.IsValid(); }
        inline bool IsLoadingMesh() const { return !m_pStaticMesh.IsLoaded() && !m_pSkeletalMesh.IsLoaded(); }

        void DrawMeshInfoWindow( UpdateContext const& context );
        void DrawSkeletalMeshInfo( UpdateContext const& context );

        void CreateSkeletonTree();
        void DestroySkeletonTree();
        ImRect RenderSkeletonTree( BoneInfo* pBone ) const;

        void UpdatePreviewEntity();

    private:

        DataPath                        m_currentlyPreviewedMesh;
        TResourcePtr<StaticMesh>        m_pStaticMesh = nullptr;
        TResourcePtr<SkeletalMesh>      m_pSkeletalMesh = nullptr;
        BoneInfo*                       m_pSkeletonTreeRoot = nullptr;
        bool                            m_isLoadingMesh = true;

        // Preview scene
        Entity*                         m_pPreviewEntity = nullptr;
        StaticMeshComponent*            m_pStaticMeshComponent = nullptr;
        SkeletalMeshComponent*          m_pSkeletalMeshComponent = nullptr;
    };
}