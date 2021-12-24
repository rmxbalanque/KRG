#pragma once

#include "Component_RenderMesh.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    enum class Mobility
    {
        KRG_REGISTER_ENUM

        Static = 0,
        Dynamic = 1
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_RENDER_API StaticMeshComponent final : public MeshComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( StaticMeshComponent );

    public:

        using MeshComponent::MeshComponent;

        // Mesh Data
        //-------------------------------------------------------------------------

        virtual bool HasMeshResourceSet() const override final { return m_pMesh.IsValid(); }

        inline void SetMesh( ResourceID meshResourceID )
        {
            KRG_ASSERT( IsUnloaded() );
            KRG_ASSERT( meshResourceID.IsValid() );
            m_pMesh = meshResourceID;
        }

        inline StaticMesh const* GetMesh() const
        {
            KRG_ASSERT( m_pMesh != nullptr && m_pMesh->IsValid() );
            return m_pMesh.GetPtr();
        }

        // Mobility
        //-------------------------------------------------------------------------

        inline TEventHandle<StaticMeshComponent*> OnMobilityChanged() { return m_mobilityChangedEvent; }
        inline TEventHandle<StaticMeshComponent*> OnStaticMobilityTransformUpdated() { return m_staticMobilityTransformUpdatedEvent; }
        inline Mobility GetMobility() const { return m_mobility; }
        void ChangeMobility( Mobility newMobility );

        virtual TVector<TResourcePtr<Material>> const& GetDefaultMaterials() const override;

    protected:

        virtual void Initialize() override final;
        virtual void OnWorldTransformUpdated() override final;

    private:

        KRG_EXPOSE TResourcePtr<StaticMesh>               m_pMesh;
        KRG_EXPOSE Mobility                               m_mobility = Mobility::Static;
        TEvent<StaticMeshComponent*>                      m_mobilityChangedEvent; // Fired whenever we switch mobility 
        TEvent<StaticMeshComponent*>                      m_staticMobilityTransformUpdatedEvent; // Fired whenever a "static" component is moved
    };
}