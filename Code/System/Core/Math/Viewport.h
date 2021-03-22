#pragma once

#include "../_Module/API.h"
#include "ViewVolume.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Math
    {
        //-------------------------------------------------------------------------
        // KRG viewport
        //-------------------------------------------------------------------------
        // Defines an area of the 2D window that we can render to as well as the 3D volume to be rendered within it
        // Provides space conversion functions between the three main spaces:
        //  * World Space - Engine 3D space, units = meters
        //  * Clip Space - The 3d world space projected to the unit cube for clipping, units = Normalized device coordinates (NDC)
        //  * Screen Space - The 2D screen rectangle we render in, units = pixels

        class KRG_SYSTEM_CORE_API Viewport
        {

        public:

            Viewport();
            Viewport( Int2 const& topLeft, Int2 const& size, Math::ViewVolume const& viewVolume = Math::ViewVolume() );

            inline bool IsValid() const { return ( m_size.m_x > 0 && m_size.m_y > 0 ) && ( m_topLeftPosition.m_x >= 0 && m_topLeftPosition.m_y >= 0 ); }

            void Resize( Int2 const& dimensions, Int2 const& topLeftPosition = Int2::Zero );

            // Get the viewport position and dimensions - All in screen space
            //-------------------------------------------------------------------------

            inline Float2 const& GetSize() const { return m_size; }
            inline Float2 const& GetTopLeftPosition() const { return m_topLeftPosition; }
            inline float GetAspectRatio() const { return m_size.m_x / m_size.m_y; }

            // View Volume
            //-------------------------------------------------------------------------

            void SetViewVolume( Math::ViewVolume const& viewVolume );
            inline Math::ViewVolume const& GetViewVolume() const { return m_viewVolume; }

            inline Vector const& GetViewOrigin() const { return m_viewVolume.GetWorldMatrix().GetTranslation(); }
            inline Vector GetViewForwardDirection() const { return m_viewVolume.GetForwardVector(); }
            inline Vector GetViewRightDirection() const { return m_viewVolume.GetRightVector(); }
            inline Vector GetViewUpDirection() const { return m_viewVolume.GetUpVector(); }

            // Space conversions
            //-------------------------------------------------------------------------

            Vector WorldSpaceToClipSpace( Vector const& pointWS ) const;
            inline Float2 WorldSpaceToScreenSpace( Vector const& pointWS ) const { return ClipSpaceToScreenSpace( WorldSpaceToClipSpace( pointWS ) ); }

            Float2 ClipSpaceToScreenSpace( Vector const& pointCS ) const;
            LineSegment ClipSpaceToWorldSpace( Vector const& pointCS ) const;

            Vector ScreenSpaceToClipSpace( Float2 const& pointSS ) const;
            inline LineSegment ScreenSpaceToWorldSpace( Float2 const& pointSS ) const { return ClipSpaceToWorldSpace( ScreenSpaceToClipSpace( pointSS ) ); }
            Vector ScreenSpaceToWorldSpaceNearPlane( Vector const& pointSS ) const;
            Vector ScreenSpaceToWorldSpaceFarPlane( Vector const& pointSS ) const;

        private:

            Math::ViewVolume                m_viewVolume;
            Float2                          m_topLeftPosition = Float2( 0.0f );
            Float2                          m_size = Float2( 0.0f );
        };
    }
}