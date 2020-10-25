#pragma once

#include "Engine/Render/_Module/API.h"
#include "System/Render/Mesh/StaticMesh.h"
#include "System/Render/RenderMaterial.h"
#include "System/Entity/EntitySpatialComponent.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        enum class Mobility
        {
            KRG_REGISTER_ENUM

            Static = 0,
            Dynamic = 1
        };

        //-------------------------------------------------------------------------

        class KRG_ENGINE_RENDER_API StaticMeshComponent : public SpatialEntityComponent
        {
            KRG_REGISTER_ENTITY_COMPONENT;

        public:

            // Mesh Data
            //-------------------------------------------------------------------------

            inline StaticMesh const* GetMesh() const
            {
                KRG_ASSERT( m_pMesh != nullptr && m_pMesh->IsValid() );
                return m_pMesh.GetPtr();
            }

            inline TVector<TResourcePtr<Material>> const& GetMaterials() const 
            { 
                KRG_ASSERT( m_pMesh != nullptr && m_pMesh->IsValid() );
                return m_materials; 
            }

            // Mobility
            //-------------------------------------------------------------------------

            inline TSingleUserEvent<void( StaticMeshComponent* )>& OnMobilityChanged() { return m_mobilityChangedEvent; }
            inline Mobility GetMobility() const { return m_mobility; }

            inline void ChangeMobility( Mobility newMobility ) 
            { 
                if( newMobility != m_mobility )
                {
                    m_mobility = newMobility;
                    if ( m_mobilityChangedEvent.HasBoundUser() )
                    {
                        m_mobilityChangedEvent.Execute( this );
                    }
                }
            }

        protected:

            virtual void Initialize() override final;
            virtual void OnWorldTransformUpdated() override final;

        private:

            EXPOSE TResourcePtr<StaticMesh>                                 m_pMesh;
            EXPOSE TVector<TResourcePtr<Material>>                          m_materials;
            EXPOSE Mobility                                                 m_mobility = Mobility::Static;
            TSingleUserEventInternal<void(StaticMeshComponent*)>            m_mobilityChangedEvent;
        };
    }
}