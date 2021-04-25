#pragma once

#include "Applications/Editor/EditorModel.h"
#include "Engine/Render/Components/StaticMeshComponent.h"
#include "Engine/Render/Components/SkeletalMeshComponent.h"
#include "System/Entity/Entity.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
{
    class Model final : public EditorModel
    {
    public:

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

        // Preview
        inline Entity* GetPreviewEntity() { return m_pPreviewEntity; }
        bool IsPreviewingStaticMesh() const;
        bool IsPreviewingSkeletalMesh() const;

    private:

        virtual void OnSetActiveResource( ResourceID const& resourceID ) override;
        virtual void OnClearActiveResource() override;

    private:

        Entity*                             m_pPreviewEntity = nullptr;
        StaticMeshComponent*                m_pStaticMeshComponent = nullptr;
        SkeletalMeshComponent*              m_pSkeletalMeshComponent = nullptr;
    };
}