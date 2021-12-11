#pragma once

#include "Plane.h"
#include "Line.h"
#include "NumericRange.h"
#include "BoundingVolumes.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Drawing { class DrawContext; }

    //-------------------------------------------------------------------------

    namespace Math
    {
        //-------------------------------------------------------------------------
        // A view volume looking along the Y axis (KRG Forward)
        //-------------------------------------------------------------------------
        // Using a horizontal field of view

        class KRG_SYSTEM_CORE_API ViewVolume
        {

        public:

            enum class PlaneID { Left = 0, Right, Top, Bottom, Near, Far };
            enum class ProjectionType { Orthographic, Perspective };
            enum class IntersectionResult { FullyOutside = 0, FullyInside, Intersects };

            inline static Radians ConvertVerticalToHorizontalFOV( float width, float height, Radians VerticalAngle )
            {
                KRG_ASSERT( !Math::IsNearZero( height ) );
                Radians const horizontalFOV( 2.0f * Math::ATan( width / height * Math::Tan( (float) VerticalAngle / 2.0f ) ) );
                return horizontalFOV;
            }

            inline static Radians ConvertHorizontalToVerticalFOV( float width, float height, Radians HorizontalAngle )
            {
                KRG_ASSERT( !Math::IsNearZero( width ) );
                Radians const verticalFOV( 2.0f * Math::ATan( height / width * Math::Tan( (float) HorizontalAngle / 2.0f ) ) );
                return verticalFOV;
            }

        public:

            ViewVolume() {} // Warning: Leaves most members uninitialized!
            ViewVolume( Float2 const& viewDimensions, FloatRange depthRange, Matrix const& worldMatrix = Matrix::Identity );
            ViewVolume( Float2 const& viewDimensions, FloatRange depthRange, Radians FOV, Matrix const& worldMatrix = Matrix::Identity );

            bool IsValid() const;

            void SetWorldMatrix( Matrix const& worldTransform );
            void SetView( Float3 const& position, Float3 const& viewDir, Float3 const& upDir );
            void SetDepthRange( FloatRange depthRange );
            void SetViewDimensions( Float2 dimensions );
            void SetHorizontalFOV( Radians FOV );

            // Projection Info
            //-------------------------------------------------------------------------

            inline bool IsPerspective() const { return m_type == ProjectionType::Perspective; }
            inline bool IsOrthographic() const { return m_type == ProjectionType::Orthographic; }

            inline FloatRange GetDepthRange() const { return m_depthRange; }
            inline Float2 GetViewDimensions() const { return m_viewDimensions; }
            inline float GetAspectRatio() const { return m_viewDimensions.m_x / m_viewDimensions.m_y; }
            inline Radians GetFOV() const { KRG_ASSERT( IsPerspective() ); return m_FOV; }

            // Spatial Info
            //-------------------------------------------------------------------------

            inline Vector GetViewPosition() const { return m_worldMatrix.GetTranslation(); }
            inline Quaternion GetViewOrientation() const { return m_worldMatrix.GetRotation(); }
            inline Vector GetForwardVector() const { return m_worldMatrix.GetForwardVector(); }
            inline Vector GetRightVector() const { return m_worldMatrix.GetRightVector(); }
            inline Vector GetUpVector() const { return m_worldMatrix.GetUpVector(); }

            inline Matrix const& GetWorldMatrix() const { return m_worldMatrix; }
            inline Matrix const& GetViewMatrix() const { return m_viewMatrix; }
            inline Matrix const& GetProjectionMatrix() const { return m_projectionMatrix; }
            inline Matrix const& GetViewProjectionMatrix() const { return m_viewProjectionMatrix; }
            inline Matrix const& GetInverseViewProjectionMatrix() const { return m_inverseViewProjectionMatrix; }

            // View Planes
            //-------------------------------------------------------------------------

            inline Plane const& GetViewPlane( PlaneID p ) const { return GetViewPlane( (uint32) p ); }
            inline Plane const& GetViewPlane( uint32 p ) const { KRG_ASSERT( p < 6 ); return m_viewPlanes[p]; }
            inline void GetCorners( Vector corners[8] ) const; // The first 4 points are the near plane corners, the last 4 are the far plane corners

            // Bounds and Intersection tests
            //-------------------------------------------------------------------------

            inline AABB GetAABB() const
            {
                Vector corners[8];
                GetCorners( corners );
                return AABB( corners, 8 );
            }

            OBB GetOBB() const;
            IntersectionResult Intersect( AABB const& aabb ) const;
            IntersectionResult Intersect( Vector const& point ) const;

            inline bool Contains( AABB const& aabb ) const { return Intersect( aabb ) != IntersectionResult::FullyOutside; }
            inline bool Contains( Vector const& point ) const { return Intersect( point ) != IntersectionResult::FullyOutside; }

            //-------------------------------------------------------------------------

            #if KRG_DEVELOPMENT_TOOLS
            void DrawDebug( Drawing::DrawContext& drawingContext ) const;
            #endif

        private:

            void CalculateProjectionMatrix();
            void UpdateInternals();

        private:

            Matrix                  m_worldMatrix;                          // The actual world transform of the volume
            Matrix                  m_viewMatrix;                           // The conversion from KRG space ( KRG Z-up space to rendering Y-up space )
            Matrix                  m_projectionMatrix;                     // The projection conversion for this volume // TODO: infinite perspective, inverse Z
            Matrix                  m_viewProjectionMatrix;                 // Cached view projection matrix
            Matrix                  m_inverseViewProjectionMatrix;          // Inverse of the cached view projection matrix
            Plane                   m_viewPlanes[6];                        // Cached view planes for this volume

            Float2                  m_viewDimensions = Float2::Zero;        // The dimensions of the view volume
            Radians                 m_FOV = Radians( 0.0f );                // The horizontal field of view angle (only for perspective projection)
            FloatRange              m_depthRange = FloatRange( 0 );         // The distance from the volume origin of the near/far planes ( X = near plane, Y = far plane )
            ProjectionType          m_type = ProjectionType::Perspective;   // The projection type
        };
    }
}