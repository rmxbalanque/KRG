#pragma once

#include "MeshComponent.h"
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

        inline TSingleUserEvent<void( StaticMeshComponent* )> OnMobilityChanged() { return m_mobilityChangedEvent; }
        inline Mobility GetMobility() const { return m_mobility; }

        inline void ChangeMobility( Mobility newMobility )
        {
            if ( newMobility != m_mobility )
            {
                m_mobility = newMobility;
                if ( m_mobilityChangedEvent.HasBoundUser() )
                {
                    m_mobilityChangedEvent.Execute( this );
                }
            }
        }

        virtual TVector<TResourcePtr<Material>> const& GetDefaultMaterials() const override;

    protected:

        virtual void Initialize() override final;
        virtual void OnWorldTransformUpdated() override final;

    private:

        EXPOSE TResourcePtr<StaticMesh>                                 m_pMesh;
        EXPOSE Mobility                                                 m_mobility = Mobility::Static;
        TSingleUserEventInternal<void( StaticMeshComponent* )>          m_mobilityChangedEvent;
    };
}