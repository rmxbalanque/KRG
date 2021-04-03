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
                    HeaderTimestamp( HeaderID ID, uint64 timestamp ) : m_ID( ID ), m_timestamp( timestamp ) {}

                    HeaderID    m_ID;
                    uint64         m_timestamp;
                };

            public:

                Reflector();

                bool ParseSolution( FileSystem::Path const& slnPath );
                bool Clean();
                bool Build();

            private:

                bool LogError( char const* pErrorFormat, ... ) const;
                bool ParseProject( FileSystem::Path const& prjPath );

                HeaderProcessResult ProcessHeaderFile( FileSystem::Path const& filePath, String& exportMacro );
                uint64 CalculateHeaderChecksum( FileSystem::Path const& engineIncludePath, FileSystem::Path const& filePath );

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