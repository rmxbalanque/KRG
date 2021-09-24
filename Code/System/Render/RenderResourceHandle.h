#pragma once

#include "System/Core/Types/IntegralTypes.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    struct ResourceHandle
    {
        enum class Type : uint8
        {
            None,
            Buffer,
            Shader,

            ShaderInputBinding,
            BlendState,
            RasterizerState,
            SamplerState,
            Texture,
            RenderTarget,

            numTypes,
        };

    public:

        ResourceHandle() = default;

        inline ~ResourceHandle()
        {
            KRG_ASSERT( m_pData0 == nullptr && m_pData1 == nullptr && m_type == Type::None );
        }

        inline bool IsValid() const 
        {
            return m_pData0 != nullptr && m_type != Type::None; 
        }

        inline void Reset() 
        { 
            m_pData0 = m_pData1 = nullptr; 
            m_type = Type::None; 
        }

    public:

        void*           m_pData0 = nullptr;         // Ptr to the actual API type
        void*           m_pData1 = nullptr;         // Ptr to the actual API type
        Type            m_type = Type::None;
    };
}