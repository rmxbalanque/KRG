#include "KRG_RapidJson.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    JsonReader::~JsonReader()
    {
        if ( m_pStringBuffer != nullptr )
        {
            KRG::Free( m_pStringBuffer );
        }
    }

    bool JsonReader::ReadFromFile( FileSystem::Path const& filePath )
    {
        KRG_ASSERT( filePath.IsFile() );
        Reset();

        //-------------------------------------------------------------------------

        if ( FileSystem::Exists( filePath ) )
        {
            FILE* pFile = fopen( filePath, "r" );

            if ( pFile == nullptr )
            {
                return false;
            }

            fseek( pFile, 0, SEEK_END );
            size_t filesize = (size_t) ftell( pFile );
            fseek( pFile, 0, SEEK_SET );

            m_pStringBuffer = (Byte*) KRG::Alloc( filesize + 1 );
            size_t readLength = fread( m_pStringBuffer, 1, filesize, pFile );
            m_pStringBuffer[readLength] = '\0';
            fclose( pFile );

            m_document.ParseInsitu( (char*) m_pStringBuffer );

            bool const isValidJsonFile = ( m_document.GetParseError() == rapidjson::kParseErrorNone );
            if ( isValidJsonFile )
            {
                OnFileReadSuccess();
            }

            return isValidJsonFile;
        }

        return false;
    }

    bool JsonReader::ReadFromString( char const* pString )
    {
        KRG_ASSERT( pString != nullptr );
        Reset();

        // Copy string data
        //-------------------------------------------------------------------------

        size_t const stringLength = strlen( pString );
        size_t const requiredMemory = sizeof( char ) * ( stringLength + 1 );
        m_pStringBuffer = (Byte*) KRG::Alloc( requiredMemory );
        m_pStringBuffer[stringLength] = '\0';

        memcpy( m_pStringBuffer, pString, requiredMemory - sizeof( char ) );

        // Parse json data
        //-------------------------------------------------------------------------

        m_document.ParseInsitu( (char*) m_pStringBuffer );

        bool const isValidJsonFile = ( m_document.GetParseError() == rapidjson::kParseErrorNone );
        if ( isValidJsonFile )
        {
            OnFileReadSuccess();
        }

        return isValidJsonFile;
    }

    void JsonReader::Reset()
    {
        m_document.Clear();

        if ( m_pStringBuffer != nullptr )
        {
            KRG::Free( m_pStringBuffer );
        }
    }

    //-------------------------------------------------------------------------

    bool JsonWriter::WriteToFile( FileSystem::Path const& outPath )
    {
        KRG_ASSERT( outPath.IsFile() );

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

    void JsonWriter::Reset()
    {
        m_stringBuffer.Clear();
    }
}