#include "krg_ini.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG
{
    IniFile::IniFile( FileSystem::Path const& filePath )
    {
        KRG_ASSERT( filePath.IsValid() );
        m_pDictionary = iniparser_load( filePath.c_str() );
    }

    IniFile::IniFile()
    {
        m_pDictionary = dictionary_new( 0 );
    }

    IniFile::~IniFile()
    {
        if ( m_pDictionary != nullptr )
        {
            iniparser_freedict( m_pDictionary );
        }
    }

    bool IniFile::HasEntry( char const* key ) const
    {
        KRG_ASSERT( IsValid() );
        return iniparser_find_entry( m_pDictionary, key ) > 0;
    }

    void IniFile::SaveToFile( FileSystem::Path const& filePath ) const
    {
        KRG_ASSERT( IsValid() );

        FILE* pFile;
        pFile = fopen( filePath.c_str(), "w" );
        iniparser_dump_ini( m_pDictionary, pFile );
        fclose( pFile );
    }

    bool IniFile::TryGetBool( char const* key, bool& outValue )
    {
        KRG_ASSERT( IsValid() );

        if ( HasEntry( key ) )
        {
            outValue = (bool) iniparser_getboolean( m_pDictionary, key, false );
            return true;
        }

        return false;
    }

    bool IniFile::TryGetInt( char const* key, int32& outValue )
    {
        KRG_ASSERT( IsValid() );

        if ( HasEntry( key ) )
        {
            outValue = iniparser_getint( m_pDictionary, key, 0 );
            return true;
        }

        return false;
    }

    bool IniFile::TryGetUInt( char const* key, uint32& outValue )
    {
        KRG_ASSERT( IsValid() );

        if ( HasEntry( key ) )
        {
            outValue = ( uint32) iniparser_getint( m_pDictionary, key, 0 );
            return true;
        }

        return false;
    }

    bool IniFile::TryGetString( char const* key, String& outValue )
    {
        KRG_ASSERT( IsValid() );

        if ( HasEntry( key ) )
        {
            outValue = iniparser_getstring( m_pDictionary, key, "" );
            return true;
        }

        return false;
    }

    bool IniFile::TryGetFloat( char const* key, float& outValue )
    {
        KRG_ASSERT( IsValid() );

        if ( HasEntry( key ) )
        {
            outValue = ( float) iniparser_getdouble( m_pDictionary, key, 0.0f );
            return true;
        }

        return false;
    }
}