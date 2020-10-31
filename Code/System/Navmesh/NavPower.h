#pragma once
#include "_Module/API.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Memory/Memory.h"

#include <bfxSystem.h>

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
        return Color( (U8) color.m_r * 255, (U8) color.m_g * 255, (U8) color.m_b * 255, (U8) color.m_a * 255 );
    }

    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE bfx::Vector3 ToBfx( Float3 const& v )
    {
        return bfx::Vector3( v.x, v.y, v.z );
    }

    KRG_FORCE_INLINE bfx::Vector3 ToBfx( Float4 const& v )
    {
        return bfx::Vector3( v.x, v.y, v.z );
    }

    KRG_FORCE_INLINE bfx::Vector3 ToBfx( Vector const& v )
    {
        return bfx::Vector3( v.x, v.y, v.z );
    }

    KRG_FORCE_INLINE bfx::Quaternion ToBfx( Quaternion const& q )
    {
        bfx::Quaternion quaternion;
        quaternion.m_x = q.x;
        quaternion.m_y = q.y;
        quaternion.m_z = q.z;
        quaternion.m_w = q.w;
        return quaternion;
    }

    KRG_FORCE_INLINE bfx::Color ToBfx( Color const& color )
    {
        return bfx::Color( color.m_byteColor.m_r, color.m_byteColor.m_g, color.m_byteColor.m_b, color.m_byteColor.m_a );
    }

    //-------------------------------------------------------------------------
    // Integration
    //-------------------------------------------------------------------------

    class NavpowerAllocator final : public bfx::CustomAllocator
    {
        virtual void* CustomMalloc( U32 size ) override final { return KRG::Alloc( size ); }
        virtual void* CustomAlignedMalloc( U32 alignment, U32 size ) override final { return KRG::Alloc( size, alignment ); }
        virtual void CustomFree( void* ptr ) override final { KRG::Free( ptr ); }
        virtual bool IsThreadSafe() const override final { return true; }
        virtual const char* GetName() const override { return "KRUGER"; }
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_NAVMESH_API NavPower
    {
        static NavPower*                                   Instance;

    public:

        static void Initialize();
        static void Shutdown();

        static bfx::Instance* GetNavpowerInstance();

    private:

        bfx::Instance*                                      m_pNavpowerInstance = nullptr;
        NavpowerAllocator                                   m_allocator;
    };
}