#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <shellapi.h>
#include <assert.h>
#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>

#if _MSC_VER
#pragma warning(push, 0)
#pragma warning(disable: 4702)
#endif

// Note: this has been modified for this application
#include "cmdParser.h"

#if _MSC_VER
#pragma warning(pop)
#endif

//-------------------------------------------------------------------------

namespace FileSystemHelpers
{
    static std::string GetFullPathString( char const* pPath )
    {
        assert( pPath != nullptr && pPath[0] != 0 );

        char fullpath[256] = { 0 };
        DWORD length = GetFullPathNameA( pPath, 256, fullpath, nullptr );
        assert( length != 0 && length != 255 );

        // We always append the trailing slash to simplify further operations
        DWORD const result = GetFileAttributesA( fullpath );
        if ( result != INVALID_FILE_ATTRIBUTES && ( result & FILE_ATTRIBUTE_DIRECTORY ) && fullpath[length - 1] != '\\' )
        {
            fullpath[length] = '\\';
            fullpath[length + 1] = 0;
        }

        return std::string( fullpath );
    }

    static std::string GetFullPathString( std::string const& path )
    {
        return GetFullPathString( path.c_str() );
    }

    static std::string GetParentDirectoryPath( std::string const& path )
    {
        std::string dirPath;
        size_t const lastSlashIdx = path.rfind( '\\' );
        if ( lastSlashIdx != std::string::npos )
        {
            dirPath = path.substr( 0, lastSlashIdx + 1 );
        }

        return dirPath;
    }
}

//-------------------------------------------------------------------------

static void PrintErrorAndHelp( char const* pErrorMessage = nullptr )
{
    printf( "================================================\n" );
    printf( "Simple Embed\n" );
    printf( "================================================\n" );
    printf( "2021 - Bobby Anguelov - MIT License\n\n" );

    if ( pErrorMessage != nullptr )
    {
        printf( "Error! %s\n\n", pErrorMessage );
    }

    printf( "Usage: -i <input file path> -o <output resource name> {-ns <namespace>} {-dll <dll api macro>}\n" );
}

static bool SaveStreamToFile( std::string const& filePath, std::stringstream& stream )
{
    // Rewind stream to beginning
    stream.seekg( std::ios::beg );

    // Open existing file and compare contents to the newly generated stream
    std::fstream outputFile( filePath.c_str(), std::ios::out | std::ios::trunc );
    if ( !outputFile.is_open() )
    {
        return false;
    }

    stream.seekg( std::ios::beg );
    outputFile << stream.rdbuf();
    outputFile.close();

    return true;
}

//-------------------------------------------------------------------------

namespace Encoding::Base64
{
    static char const g_charTable[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    //-------------------------------------------------------------------------

    std::vector<char> Encode( std::vector<char> const& data )
    {
        char const* pDataToEncode = data.data();

        size_t dataSize = data.size();
        assert( pDataToEncode != nullptr && dataSize > 0 );

        std::vector<char> encodedData;
        int32_t i = 0, j = 0;
        char byte3[3] = { 0 };
        char byte4[4] = { 0 };

        while ( dataSize-- )
        {
            byte3[i++] = *( pDataToEncode++ );
            if ( i == 3 )
            {
                byte4[0] = static_cast<char>( ( byte3[0] & 0xfc ) >> 2 );
                byte4[1] = static_cast<char>( ( ( byte3[0] & 0x03 ) << 4 ) + ( ( byte3[1] & 0xf0 ) >> 4 ) );
                byte4[2] = static_cast<char>( ( ( byte3[1] & 0x0f ) << 2 ) + ( ( byte3[2] & 0xc0 ) >> 6 ) );
                byte4[3] = static_cast<char>( byte3[2] & 0x3f );

                for ( i = 0; ( i < 4 ); i++ )
                {
                    encodedData.emplace_back( g_charTable[byte4[i]] );
                }
                i = 0;
            }
        }

        if ( i )
        {
            for ( j = i; j < 3; j++ )
            {
                byte3[j] = '\0';
            }

            byte4[0] = ( byte3[0] & 0xfc ) >> 2;
            byte4[1] = ( ( byte3[0] & 0x03 ) << 4 ) + ( ( byte3[1] & 0xf0 ) >> 4 );
            byte4[2] = ( ( byte3[1] & 0x0f ) << 2 ) + ( ( byte3[2] & 0xc0 ) >> 6 );
            byte4[3] = byte3[2] & 0x3f;

            for ( j = 0; ( j < i + 1 ); j++ )
            {
                encodedData.emplace_back( g_charTable[byte4[j]] );
            }

            while ( ( i++ < 3 ) )
            {
                encodedData.emplace_back( '=' );
            }
        }

        return encodedData;
    }
}

//-------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
    cli::Parser cmdParser( argc, argv );
    cmdParser.disable_help();
    cmdParser.set_required<std::string>( "i", "input", "" );
    cmdParser.set_required<std::string>( "o", "output", "" );
    cmdParser.set_optional<std::string>( "ns", "namespace", "" );
    cmdParser.set_optional<std::string>( "dll", "dll", "" );

