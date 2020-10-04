#pragma once

#include "ClangParserContext.h"
#include "System/Core/Types/Time.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            class ClangParser
            {

            public:

                ClangParser( SolutionDesc* pSolution, ReflectionDatabase* pDatabase, String const& reflectionDataPath );

                inline Milliseconds GetParsingTime() const { return m_totalParsingTime; }
                inline Milliseconds GetVisitingTime() const { return m_totalVisitingTime; }

                bool Parse( TVector<HeaderDesc*> const& headers );
                String GetErrorMessage() const { return m_context.GetErrorMessage(); }

            private:

                ClangParserContext                  m_context;
                Milliseconds                        m_totalParsingTime;
                Milliseconds                        m_totalVisitingTime;
                String                              m_reflectionDataPath;
            };
        }
    }
}
