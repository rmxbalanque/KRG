#pragma once

#include "System/Core/Core/IntegralTypes.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        struct ResourceHandle
        {
            enum class Type : U8
            {
                None,
                Buffer,
                Shader,

                ShaderInputBinding,
                BlendState,
                RasterizerState,
                SamplerState,
                Texture,

                numTypes,
            };

        public:

            ResourceHandle() = default;

            ~ResourceHandle()
            {
                KRG_ASSERT( m_pHandle == nullptr && m_type == Type::None );
            }

            inline bool IsValid() const { return m_pHandle != nullptr && m_type != Type::None; }
            inline void Reset() { m_pHandle = nullptr; m_type = Type::None; }

        public:

            void*           m_pHandle = nullptr;
            Type            m_type = Type::None;
        };
    }
}