    if ( cmdParser.run() )
    {
        auto inputFilePath = cmdParser.get<std::string>( "i" );
        auto outputName = cmdParser.get<std::string>( "o" );

        auto namespaceName = cmdParser.get<std::string>( "ns" );
        auto dllExportMacro = cmdParser.get<std::string>( "dll" );

        //-------------------------------------------------------------------------
        // Read file
        //-------------------------------------------------------------------------

        std::string const& inputFullPath = FileSystemHelpers::GetFullPathString( inputFilePath );

        std::ifstream file( inputFullPath, std::ios::binary | std::ios::ate );
        if ( !file.is_open() )
        {
            PrintErrorAndHelp( "Failed to open input file" );
            return 1;
        }

        std::streamsize size = file.tellg();
        file.seekg( 0, std::ios::beg );

        std::vector<char> buffer( size );
        if ( !file.read( buffer.data(), size ) )
        {
            PrintErrorAndHelp( "Failed to read file contents" );
            return 1;
        }

        file.close();

        //-------------------------------------------------------------------------
        // Compress contents
        //-------------------------------------------------------------------------

        std::vector<char> encodedData = Encoding::Base64::Encode( buffer );
        size_t const encodedDataSize = encodedData.size();
        
        //-------------------------------------------------------------------------
        // Dump contents to disk
        //-------------------------------------------------------------------------

        std::stringstream stream;
        
        //-------------------------------------------------------------------------

        {
            std::string const headerFile = FileSystemHelpers::GetParentDirectoryPath( inputFullPath ) + outputName + ".h";

            stream << "#pragma once" << std::endl;
            stream << "#include <stdint.h>" << std::endl << std::endl;
            stream << "//-------------------------------------------------------------------------" << std::endl << std::endl;

            if ( !namespaceName.empty() )
            {
                stream << "namespace " << namespaceName << "::" << outputName << std::endl;
            }
            else
            {
                stream << "namespace " << outputName << std::endl;
            }

            stream << "{" << std::endl;
            if ( dllExportMacro.empty() )
            {
                stream << "    " << "uint8_t const* GetData();" << std::endl;
            }
            else
            {
                stream << "    " << dllExportMacro << " uint8_t const* GetData();" << std::endl;
            }
            stream << "    inline size_t GetDataSize() { return " << encodedDataSize << "; }" << std::endl;
            stream << "};";

            SaveStreamToFile( headerFile, stream );
        }

        //-------------------------------------------------------------------------

        {
            std::string const implementationFile = FileSystemHelpers::GetParentDirectoryPath( inputFullPath ) + outputName + ".cpp";

            stream.str( std::string() );
            stream << "#include \"" << outputName + ".h\"" << std::endl << std::endl;
            stream << "//-------------------------------------------------------------------------" << std::endl << std::endl;

            if ( !namespaceName.empty() )
            {
                stream << "namespace " << namespaceName << "::" << outputName << std::endl;
            }
            else
            {
                stream << "namespace " << outputName << std::endl;
            }

            stream << "{" << std::endl;
            stream << "    static uint8_t const g_encodedData[" << encodedDataSize << "] = {";

            for ( auto i = 0; i < encodedDataSize; i++ )
            {
                stream << (uint32_t) encodedData[i];

                if ( i != encodedDataSize - 1 )
                {
                    stream << ", ";
                }
            }

            stream << "};" << std::endl << std::endl;

            stream << "    " << "uint8_t const* GetData() { return g_encodedData; }" << std::endl;

            stream << "};";

            SaveStreamToFile( implementationFile, stream );
        }

        return 0;
    }
    else
    {
        PrintErrorAndHelp();
    }

    return 1;
}