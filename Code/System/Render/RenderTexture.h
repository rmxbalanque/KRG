#pragma once

#include "_Module/API.h"
#include "System/Render/RenderResourceHandle.h"
#include "System/Resource/IResource.h"
#include "System/Core/Serialization/Serialization.h"
#include "System/Core/Math/Math.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        enum class TextureFiltering : uint8
        {
            MinMagMipPoint = 0,
            MinMagPointMipLinear,
            MinPointMagLinearMipPoint,
            MinPointMagMipLinear,
            MinLinearMagMipPoint,
            MinLinearMagPointMipLinear,
            MinMagLinearMipPoint,
            MinMagMipLinear,
            Anisotropic,
            ComparisonMinMagMipPoint,
            ComparisonMinMagPointMipLinear,
            ComparisonMinPointMagLinearMipPoint,
            ComparisonMinPointMagMipLinear,
            ComparisonMinLinearMagMipPoint,
            ComparisonMinLinearMagPointMipLinear,
            ComparisonMinMagLinearMipPoint,
            ComparisonMinMagMipLinear,
            ComparisonAnisotropic,
            MinimumMinMagMipPoint,
            MinimumMinMagPointMipLinear,
            MinimumMinPointMagLinearMipPoint,
            MinimumMinPointMagMipLinear,
            MinimumMinLinearMagMipPoint,
            MinimumMinLinearMagPointMipLinear,
            MinimumMinMagLinearMipPoint,
            MinimumMinMagMipLinear,
            MinimumAnisotropic,
            MaximumMinMagMipPoint,
            MaximumMinMagPointMipLinear,
            MaximumMinPointMagLinearMipPoint,
            MaximumMinPointMagMipLinear,
            MaximumMinLinearMagMipPoint,
            MaximumMinLinearMagPointMipLinear,
            MaximumMinMagLinearMipPoint,
            MaximumMinMagMipLinear,
            MaximumAnisotropic,
        };

        enum class TextureAddressMode : uint8
        {
            Wrap = 0,
            Mirror,
            Clamp,
            Border,
        };

        enum class TextureFormat : uint8
        {
            Raw,
            DDS,
        };

        //-------------------------------------------------------------------------

        struct SamplerState
        {
            friend class RenderDevice;

            inline bool IsValid() const { return m_resourceHandle.IsValid(); }

            ResourceHandle const& GetResourceHandle() const { return m_resourceHandle; }

        public:

            TextureFiltering        m_filtering = TextureFiltering::MinMagMipLinear;
            TextureAddressMode      m_addressModeU = TextureAddressMode::Wrap;
            TextureAddressMode      m_addressModeV = TextureAddressMode::Wrap;
            TextureAddressMode      m_addressModeW = TextureAddressMode::Wrap;
            uint32                  m_maxAnisotropyValue = 1;
            float                   m_LODBias = 0;
            float                   m_minLOD = -FLT_MAX;
            float                   m_maxLOD = FLT_MAX;

        private:

            ResourceHandle          m_resourceHandle;
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_RENDER_API Texture : public Resource::IResource
        {
            friend class RenderDevice;
            friend class TextureCompiler;
            friend class TextureLoader;

            KRG_REGISTER_RESOURCE( 'TXTR' );
            KRG_SERIALIZE_MEMBERS( m_dimensions );

        public:

            Texture() = default;
            Texture( Int2 const& dimensions ) : m_dimensions( dimensions ) {}

            virtual bool IsValid() const override { return m_resourceHandle.IsValid(); }

            Int2 const& GetDimensions() const { return m_dimensions; }
            ResourceHandle const& GetResourceHandle() const { return m_resourceHandle; }

            inline bool operator==( Texture const& rhs ) const { return m_resourceHandle.m_pHandle == rhs.m_resourceHandle.m_pHandle; }
            inline bool operator!=( Texture const& rhs ) const { return m_resourceHandle.m_pHandle != rhs.m_resourceHandle.m_pHandle; }

        protected:

            ResourceHandle  m_resourceHandle;
            Int2            m_dimensions;
        };
    }
}