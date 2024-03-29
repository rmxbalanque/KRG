#pragma once

#include "ClangParserContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            CXChildVisitResult VisitStructure( ClangParserContext* pContext, CXCursor& cr, FileSystem::Path const& headerFilePath, HeaderID const headerID );
        }
    }
}