#pragma once

#include "../_Module/API.h"
#include "System/Core/Math/Math.h"
#include "System/Core/Math/NumericRange.h"
#include "System/Core/Types/String.h"
#include "System/Core/Types/BitFlags.h"
#include "System/Core/Threading/Threading.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Debug
{
    enum class DepthTestState : uint8
    {
        On,
        Off
    };

    //-------------------------------------------------------------------------

    enum class TextAlignment : uint8
    {
        TopLeft = 0,
        TopCenter,
        TopRight,
        MiddleLeft,
        MiddleCenter,
        MiddleRight,
        BottomLeft,
        BottomCenter,
        BottomRight,
    };

    enum class TextSize : uint8
    {
        Regular,
        Small
    };

    enum class TextCoordinateSpace : uint8
    {
        Screen,
        World
    };

    enum class TextBackground : uint8
    {
        Transparent,
        Opaque
    };
}

//-------------------------------------------------------------------------

namespace KRG::Debug::Drawing
{
    struct PointCommand
    {
        PointCommand( Float3 const& position, Float4 const& color, float pointThickness = 1.0f )
            : m_position( position )
            , m_thickness( pointThickness )
            , m_color( color )
        {}

        KRG_FORCE_INLINE bool IsTransparent() const { return m_color[3] != 1.0f; }

        Float3      m_position;
        float       m_thickness;
        Float4      m_color;
    };

    //-------------------------------------------------------------------------

    struct LineCommand
    {
        LineCommand( Float3 const& startPosition, Float3 const& endPosition, Float4 const& color, float lineThickness )
            : m_startPosition( startPosition )
            , m_endPosition( endPosition )
            , m_startColor( color )
            , m_endColor( color )
            , m_startThickness( lineThickness )
            , m_endThickness( lineThickness )
        {}

        LineCommand( Float3 const& startPosition, Float3 const& endPosition, Float4 const& startColor, Float4 const& endColor, float lineThickness )
            : m_startPosition( startPosition )
            , m_endPosition( endPosition )
            , m_startColor( startColor )
            , m_endColor( endColor )
            , m_startThickness( lineThickness )
            , m_endThickness( lineThickness )
        {}

        LineCommand( Float3 const& startPosition, Float3 const& endPosition, Float4 const& color, float startThickness, float endThickness )
            : m_startPosition( startPosition )
            , m_endPosition( endPosition )
            , m_startColor( color )
            , m_endColor( color )
            , m_startThickness( startThickness )
            , m_endThickness( endThickness )
        {}

        LineCommand( Float3 const& startPosition, Float3 const& endPosition, Float4 const& startColor, Float4 const& endColor, float startThickness, float endThickness )
            : m_startPosition( startPosition )
            , m_endPosition( endPosition )
            , m_startColor( startColor )
            , m_endColor( endColor )
            , m_startThickness( startThickness )
            , m_endThickness( endThickness )
        {}

        KRG_FORCE_INLINE bool IsTransparent() const { return m_startColor[3] != 1.0f || m_endColor[3] != 1.0f; }

        Float3      m_startPosition;
        float       m_startThickness;
        Float4      m_startColor;
        Float3      m_endPosition;
        float       m_endThickness;
        Float4      m_endColor;
    };

    //-------------------------------------------------------------------------

    struct TriangleCommand
    {
        TriangleCommand( Float3 const& V0, Float3 const& V1, Float3 const& V2, Float4 const& color )
            : m_vertex0( V0 )
            , m_vertex1( V1 )
            , m_vertex2( V2 )
            , m_color0( color )
            , m_color1( color )
            , m_color2( color )
        {}

        TriangleCommand( Float3 const& V0, Float3 const& V1, Float3 const& V2, Float4 const& color0, Float4 const& color1, Float4 const& color2 )
            : m_vertex0( V0 )
            , m_vertex1( V1 )
            , m_vertex2( V2 )
            , m_color0( color0 )
            , m_color1( color1 )
            , m_color2( color2 )
        {}

        KRG_FORCE_INLINE bool IsTransparent() const { return m_color0[3] != 1.0f || m_color1[3] != 1.0f || m_color2[3] != 1.0f; }

        Float4      m_vertex0;
        Float4      m_color0;
        Float4      m_vertex1;
        Float4      m_color1;
        Float4      m_vertex2;
        Float4      m_color2;
    };

    //-------------------------------------------------------------------------

    struct TextCommand
    {
        TextCommand( Float2 const& position, char const* pText, Float4 const& color, TextSize size, TextAlignment alignment, TextBackground background )
            : m_position( position.m_x, position.m_y, 0 )
            , m_text( pText )
            , m_color( color )
            , m_fontSize( size )
            , m_alignment( alignment )
            , m_background( background )
            , m_coordinateSpace( TextCoordinateSpace::Screen )
        {}

        TextCommand( Float3 const& position, char const* pText, Float4 const& color, TextSize size, TextAlignment alignment, TextBackground background )
            : m_position( position )
            , m_text( pText )
            , m_color( color )
            , m_fontSize( size )
            , m_alignment( alignment )
            , m_background( background )
            , m_coordinateSpace( TextCoordinateSpace::World )
        {}

        KRG_FORCE_INLINE bool IsTransparent() const { return m_color[3] != 1.0f; }

