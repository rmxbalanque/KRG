#pragma once

#include "_Module/API.h"
#include "System/Entity/EntityWorldSystem.h"
#include "System/Core/Math/Viewport.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG::Camera
{
    class CameraComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CAMERA_API CameraWorldSystem : public IWorldEntitySystem
    {
        struct RegisteredCamera
        {
            RegisteredCamera( UUID entityID, CameraComponent* pComponent );

        public:

            UUID                                m_entityID;
            CameraComponent*                    m_pComponent = nullptr;
        };

    public:

        KRG_ENTITY_WORLD_SYSTEM( CameraWorldSystem, RequiresUpdate( UpdateStage::PrePhysics ) );

    public:

        inline bool HasActiveCameras() const;

    private:

        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UpdateEntitySystem( UpdateContext const& ctx ) override;

    private:

        TVector<RegisteredCamera>               m_cameras;
        Int2                                    m_viewDimensions = Int2::Zero;
    protected:

    };
} 