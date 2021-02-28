#pragma once

#include "_Module/API.h"
#include "System/Core/Core/IntegralTypes.h"
#include "System/Core/Algorithm/Hash.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Math { class Viewport; }

    //-------------------------------------------------------------------------

    namespace Render
    {

        //-------------------------------------------------------------------------

        // This is a basic set of priority values to help order any registered renderers
        enum RendererPriorityLevel
        {
            Game = 0, 
            GameUI = 100,
            Debug = 200,
            DebugUI = 300
        };

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_RENDER_API IRenderer
        {

        public:

            virtual void Render( Math::Viewport const& viewport ) = 0;
            virtual U32 GetRendererID() const = 0;
            virtual S32 GetPriority() const = 0;
        };
    }
}

//-------------------------------------------------------------------------

#define KRG_RENDERER_ID( typeName, priority ) \
constexpr static U32 const RendererID = Hash::FNV1a::GetHash32( #typeName ); \
virtual U32 GetRendererID() const override final { return typeName::RendererID; } \
virtual S32 GetPriority() const override{ return priority; }