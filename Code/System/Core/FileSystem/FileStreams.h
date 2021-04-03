#pragma once

#include "FileSystem.h"
#include <fstream>
#include <iosfwd>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace FileSystem
    {
        class KRG_SYSTEM_CORE_API InputFileStream
        {
        public:

            InputFileStream( Path const& filePath );
            inline bool IsValid() const { return m_filestream.is_open(); }
            inline std::ifstream& GetStream() { return m_filestream; }

            inline void Read( void* pData, size_t size )
            {
                KRG_ASSERT( IsValid() );
                m_filestream.read( (char*) pData, size );
            }

            inline void Close()
            {
                KRG_ASSERT( IsValid() );
                m_filestream.close();
            }

        private:

            InputFileStream();

        private:

            std::ifstream m_filestream;
        };

        //-------------------------------------------------------------------------
        
        class KRG_SYSTEM_CORE_API OutputFileStream
        {
        public:

            OutputFileStream( Path const& filePath );
            inline std::ofstream& GetStream() { return m_filestream; }
            inline bool IsValid() const { return m_filestream.is_open(); }

            inline void Write( void* pData, size_t size )
            {
                KRG_ASSERT( IsValid() );
                m_filestream.write( (char*) pData, size );
            }
            
            inline void Close()
            {
                KRG_ASSERT( IsValid() );
                m_filestream.close();
            }

        private:

            OutputFileStream();

        private:

            std::ofstream m_filestream;
        };
    }
}