        Float4                      m_color;
        Float3                      m_position;
        TextSize                    m_fontSize;
        TextAlignment               m_alignment;
        TextCoordinateSpace         m_coordinateSpace;
        TextBackground              m_background;
        String                      m_text;
    };

    //-------------------------------------------------------------------------

    struct CommandBuffer
    {
        inline void Append( CommandBuffer const& buffer )
        {
            m_pointCommands.reserve( m_pointCommands.size() + buffer.m_pointCommands.size() );
            m_pointCommands.insert( m_pointCommands.end(), buffer.m_pointCommands.begin(), buffer.m_pointCommands.end() );

            m_lineCommands.reserve( m_lineCommands.size() + buffer.m_lineCommands.size() );
            m_lineCommands.insert( m_lineCommands.end(), buffer.m_lineCommands.begin(), buffer.m_lineCommands.end() );

            m_triangleCommands.reserve( m_triangleCommands.size() + buffer.m_triangleCommands.size() );
            m_triangleCommands.insert( m_triangleCommands.end(), buffer.m_triangleCommands.begin(), buffer.m_triangleCommands.end() );

            m_textCommands.reserve( m_textCommands.size() + buffer.m_textCommands.size() );
            m_textCommands.insert( m_textCommands.end(), buffer.m_textCommands.begin(), buffer.m_textCommands.end() );
        }

        inline void Clear()
        {
            m_pointCommands.clear();
            m_lineCommands.clear();
            m_triangleCommands.clear();
            m_textCommands.clear();
        }

    public:

        TVector<PointCommand>       m_pointCommands;
        TVector<LineCommand>        m_lineCommands;
        TVector<TriangleCommand>    m_triangleCommands;
        TVector<TextCommand>        m_textCommands;
    };

    //-------------------------------------------------------------------------

    class ThreadCommandBuffer
    {

    public:

        ThreadCommandBuffer( Threading::ThreadID threadID )
            : m_ID( threadID )
        {}

        inline Threading::ThreadID GetThreadID() const { return m_ID; }

        inline void AddCommand( PointCommand&& cmd, DepthTestState depthTestState )
        {
            CommandBuffer* pBuffer = GetCommandBuffer( depthTestState, cmd.IsTransparent() );
            pBuffer->m_pointCommands.emplace_back( cmd );
        }

        inline void AddCommand( LineCommand&& cmd, DepthTestState depthTestState )
        {
            CommandBuffer* pBuffer = GetCommandBuffer( depthTestState, cmd.IsTransparent() );
            pBuffer->m_lineCommands.emplace_back( cmd );
        }

        inline void AddCommand( TriangleCommand&& cmd, DepthTestState depthTestState )
        {
            CommandBuffer* pBuffer = GetCommandBuffer( depthTestState, cmd.IsTransparent() );
            pBuffer->m_triangleCommands.emplace_back( cmd );
        }

        inline void AddCommand( TextCommand&& cmd, DepthTestState depthTestState )
        {
            CommandBuffer* pBuffer = GetCommandBuffer( depthTestState, cmd.IsTransparent() );
            pBuffer->m_textCommands.emplace_back( cmd );
        }

        inline void Clear()
        {
            m_opaqueDepthOn.Clear();
            m_opaqueDepthOff.Clear();
            m_transparentDepthOn.Clear();
            m_transparentDepthOff.Clear();
        }

        CommandBuffer const& GetOpaqueDepthTestEnabledBuffer() const { return m_opaqueDepthOn; }
        CommandBuffer const& GetOpaqueDepthTestDisabledBuffer() const { return m_opaqueDepthOff; }
        CommandBuffer const& GetTransparentDepthTestEnabledBuffer() const { return m_transparentDepthOn; }
        CommandBuffer const& GetTransparentDepthTestDisabledBuffer() const { return m_transparentDepthOff; }

    private:

        inline CommandBuffer* GetCommandBuffer( DepthTestState depthTestState, bool isTransparent )
        {
            CommandBuffer* pBuffer = nullptr;

            if ( depthTestState == DepthTestState::On )
            {
                pBuffer = isTransparent ? &m_transparentDepthOn : &m_opaqueDepthOn;
            }
            else // Disable depth test
            {
                pBuffer = isTransparent ? &m_transparentDepthOff : &m_opaqueDepthOff;
            }

            return pBuffer;
        }

    private:

        Threading::ThreadID         m_ID;
        CommandBuffer               m_opaqueDepthOn;
        CommandBuffer               m_opaqueDepthOff;
        CommandBuffer               m_transparentDepthOn;
        CommandBuffer               m_transparentDepthOff;
    };

    //-------------------------------------------------------------------------

    class KRG_SYSTEM_CORE_API FrameCommandBuffer
    {
    public:

        void AddThreadCommands( ThreadCommandBuffer const& threadCommands );

        inline void Clear()
        {
            m_opaqueDepthOn.Clear();
            m_opaqueDepthOff.Clear();
            m_transparentDepthOn.Clear();
            m_transparentDepthOff.Clear();
        }

    public:

        CommandBuffer               m_opaqueDepthOn;
        CommandBuffer               m_opaqueDepthOff;
        CommandBuffer               m_transparentDepthOn;
        CommandBuffer               m_transparentDepthOff;
    };
}
#endif