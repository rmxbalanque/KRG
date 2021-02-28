#pragma once

#include "Applications/Reflector/Database/ReflectionDatabase.h"
#include "System/Core/Time/Time.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            class Reflector
            {
                enum class HeaderProcessResult
                {
                    ErrorOccured,
                    ParseHeader,
                    IgnoreHeader,
                };

                struct HeaderTimestamp
                {
                    HeaderTimestamp( HeaderID ID, U64 timestamp ) : m_ID( ID ), m_timestamp( timestamp ) {}

                    HeaderID    m_ID;
                    U64         m_timestamp;
                };

            public:

                Reflector();

                bool ParseSolution( FileSystemPath const& slnPath );
                bool Clean();
                bool Build();

            private:

                bool LogError( char const* pErrorFormat, ... ) const;
                bool ParseProject( FileSystemPath const& prjPath );

                HeaderProcessResult ProcessHeaderFile( FileSystemPath const& filePath, String& exportMacro );
                U64 CalculateHeaderChecksum( FileSystemPath const& engineIncludePath, FileSystemPath const& filePath );

                bool UpToDateCheck();
                bool ParseRegisteredHeaders();
                bool GenerateCode();
                bool WriteTypeData();

            private:

                String                              m_reflectionDataPath;
                SolutionDesc                        m_solution;
                ReflectionDatabase                  m_database;

                // Up to data checks
                TVector<HeaderTimestamp>            m_registeredHeaderTimestamps;

                // Timing
                Milliseconds                  m_slnParsingTime;
                Milliseconds                  m_upToDateCheckTime;
                Milliseconds                  m_clangParsingTime;
                Milliseconds                  m_clangVisitingTime;
                Milliseconds                  m_codeGenerationTime;
            };
        }
    }
}