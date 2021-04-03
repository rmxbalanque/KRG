#pragma once
#include "../_Module/API.h"
#include "System/Core/Types/String.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "iniparser.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_SYSTEM_CORE_API IniFile
    {

    public:

        IniFile();
        IniFile( FileSystem::Path const& filePath );
        ~IniFile();

        inline bool IsValid() const { return m_pDictionary != nullptr; }

        void SaveToFile( FileSystem::Path const& filePath ) const;

        bool HasEntry( char const* key ) const;

        //-------------------------------------------------------------------------

        bool TryGetBool( char const* key, bool& outValue );
        bool TryGetInt( char const* key, int32& outValue );
        bool TryGetUInt( char const* key, uint32& outValue );
        bool TryGetString( char const* key, String& outValue );
        bool TryGetFloat( char const* key, float& outValue );

        inline bool GetBoolOrDefault( char const* key, bool defaultValue )
        {
            KRG_ASSERT( IsValid() );
            return (bool) iniparser_getboolean( m_pDictionary, key, defaultValue );
        }

        inline int32 GetIntOrDefault( char const* key, int32 defaultValue )
        {
            KRG_ASSERT( IsValid() );
            return ( int32) iniparser_getint( m_pDictionary, key, defaultValue );
        }

        inline uint32 GetUIntOrDefault( char const* key, uint32 defaultValue )
        {
            KRG_ASSERT( IsValid() );
            return ( uint32) iniparser_getint( m_pDictionary, key, defaultValue );
        }

        inline KRG::String GetStringOrDefault( char const* key, String const& defaultValue )
        {
            KRG_ASSERT( IsValid() );
            return iniparser_getstring( m_pDictionary, key, defaultValue.c_str() );
        }

        inline KRG::String GetStringOrDefault( char const* key, char const* pDefaultValue )
        {
            KRG_ASSERT( IsValid() );
            return iniparser_getstring( m_pDictionary, key, pDefaultValue );
        }

        inline float GetFloatOrDefault( char const* key, float defaultValue )
        {
            KRG_ASSERT( IsValid() );
            return ( float) iniparser_getdouble( m_pDictionary, key, defaultValue );
        }

        //-------------------------------------------------------------------------

        inline void CreateSection( char const* section )
        {
            KRG_ASSERT( IsValid() );
            iniparser_set( m_pDictionary, section, nullptr );
        }

        inline void SetBool( char const* key, bool value )
        {
            KRG_ASSERT( IsValid() );
            iniparser_set( m_pDictionary, key, value ? "True" : "False" );
        }

        inline void SetInt( char const* key, int32 value )
        {
            KRG_ASSERT( IsValid() );
            char buffer[255];
            Printf( buffer, 255, "%d", value );
            iniparser_set( m_pDictionary, key, buffer );
        }

        inline void SetUInt( char const* key, uint32 value )
        {
            KRG_ASSERT( IsValid() );
            char buffer[255];
            Printf( buffer, 255, "%d", value );
            iniparser_set( m_pDictionary, key, buffer );
        }

        inline void SetFloat( char const* key, float value )
        {
            KRG_ASSERT( IsValid() );
            char buffer[255];
            Printf( buffer, 255, "%f", value );
            iniparser_set( m_pDictionary, key, buffer );
        }

        inline void SetString( char const* key, char const* pString )
        {
            KRG_ASSERT( IsValid() );
            iniparser_set( m_pDictionary, key, pString );
        }

        inline void SetString( char const* key, String const& string )
        {
            KRG_ASSERT( IsValid() );
            iniparser_set( m_pDictionary, key, string.c_str() );
        }

    private:

        dictionary*         m_pDictionary = nullptr;
    };
}