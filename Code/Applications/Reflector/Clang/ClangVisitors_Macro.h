#pragma once

#include "ClangParserContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            CXChildVisitResult VisitMacro( ClangParserContext* pContext, HeaderID headerID, CXCursor cr, String const& cursorName );
        }
    }
}