#pragma once

#include "Engine/Render/_Module/API.h"
#include "System/Core/Types/IntegralTypes.h"
#include "System/Core/Algorithm/Hash.h"
#include "System/Core/Time/Time.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

namespace KRG { class EntityWorld; }

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class Viewport;
    class RenderTarget;

    //-------------------------------------------------------------------------

    // This is a basic set of priority values to help order any registered renderers
    enum RendererPriorityLevel
    {
        Game = 0,
        GameUI = 100,
        Debug = 200,
        DevelopmentTools = 300
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_RENDER_API IRenderer
    {

    public:

        virtual uint32 GetRendererID() const = 0;
        virtual int32 GetPriority() const = 0;

        virtual void RenderWorld( Seconds const deltaTime, Viewport const& viewport, RenderTarget const& renderTarget, EntityWorld* pWorld ) {};
        virtual void RenderViewport( Seconds const deltaTime, Viewport const& viewport, RenderTarget const& renderTarget ) {};
    };
}

//-------------------------------------------------------------------------

#define KRG_RENDERER_ID( typeName, priority ) \
constexpr static uint32 const RendererID = Hash::FNV1a::GetHash32( #typeName ); \
virtual uint32 GetRendererID() const override final { return typeName::RendererID; } \
virtual int32 GetPriority() const override{ return priority; }