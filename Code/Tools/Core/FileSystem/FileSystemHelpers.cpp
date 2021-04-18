#include "FileSystemHelpers.h"
#include <shellapi.h>

//-------------------------------------------------------------------------

#if _WIN32
namespace KRG::FileSystem
{
    void OpenInExplorer( FileSystem::Path const& path )
    {
        InlineString<255> cmdLine( "/select," );
        cmdLine += path.c_str();
        ShellExecute( 0, NULL, "explorer.exe", cmdLine.c_str(), NULL, SW_SHOWNORMAL );
    }
}
#endif