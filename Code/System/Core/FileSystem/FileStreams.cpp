#include "FileStreams.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::FileSystem
{
    InputFileStream::InputFileStream( Path const& filePath )
    {
        KRG_ASSERT( filePath.IsFile() );
        if ( !FileSystem::Exists( filePath ) )
        {
            return;
        }

        m_filestream.open( filePath, std::ios::in | std::ios::binary );

        if ( !m_filestream.is_open() )
        {
            String const msg = "Error opening file ( " + (String) filePath + " ) for reading - " + strerror( errno );
            KRG_LOG_WARNING( "IO", msg.c_str() );
        }
    }

    OutputFileStream::OutputFileStream( Path const& filePath )
    {
        KRG_ASSERT( filePath.IsFile() );
        FileSystem::EnsurePathExists( filePath );
        m_filestream.open( filePath, std::ios::out | std::ios::trunc | std::ios::binary );

        if ( !m_filestream.is_open() )
        {
            String const msg = "Error opening file ( " + (String) filePath + " ) for writing- " + strerror( errno );
            KRG_LOG_WARNING( "IO", msg.c_str() );
        }
    }
}