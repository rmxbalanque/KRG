#pragma once
#include "_Module/API.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Memory/Memory.h"
#include "System/Core/Debug/DebugDrawingSystem.h"

#include <bfxSystem.h>
#include <bfxPlannerSpace.h>

// HACK
#include <bfxMover.h>

//-------------------------------------------------------------------------

namespace KRG::Debug { class DrawingSystem; }

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    //-------------------------------------------------------------------------
    // Type Conversion
    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE Float3 FromBfx( bfx::Vector3 const& v )
    {
        return Float3( v.m_x, v.m_y, v.m_z );
    }

    KRG_FORCE_INLINE Quaternion FromBfx( bfx::Quaternion const& v )
    {
        return Quaternion( v.m_x, v.m_y, v.m_z, v.m_w );
    }

    KRG_FORCE_INLINE Color FromBfx( bfx::Color const& color )
    {
        return Color( uint8( color.m_r * 255 ), uint8( color.m_g * 255 ), uint8( color.m_b ), uint8( color.m_a * 255 ) );
    }

    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE bfx::Vector3 ToBfx( Float3 const& v )
    {
        return bfx::Vector3( v.m_x, v.m_y, v.m_z );
    }

    KRG_FORCE_INLINE bfx::Vector3 ToBfx( Float4 const& v )
    {
        return bfx::Vector3( v.m_x, v.m_y, v.m_z );
    }

    KRG_FORCE_INLINE bfx::Vector3 ToBfx( Vector const& v )
    {
        return bfx::Vector3( v.m_x, v.m_y, v.m_z );
    }

    KRG_FORCE_INLINE bfx::Quaternion ToBfx( Quaternion const& q )
    {
        bfx::Quaternion quaternion;
        quaternion.m_x = q.m_x;
        quaternion.m_y = q.m_y;
        quaternion.m_z = q.m_z;
        quaternion.m_w = q.m_w;
        return quaternion;
    }

    KRG_FORCE_INLINE bfx::Color ToBfx( Color const& color )
    {
        return bfx::Color( color.m_byteColor.m_r / 255.0f, color.m_byteColor.m_g / 255.0f, color.m_byteColor.m_b / 255.0f, color.m_byteColor.m_a / 255.0f );
    }

    //-------------------------------------------------------------------------
    // Integration
    //-------------------------------------------------------------------------

    class NavPowerAllocator final : public bfx::CustomAllocator
    {
        virtual void* CustomMalloc( size_t size ) override final { return KRG::Alloc( size ); }
        virtual void* CustomAlignedMalloc( uint32 alignment, size_t size ) override final { return KRG::Alloc( size, alignment ); }
        virtual void CustomFree( void* ptr ) override final { KRG::Free( ptr ); }
        virtual bool IsThreadSafe() const override final { return true; }
        virtual const char* GetName() const override { return "KRUGER"; }
    };

    // HACK
    //-------------------------------------------------------------------------

    KRG_ENGINE_NAVMESH_API bfx::Mover* CreateMover( Vector const& pos, Quaternion const& rot, bfx::MoverTune* pTune );
    KRG_ENGINE_NAVMESH_API void DestroyMover( bfx::Mover* pMover );
}