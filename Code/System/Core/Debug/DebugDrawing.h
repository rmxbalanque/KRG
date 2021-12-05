#pragma once

#include "../_Module/API.h"
#include "DebugDrawingCommands.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Math/BoundingVolumes.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Debug
{
    //-------------------------------------------------------------------------
    // Debug Drawing Context
    //-------------------------------------------------------------------------

    // The primary API for debug drawing in KRG
    // Drawing contexts are bound to threads, so each worker thread will have it's own drawing context

    // We have helper functions to allow you to draw custom types in the same manner as the basic primitive: 
    // i.e. drawingContext.Draw( MyType const& type )
    // To support this, you need to implement one of the following functions in your type:
    // 1) 'void DrawDebug( Debug::DrawingContext& ctx ) const'
    // 2) 'void DrawDebug( Debug::DrawingContext& ctx, Transform const& worldTransform ) const'

    // Note: We use float4 for colors here since KRG::color will implicitly convert to a float4. We use float4s for color in the rendering code,
    // so this allows you to save on constantly paying the conversion cost when drawing lots of primitives

    class KRG_SYSTEM_CORE_API DrawingContext
    {
        friend class DrawingSystem;

        constexpr static float const DefaultLineThickness = 1.0f;

    public:

        inline DrawingContext( DrawingContext const& RHS ) : m_commandBuffer( RHS.m_commandBuffer ) {}

        //-------------------------------------------------------------------------

        template<typename T>
        inline void Draw( T const& type )
        {
            type.DrawDebug( *this );
        }

        template<typename T>
        inline void Draw( T const& type, Transform const& worldTransform )
        {
            type.DrawDebug( *this, worldTransform );
        }

        //-------------------------------------------------------------------------
        // Basic Primitives
        //-------------------------------------------------------------------------

        inline void DrawPoint( Float3 const& position, Float4 const& color, float thickness = 5.0f, DepthTestState depthTestState = DepthTestState::Off )
        {
            InternalDrawPoint( m_commandBuffer, position, color, thickness, depthTestState );
        }

        inline void DrawLine( Float3 const& startPosition, Float3 const& endPosition, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            InternalDrawLine( m_commandBuffer, startPosition, endPosition, color, lineThickness, depthTestState );
        }

        inline void DrawLine( Float3 const& startPosition, Float3 const& endPosition, Float4 const& startColor, Float4 const& endColor, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            InternalDrawLine( m_commandBuffer, startPosition, endPosition, startColor, endColor, lineThickness, depthTestState );
        }

        inline void DrawTriangle( Float3 const& v0, Float3 const& v1, Float3 const& v2, Float4 const& color, DepthTestState depthTestState = DepthTestState::Off )
        {
            InternalDrawTriangle( m_commandBuffer, v0, v1, v2, color, color, color, depthTestState );
        }

        inline void DrawTriangle( Float3 const& v0, Float3 const& v1, Float3 const& v2, Float4 const& color0, Float4 const& color1, Float4 const& color2, DepthTestState depthTestState = DepthTestState::Off )
        {
            InternalDrawTriangle( m_commandBuffer, v0, v1, v2, color0, color1, color2, depthTestState );
        }

        void DrawWireTriangle( Float3 const& v0, Float3 const& v1, Float3 const& v2, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off );

        inline void DrawText2D( Float2 const& screenPosition, char const* pText, Float4 const& color, TextSize size = TextSize::Regular, TextAlignment alignment = TextAlignment::MiddleLeft, DepthTestState depthTestState = DepthTestState::Off )
        {
            InternalDrawText2D( m_commandBuffer, screenPosition, pText, color, size, alignment, TextBackground::Transparent, depthTestState );
        }

        inline void DrawText3D( Float3 const& worldPosition, char const* pText, Float4 const& color, TextSize size = TextSize::Regular, TextAlignment alignment = TextAlignment::MiddleLeft, DepthTestState depthTestState = DepthTestState::Off )
        {
            InternalDrawText3D( m_commandBuffer, worldPosition, pText, color, size, alignment, TextBackground::Transparent, depthTestState );
        }

        inline void DrawTextBox2D( Float2 const& screenPos, char const* pText, Float4 const& color, TextSize size = TextSize::Regular, TextAlignment alignment = TextAlignment::MiddleLeft, DepthTestState depthTestState = DepthTestState::Off )
        {
            InternalDrawText2D( m_commandBuffer, screenPos, pText, color, size, alignment, TextBackground::Opaque, depthTestState );
        }

        inline void DrawTextBox3D( Float3 const& worldPos, char const* pText, Float4 const& color, TextSize size = TextSize::Regular, TextAlignment alignment = TextAlignment::MiddleLeft, DepthTestState depthTestState = DepthTestState::Off )
        {
            InternalDrawText2D( m_commandBuffer, worldPos, pText, color, size, alignment, TextBackground::Opaque, depthTestState );
        }

        //-------------------------------------------------------------------------
        // Boxes / Volumes / Planes
        //-------------------------------------------------------------------------

        void DrawPlane( Float4 const& planeEquation, Float4 const& color, DepthTestState depthTestState = DepthTestState::Off );

        void DrawBox( Transform const& transform, Float4 const& color, DepthTestState depthTestState = DepthTestState::Off );
        void DrawWireBox( Transform const& transform, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off );

        inline void DrawBox( Transform const& worldTransform, Float3 const& halfsize, Float4 const& color, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto const transform = Transform::FromScale( halfsize ) * Transform( worldTransform );
            DrawBox( transform, color, depthTestState );
        }

        inline void DrawBox( Float3 const& position, Transform const& rotation, Float3 const& halfsize, Float4 const& color, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto transform = Transform::FromScale( halfsize ) * Transform( rotation );
            transform.SetTranslation( Vector( position, 1.0f ) );
            DrawBox( transform, color, depthTestState );
        }

        inline void DrawBox( Float3 const& position, Quaternion const& rotation, Float3 const& halfsize, Float4 const& color, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto transform = Transform::FromScale( halfsize ) * Transform( Quaternion( rotation ) );
            transform.SetTranslation( Vector( position, 1.0f ) );
            DrawBox( transform, color, depthTestState );
        }

        inline void DrawBox( OBB const& box, Float4 const& color, DepthTestState depthTestState = DepthTestState::Off )
        {
            Transform const transform( box.m_orientation, box.m_center, box.m_extents );
            DrawBox( transform, color, depthTestState );
        }

        inline void DrawBox( AABB const& box, Float4 const& color, DepthTestState depthTestState = DepthTestState::Off )
        {
            Transform const transform = Transform::FromTranslationAndScale( box.m_center, box.m_extents );
            DrawBox( transform, color, depthTestState );
        }

        inline void DrawWireBox( Transform const& worldTransform, Float3 const& halfsize, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto const transform = Transform::FromScale( halfsize ) * Transform( worldTransform );
            DrawWireBox( transform, color, lineThickness, depthTestState );
        }

        inline void DrawWireBox( Float3 const& position, Transform const& rotation, Float3 const& halfsize, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto transform = Transform::FromScale( halfsize ) * Transform( rotation );
            transform.SetTranslation( Vector( position, 1.0f ) );
            DrawWireBox( transform, color, lineThickness, depthTestState );
        }

        inline void DrawWireBox( Float3 const& position, Quaternion const& rotation, Float3 const& halfsize, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto transform = Transform::FromScale( halfsize ) * Transform( Quaternion( rotation ) );
            transform.SetTranslation( Vector( position, 1.0f ) );
            DrawWireBox( transform, color, lineThickness, depthTestState );
        }

        inline void DrawWireBox( OBB const& box, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            Transform const transform( box.m_orientation, box.m_center, box.m_extents );
            DrawWireBox( transform, color, lineThickness, depthTestState );
        }

        inline void DrawWireBox( AABB const& box, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            Transform const transform = Transform::FromTranslationAndScale( box.m_center, box.m_extents );
            DrawWireBox( transform, color, lineThickness, depthTestState );
        }

        //-------------------------------------------------------------------------
        // Sphere / Circle / Cylinder / Capsule
        //-------------------------------------------------------------------------

        void DrawCircle( Transform const& transform, Axis upAxis, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off );

        inline void DrawCircle( Float3 const& origin, Axis upAxis, float const& radii, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto const transform = Transform::FromScale( Vector( radii ) ) * Transform( Quaternion::Identity, origin );
            DrawCircle( transform, upAxis, color, lineThickness, depthTestState );
        }

        inline void DrawCircle( Transform const& worldTransform, Axis upAxis, float const& radii, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto const transform = Transform::FromScale( Vector( radii ) ) * Transform( worldTransform );
            DrawCircle( transform, upAxis, color, lineThickness, depthTestState );
        }

        inline void DrawCircle( Transform const& worldTransform, Axis upAxis, Float3 const& radii, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto const transform = Transform::FromScale( Vector( radii ) ) * Transform( worldTransform );
            DrawCircle( transform, upAxis, color, lineThickness, depthTestState );
        }

        //-------------------------------------------------------------------------

        void DrawSphere( Transform const& transform, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off );

        inline void DrawSphere( Transform const& worldTransform, Float3 const& radii, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto const transform = Transform::FromScale( radii ) * Transform( worldTransform );
            DrawSphere( transform, color, lineThickness, depthTestState );
        }

        inline void DrawSphere( Float3 const& position, Float3 const& radii, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto transform = Transform::FromScale( radii );
            transform.SetTranslation( Vector( position, 1.0f ) );
            DrawSphere( transform, color, lineThickness, depthTestState );
        }

        inline void DrawSphere( Sphere const& sphere, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto transform = Transform::FromScale( Vector( sphere.GetRadius() ) );
            transform.SetTranslation( sphere.GetCenter() );
            DrawSphere( transform, color, lineThickness, depthTestState );
        }

        //-------------------------------------------------------------------------

        // A half sphere from the transform point, with the radius along the +Z axis
        void DrawHalfSphere( Transform const& transform, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off );

        inline void DrawHalfSphere( Transform const& worldTransform, Float3 const& radii, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto const transform = Transform::FromScale( radii ) * Transform( worldTransform );
            DrawHalfSphere( transform, color, lineThickness, depthTestState );
        }

        inline void DrawHalfSphere( Float3 const& position, Float3 const& radii, Float4 const& color, float lineThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            auto transform = Transform::FromScale( radii );
            transform.SetTranslation( Vector( position, 1.0f ) );
            DrawHalfSphere( transform, color, lineThickness, depthTestState );
        }

        //-------------------------------------------------------------------------

        void DrawDisc( Float3 const& worldPoint, float radius, Float4 const& color, DepthTestState depthTestState = DepthTestState::Off );

        // Cylinder with radius on the XY plane and half-height along Z
        void DrawCylinder( Transform const& worldTransform, float radius, float halfHeight, Float4 const& color, float thickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off );

        // Capsule with radius on the XY plane and half-height along Z, total capsule height = 2 * ( halfHeight + radius )
        void DrawCapsule( Transform const& worldTransform, float radius, float halfHeight, Float4 const& color, float thickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off );

        //-------------------------------------------------------------------------
        // Complex Shapes
        //-------------------------------------------------------------------------

        inline void DrawAxis( Transform const& worldTransform, float axisLength = 0.05f, float axisThickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            Vector const xAxis = worldTransform.GetAxisX().GetNormalized3() * axisLength;
            Vector const yAxis = worldTransform.GetAxisY().GetNormalized3() * axisLength;
            Vector const zAxis = worldTransform.GetAxisZ().GetNormalized3() * axisLength;

            DrawLine( worldTransform.GetTranslation(), worldTransform.GetTranslation() + xAxis, Colors::Red, axisThickness, depthTestState );
            DrawLine( worldTransform.GetTranslation(), worldTransform.GetTranslation() + yAxis, Colors::LimeGreen, axisThickness, depthTestState );
            DrawLine( worldTransform.GetTranslation(), worldTransform.GetTranslation() + zAxis, Colors::Blue, axisThickness, depthTestState );
        }

        void DrawArrow( Float3 const& startPoint, Float3 const& endPoint, Float4 const& color, float thickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off );

        inline void DrawArrow( Float3 const& startPoint, Float3 const& direction, float arrowLength, Float4 const& color, float thickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            Vector const endPoint = Vector::MultiplyAdd( Vector( direction ).GetNormalized3(), Vector( arrowLength ), Vector( startPoint ) );
            DrawArrow( startPoint, endPoint.ToFloat3(), color, thickness, depthTestState );
        }

        // Draw a cone, originating at the transform, aligned to the -Y axis of the transform
        void DrawCone( Transform const& transform, Radians coneAngle, float length, Float4 const& color, float thickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off );

        inline void DrawCone( Float3 const& startPoint, Float3 const& direction, Radians coneAngle, float length, Float4 const& color, float thickness = DefaultLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            Transform const coneTransform( Quaternion::FromOrientationVector( direction ), startPoint );
            DrawCone( coneTransform, coneAngle, length, color, thickness, depthTestState );
        }

    private:

        KRG_FORCE_INLINE void InternalDrawPoint( Drawing::ThreadCommandBuffer& cmdList, Float3 const& position, Float4 const& color, float thickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            cmdList.AddCommand( Drawing::PointCommand( position, color, thickness ), depthTestState );
        }

        KRG_FORCE_INLINE void InternalDrawLine( Drawing::ThreadCommandBuffer& cmdList, Float3 const& startPosition, Float3 const& endPosition, Float4 const& color, float lineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            cmdList.AddCommand( Drawing::LineCommand( startPosition, endPosition, color, lineThickness ), depthTestState );
        }

        KRG_FORCE_INLINE void InternalDrawLine( Drawing::ThreadCommandBuffer& cmdList, Float3 const& startPosition, Float3 const& endPosition, Float4 const& startColor, Float4 const& endColor, float lineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            cmdList.AddCommand( Drawing::LineCommand( startPosition, endPosition, startColor, endColor, lineThickness ), depthTestState );
        }

        KRG_FORCE_INLINE void InternalDrawLine( Drawing::ThreadCommandBuffer& cmdList, Float3 const& startPosition, Float3 const& endPosition, Float4 const& color, float startLineThickness, float endLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            cmdList.AddCommand( Drawing::LineCommand( startPosition, endPosition, color, startLineThickness, endLineThickness ), depthTestState );
        }

        KRG_FORCE_INLINE void InternalDrawLine( Drawing::ThreadCommandBuffer& cmdList, Float3 const& startPosition, Float3 const& endPosition, Float4 const& startColor, Float4 const& endColor, float startLineThickness, float endLineThickness, DepthTestState depthTestState = DepthTestState::Off )
        {
            cmdList.AddCommand( Drawing::LineCommand( startPosition, endPosition, startColor, endColor, startLineThickness, endLineThickness ), depthTestState );
        }

        KRG_FORCE_INLINE void InternalDrawTriangle( Drawing::ThreadCommandBuffer& cmdList, Float3 const& v0, Float3 const& v1, Float3 const& v2, Float4 const& color, DepthTestState depthTestState = DepthTestState::Off )
        {
            cmdList.AddCommand( Drawing::TriangleCommand( v0, v1, v2, color ), depthTestState );
        }

        KRG_FORCE_INLINE void InternalDrawTriangle( Drawing::ThreadCommandBuffer& cmdList, Float3 const& v0, Float3 const& v1, Float3 const& v2, Float4 const& color0, Float4 const& color1, Float4 const& color2, DepthTestState depthTestState = DepthTestState::Off )
        {
            cmdList.AddCommand( Drawing::TriangleCommand( v0, v1, v2, color0, color1, color2 ), depthTestState );
        }

        KRG_FORCE_INLINE void InternalDrawText2D( Drawing::ThreadCommandBuffer& cmdList, Float2 const& position, char const* pText, Float4 const& color, TextSize size, TextAlignment alignment, TextBackground background, DepthTestState depthTestState )
        {
            cmdList.AddCommand( Drawing::TextCommand( position, pText, color, size, alignment, background ), depthTestState );
        }

        KRG_FORCE_INLINE void InternalDrawText3D( Drawing::ThreadCommandBuffer& cmdList, Float3 const& position, char const* pText, Float4 const& color, TextSize size, TextAlignment alignment, TextBackground background, DepthTestState depthTestState )
        {
            cmdList.AddCommand( Drawing::TextCommand( position, pText, color, size, alignment, background ), depthTestState );
        }

        // Try to prevent users from copying these contexts around - TODO: we should record the thread ID and assert everywhere that we are on the correct thread
        inline DrawingContext( Drawing::ThreadCommandBuffer& buffer ) : m_commandBuffer( buffer ) {}
        inline DrawingContext operator=( DrawingContext const& RHS ) { return DrawingContext( RHS.m_commandBuffer ); }

    private:

        Drawing::ThreadCommandBuffer& m_commandBuffer;
    };
}
#endif