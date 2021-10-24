#pragma once

#include "../_Module/API.h"
#include "Engine/Core/Entity/EntityWorldSystem.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class CameraComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API CameraWorldSystem : public IWorldEntitySystem
    {
        struct RegisteredCamera
        {
            RegisteredCamera( UUID entityID, CameraComponent* pComponent );

        public:

            UUID                                m_entityID;
            CameraComponent*                    m_pComponent = nullptr;
        };

    public:

        KRG_REGISTER_TYPE( CameraWorldSystem );
        KRG_ENTITY_WORLD_SYSTEM( CameraWorldSystem, RequiresUpdate( UpdateStage::PrePhysics ) );

    public:

        inline bool HasActiveCamera() const { return m_pActiveCamera != nullptr; }
        inline CameraComponent* GetActiveCamera() const { return m_pActiveCamera; }

    private:

        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UpdateSystem( EntityUpdateContext const& ctx ) override;

    private:

        TVector<RegisteredCamera>               m_cameras;
        CameraComponent*                        m_pActiveCamera = nullptr;
        Int2                                    m_viewDimensions = Int2::Zero;
    };
} 