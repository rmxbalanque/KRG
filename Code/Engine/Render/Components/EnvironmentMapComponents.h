#pragma once

#include "Engine/Render/_Module/API.h"
#include "Engine/Core/Entity/EntitySpatialComponent.h"
#include "System/Render/RenderTexture.h"

//-------------------------------------------------------------------------
namespace KRG::Render
{
    class KRG_ENGINE_RENDER_API LocalEnvironmentMapComponent : public SpatialEntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( LocalEnvironmentMapComponent );

    public:

        inline CubemapTexture const* GetEnvironmentMapTexture() const { return m_environmentMapTexture.GetPtr(); }

    private:

        KRG_EXPOSE TResourcePtr<CubemapTexture> m_environmentMapTexture;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_RENDER_API GlobalEnvironmentMapComponent : public EntityComponent
    {
        KRG_REGISTER_SINGLETON_ENTITY_COMPONENT( GlobalEnvironmentMapComponent );

    public:

        inline CubemapTexture const* GetSkyboxTexture() const { return m_skyboxTexture.GetPtr(); }
        inline CubemapTexture const* GetSkyboxRadianceTexture() const { return m_skyboxRadianceTexture.GetPtr(); }
        inline float  GetSkyboxIntensity() const { return m_skyboxIntensity; }

    private:

        KRG_EXPOSE TResourcePtr<CubemapTexture> m_skyboxTexture;
        KRG_EXPOSE TResourcePtr<CubemapTexture> m_skyboxRadianceTexture;
        KRG_EXPOSE float m_skyboxIntensity = 1.0;
    };
}