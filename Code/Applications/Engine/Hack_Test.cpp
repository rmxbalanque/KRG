#include "Hack_Test.h"
#include "System/Core/Math/Random.h"
#include "System/Core/Time/Timers.h"
#include "System/Core/Math/BVH/AABBTree.h"

//-------------------------------------------------------------------------

namespace KRG
{
    static constexpr S32 const NumBoxes = 100;
    static constexpr F32 const ExtentRangeMin = 0.75f;
    static constexpr F32 const ExtentRangeMax = 6.0f;
    static constexpr F32 const CenterRange = 60.0f;
    Vector g_centerPoints[NumBoxes];
    AABB g_testBoxes[NumBoxes];
    U64 g_testBoxIDs[NumBoxes];

    void HACK_Test( UpdateContext const& context )
    {
        auto drawingContext = context.GetDrawingContext();

        //-------------------------------------------------------------------------

        static bool isInitialized = false;
        if ( !isInitialized )
        {
            for ( auto i = 0; i < NumBoxes; i++ )
            {
                g_centerPoints[i] = Vector( Math::GetRandomFloat( -CenterRange, CenterRange ), Math::GetRandomFloat( -CenterRange, CenterRange ), Math::GetRandomFloat( -CenterRange, CenterRange ) );
                Vector extents( Math::GetRandomFloat( ExtentRangeMin, ExtentRangeMax ), Math::GetRandomFloat( ExtentRangeMin, ExtentRangeMax ), Math::GetRandomFloat( ExtentRangeMin, ExtentRangeMax ) );
                g_testBoxes[i] = AABB( g_centerPoints[i], extents );
            }

            isInitialized = true;
        }

        //-------------------------------------------------------------------------

        for ( auto i = 0; i < NumBoxes; i++ )
        {
            drawingContext.DrawPoint( g_centerPoints[i], Colors::LightPink, 10.0f );
        }

        //-------------------------------------------------------------------------

        /*Math::AABBTree tree;
        for ( auto i = 0; i < NumBoxes; i++ )
        {
            tree.InsertBox( g_testBoxes[i], i + 1 );
            drawingContext.DrawBox( g_testBoxes[i], Colors::Pink.GetAlphaVersion( 0.25f ) );
            drawingContext.DrawWireBox( g_testBoxes[i], Colors::Red, 2.0f, Debug::Drawing::DepthTestState::Off );
        }*/

        for ( auto i = 0; i < NumBoxes; i++ )
        {
            drawingContext.DrawBox( g_testBoxes[i], Colors::Pink.GetAlphaVersion( 0.25f ) );
            drawingContext.DrawWireBox( g_testBoxes[i], Colors::Red, 2.0f, Debug::Drawing::DepthTestState::Off );
        }

        //-------------------------------------------------------------------------

        //drawingContext.Draw( tree );

        ////-------------------------------------------------------------------------

        //Matrix viewOrigin( EulerAngles( 0, 0, 33 ), Vector( 1.5f, 0.5f, -0.5f ) );

        //Math::ViewVolume::OrthographicSettings orthoSettings;
        //orthoSettings.m_width = 10.0f;
        //orthoSettings.m_height = 5.0f;

        //Math::ViewVolume::PerspectiveSettings perspectiveSettings;
        //perspectiveSettings.m_aspectRatio = 4.0f / 3;
        //perspectiveSettings.m_verticalFOV = Degrees( 10.0f );

        //Math::ViewVolume volume( perspectiveSettings, TRange<F32>( 5.0f, 100.0f ), viewOrigin );

        //drawingContext.Draw( volume );

        ////-------------------------------------------------------------------------

        //auto queryBox = volume.GetAABB();
        //TVector<U64> results;
        //tree.FindOverlaps( queryBox, results );

        //drawingContext.DrawBox( queryBox, Colors::LimeGreen.GetAlphaVersion( 0.25f ) );
        //for ( auto result : results )
        //{
        //    drawingContext.DrawPoint( g_testBoxes[result - 1].GetCenter(), Colors::Red, 10.0f );
        //}

        //-------------------------------------------------------------------------
        //-------------------------------------------------------------------------

       /* Sphere s0( g_centerPoints, NumBoxes );
        drawingContext.DrawSphere( s0, Colors::Red, 3.0f );*/
    }
}