#pragma once

#include "../_Module/API.h"
#include "CameraComponent.h"
#include "System/Entity/EntityGlobalSystem.h"
#include "System/Core/Math/Viewport.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_ENGINE_CORE_API CameraSystem : public IGlobalEntitySystem, public ISystem
    {
        struct RegisteredCamera
        {
            RegisteredCamera( UUID entityID, CameraComponent* pComponent );

        public:

            UUID                                m_entityID;
            CameraComponent*                    m_pComponent = nullptr;
        };

    public:

        KRG_SYSTEM_ID( CameraSystem );
        KRG_ENTITY_GLOBAL_SYSTEM( CameraSystem );

    public:

        inline bool HasActiveCameras() const;
        inline void SetViewDimensions( Int2 newNindowDimensions );
        inline TInlineVector<Math::Viewport, 2> const& GetActiveViewports() const { return m_activeViewports; }

        void Update();

    private:

        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

        void UpdateActiveViewports();

    private:

        TVector<RegisteredCamera>               m_cameras;
        TInlineVector<Math::Viewport,2>         m_activeViewports;
        Int2                                    m_viewDimensions = Int2::Zero;
    };
} 