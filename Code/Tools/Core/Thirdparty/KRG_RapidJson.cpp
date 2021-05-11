#include "KRG_RapidJson.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    JsonFileReader::~JsonFileReader()
    {
        if ( m_pFileBuffer != nullptr )
        {
            KRG::Free( m_pFileBuffer );
        }
    }

    bool JsonFileReader::ReadFromFile( FileSystem::Path const& filePath )
    {
        KRG_ASSERT( filePath.IsFilePath() );
        Reset();

        //-------------------------------------------------------------------------

        if ( filePath.Exists() )
        {
            FILE* pFile = fopen( filePath, "r" );

            if ( pFile == nullptr )
            {
                return false;
            }

            fseek( pFile, 0, SEEK_END );
            size_t filesize = (size_t) ftell( pFile );
            fseek( pFile, 0, SEEK_SET );

            m_pFileBuffer = (Byte*) KRG::Alloc( filesize + 1 );
            size_t readLength = fread( m_pFileBuffer, 1, filesize, pFile );
            m_pFileBuffer[readLength] = '\0';
            fclose( pFile );

            m_document.ParseInsitu( (char*) m_pFileBuffer );

            bool const isValidJsonFile = ( m_document.GetParseError() == rapidjson::kParseErrorNone );
            if ( isValidJsonFile )
            {
                OnFileReadSuccess();
            }

            return isValidJsonFile;
        }

        return true;
    }

    void JsonFileReader::Reset()
    {
        m_document.Clear();

        if ( m_pFileBuffer != nullptr )
        {
            KRG::Free( m_pFileBuffer );
        }
    }

    //-------------------------------------------------------------------------

    bool JsonFileWriter::WriteToFile( FileSystem::Path const& outPath )
    {
        KRG_ASSERT( outPath.IsFilePath() );

        FinalizeSerializedData();

        //-------------------------------------------------------------------------

        FileSystem::EnsurePathExists( outPath );

        FILE* pFile = fopen( outPath, "wb" );
        if ( pFile == nullptr )
        {
            Reset();
            return false;
        }

        fwrite( m_stringBuffer.GetString(), m_stringBuffer.GetSize(), 1, pFile );
        fclose( pFile );

        Reset();
        return true;
    }

    void JsonFileWriter::Reset()
    {
        m_stringBuffer.Clear();
    }
}