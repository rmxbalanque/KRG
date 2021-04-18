#pragma once
#include "MeshEditor_Model.h"
#include "Applications/Editor/EditorTool.h"
#include "Engine/Render/Mesh/RenderMesh.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    class MeshInfo : public TEditorTool<Model>
    {
    public:

        MeshInfo( EditorModel* pModel ) : TEditorTool<Model>( pModel ) {}

    private:

        virtual char const * const GetName() { return "Mesh Info"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

        void UnloadAnyLoadedResources( UpdateContext const& context );

        inline bool IsPreviewingStaticMesh() const { return m_pStaticMesh.IsValid(); }
        inline bool IsPreviewingSkeletalMesh() const { return m_pSkeletalMesh.IsValid(); }
        inline bool IsLoadingMesh() const { return !m_pStaticMesh.IsLoaded() && !m_pSkeletalMesh.IsLoaded(); }

        void DrawMeshInfo( UpdateContext const& context );
        void DrawSkeletalMeshInfo( UpdateContext const& context );

    private:

        DataPath                        m_currentlyPreviewedMesh;
        TResourcePtr<StaticMesh>        m_pStaticMesh = nullptr;
        TResourcePtr<SkeletalMesh>      m_pSkeletalMesh = nullptr;
    };
}