#pragma once

#include "ClangParserContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            CXChildVisitResult VisitStructure( ClangParserContext* pContext, CXCursor& cr, FileSystemPath const& headerFilePath, HeaderID const headerID );
        }
    }
}