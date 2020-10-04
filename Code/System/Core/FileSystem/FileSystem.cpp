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

        //-------------------------------------------------------------------------

        bool LoadFile( FileSystemPath const& path, TVector<Byte>& fileData, FileType fileType )
        {
            KRG_ASSERT( path.IsFilePath() );

            std::ifstream file;
            file.open( path, std::ios::in | std::ios::binary | std::ios::ate );
            if ( file.is_open() )
            {
                bool const isASCII = ( fileType == FileType::ASCII );

                U32 const size = (U32) file.tellg() + ( isASCII ? 1 : 0 );
                if ( size > 0 )
                {
                    fileData.resize( size );

                    file.seekg( 0, std::ios::beg );
                    file.read( (char*) fileData.data(), size );
                    file.close();

                    // Add terminating null character to the file data
                    if ( isASCII )
                    {
                        fileData.back() = 0;
                    }

                    return true;
                }
            }

            return false;
        }
    }
}
