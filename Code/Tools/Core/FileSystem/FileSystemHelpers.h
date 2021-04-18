#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

#if _WIN32
namespace KRG::FileSystem
{
    KRG_TOOLS_CORE_API void OpenInExplorer( FileSystem::Path const& path );
}
#endif