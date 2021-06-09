#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Math/Matrix.h"
#include "System/Core/Types/String.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

namespace KRG::RawAssets
{
    class KRG_TOOLS_CORE_API RawAsset
    {

    public:

        RawAsset() = default;
        virtual ~RawAsset() = default;

        virtual bool IsValid() const = 0;

        inline bool HasWarnings() const { return !m_warnings.empty(); }
        inline TVector<String> const& GetWarnings() const { return m_warnings; }

        inline bool HasErrors() const { return !m_errors.empty(); }
        inline TVector<String> const& GetErrors() const { return m_errors; }

    protected:

        void LogError( char const* pFormat, ... )
        {
            char buffer[512];
            va_list args;
            va_start( args, pFormat );
            VPrintf( buffer, 512, pFormat, args );
            va_end( args );
            m_errors.emplace_back( buffer );
        }

        void LogWarning( char const* pFormat, ... )
        {
            char buffer[512];
            va_list args;
            va_start( args, pFormat );
            VPrintf( buffer, 512, pFormat, args );
            va_end( args );
            m_warnings.emplace_back( buffer );
        }

    protected:

        TVector<String>                     m_warnings;
        TVector<String>                     m_errors;
    };
}