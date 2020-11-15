#include "FileSystem.h"
#include <fstream>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace FileSystem
    {
        bool EnsurePathExists( FileSystemPath const& path )
        {
            if ( !path.Exists() )
            {
                return path.IsDirectoryPath() ? CreateDir( path ) : CreateDir( path.GetParentDirectory() );
            }

            return true;
        }
    }
}
