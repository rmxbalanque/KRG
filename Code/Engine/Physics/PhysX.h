#pragma once
#include "_Module/API.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Math/Plane.h"
#include "System/Core/Math/BoundingVolumes.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Logging/Log.h"

#include <PxPhysicsAPI.h>
#include <extensions/PxDefaultAllocator.h>
#include <extensions/PxDefaultErrorCallback.h>

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    struct KRG_ENGINE_PHYSICS_API Constants
    {
        static constexpr float const    s_lengthScale = 1.0f;
        static constexpr float const    s_speedScale = 9.81f;
        static Float3 const             s_gravity;
    };

    //-------------------------------------------------------------------------
    // Shared Resources
    //-------------------------------------------------------------------------

    struct KRG_ENGINE_PHYSICS_API SharedMeshes
    {
        static physx::PxConvexMesh*     s_pUnitCylinderMesh;
    };

    //-------------------------------------------------------------------------
    // Type Conversion
    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE Float2 FromPx( physx::PxVec2 const& v )
    {
        return Float2( v.x, v.y );
    }

    KRG_FORCE_INLINE Float3 FromPx( physx::PxVec3 const& v )
    {
        return Float3( v.x, v.y, v.z );
    }

    KRG_FORCE_INLINE Float4 FromPx( physx::PxVec4 const& v )
    {
        return Float4( v.x, v.y, v.z, v.w );
    }

    KRG_FORCE_INLINE Quaternion FromPx( physx::PxQuat const& v )
    {
        return Quaternion( v.x, v.y, v.z, v.w );
    }

    KRG_FORCE_INLINE Transform FromPx( physx::PxTransform const& v )
    {
        return Transform( FromPx( v.q ), FromPx( v.p ) );
    }

    KRG_FORCE_INLINE Plane FromPx( physx::PxPlane const& v )
    {
        return Plane( v.d, FromPx( v.n ) );
    }

    KRG_FORCE_INLINE Matrix FromPx( physx::PxMat33 const& v )
    {
        Matrix m( Matrix::Identity );

        m[0][0] = v.column0[0];
        m[1][0] = v.column0[1];
        m[2][0] = v.column0[2];

        m[0][1] = v.column1[0];
        m[1][1] = v.column1[1];
        m[2][1] = v.column1[2];

        m[0][2] = v.column2[0];
        m[1][2] = v.column2[1];
        m[2][2] = v.column2[2];

        return m;
    }

    KRG_FORCE_INLINE Matrix FromPx( physx::PxMat44 const& v )
    {
        return (Matrix&) v;
    }

    KRG_FORCE_INLINE AABB FromPx( physx::PxBounds3 const& bounds )
    {
        return AABB::FromMinMax( FromPx( bounds.minimum ), FromPx( bounds.maximum ) );
    }

    KRG_FORCE_INLINE Color FromPxColor( physx::PxU32 color )
    {
        return Color( color | 0x000000FF );
    }

    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE physx::PxVec2 ToPx( Float2 const& v )
    {
        return physx::PxVec2( v.x, v.y );
    }

    KRG_FORCE_INLINE physx::PxVec3 ToPx( Float3 const& v )
    {
        return physx::PxVec3( v.x, v.y, v.z );
    }

    KRG_FORCE_INLINE physx::PxVec3 ToPx( Vector const& v )
    {
        return physx::PxVec3( v.x, v.y, v.z );
    }

    KRG_FORCE_INLINE physx::PxVec4 ToPx( Float4 const& v )
    {
        return physx::PxVec4( v.x, v.y, v.z, v.w );
    }

    KRG_FORCE_INLINE physx::PxQuat ToPx( Quaternion const& v )
    {
        return physx::PxQuat( v.x, v.y, v.z, v.w );
    }

    KRG_FORCE_INLINE physx::PxTransform ToPx( Transform const& v )
    {
        KRG_ASSERT( v.IsRigidTransform() );
        return physx::PxTransform( ToPx( v.GetTranslation().ToFloat3() ), ToPx( v.GetRotation() ) );
    }

    KRG_FORCE_INLINE physx::PxPlane ToPx( Plane const& v )
    {
        KRG_ASSERT( v.IsNormalized() );
        return physx::PxPlane( v.a, v.b, v.c, v.d );
    }

    KRG_FORCE_INLINE physx::PxMat33 ToPxMat33( Matrix const& v )
    {
        physx::PxMat33 m( physx::PxIdentity );

        m.column0[0] = v[0][0];
        m.column0[1] = v[1][0];
        m.column0[2] = v[2][0];

        m.column1[0] = v[0][1];
        m.column1[1] = v[1][1];
        m.column1[2] = v[2][1];

        m.column2[0] = v[0][2];
        m.column2[1] = v[1][2];
        m.column2[2] = v[2][2];

        return m;
    }

    KRG_FORCE_INLINE physx::PxMat44 ToPxMat44( Matrix const& v )
    {
        return ( physx::PxMat44& ) v;
    }

    KRG_FORCE_INLINE physx::PxBounds3 ToPx( AABB const& bounds )
    {
        return physx::PxBounds3( ToPx( bounds.GetMin() ), ToPx( bounds.GetMax() ) );
    }

    //-------------------------------------------------------------------------
    // Memory
    //-------------------------------------------------------------------------

    class PhysXAllocator final : public physx::PxAllocatorCallback
    {
        virtual void* allocate( size_t size, const char* typeName, const char* filename, int line ) override
        {
            return KRG::Alloc( size, 16 );
        }

        virtual void deallocate( void* ptr ) override
        {
            KRG::Free( ptr );
        }
    };

    //-------------------------------------------------------------------------
    // Error Reporting
    //-------------------------------------------------------------------------

    class PhysXUserErrorCallback final : public physx::PxErrorCallback
    {
        virtual void reportError( physx::PxErrorCode::Enum code, const char* message, const char* file, int line ) override
        {
            Log::AddEntry( Log::Severity::Error, "Physics", file, line, message );
        }
    };

    //-------------------------------------------------------------------------
    // Task System
    //-------------------------------------------------------------------------

    class PhysXTaskDispatcher final : public physx::PxCpuDispatcher
    {
        virtual void submitTask( physx::PxBaseTask& task ) override
        {
            // Surprisingly it is faster to run all physics tasks on a single thread since there is a fair amount of gaps when spreading the tasks across multiple cores.
            // TODO: re-evaluate this when we have additional work. Perhaps we can interleave other tasks while physics tasks are waiting
            auto pTask = &task;
            pTask->run();
            pTask->release();
        }

        virtual physx::PxU32 getWorkerCount() const override
        {
            return 1;
        }
    };
}