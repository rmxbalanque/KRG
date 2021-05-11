#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Core/Memory/Memory.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/stringbuffer.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/writer.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/prettywriter.h"
#include "System/Core/ThirdParty/cereal/external/rapidjson/document.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace FileSystem { class Path; }

    //-------------------------------------------------------------------------

    struct RapidJsonAllocator
    {
        inline void* Malloc( size_t size ) { return KRG::Alloc( size ); }
        inline void* Realloc( void* pOriginalPtr, size_t originalSize, size_t newSize ) { return KRG::Realloc( pOriginalPtr, newSize ); }
        inline static void Free( void* pData ) { KRG::Free( pData ); }
    };

    using JsonCharType = rapidjson::UTF8<char>;
    using RapidJsonStringBuffer = rapidjson::GenericStringBuffer<JsonCharType, RapidJsonAllocator>;
    using RapidJsonWriter = rapidjson::PrettyWriter<RapidJsonStringBuffer>;

    // File reader
    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API JsonFileReader
    {
    public:

        virtual ~JsonFileReader();

        // Read entire json file
        bool ReadFromFile( FileSystem::Path const& filePath );

        // Get the read document
        inline rapidjson::Document const& GetDocument() { return m_document; }

    protected:

        // Reset the reader state
        virtual void Reset();

        // Called if we successfully read and parsed the json data
        virtual void OnFileReadSuccess() {}

    protected:

        rapidjson::Document                                         m_document;

    private:

        Byte*                                                       m_pFileBuffer = nullptr;
    };

    // File writer
    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API JsonFileWriter
    {
    public:

        virtual ~JsonFileWriter() = default;

        // Get the writer
        inline RapidJsonWriter* GetWriter() { return &m_writer; }

        // Write currently serialized data to disk and reset serialized data
        bool WriteToFile( FileSystem::Path const& outPath );

        // Reset all serialized data without writing to disk
        virtual void Reset();

    protected:

        // Called as part of the write to file function to allow clients to finalize serialized data
        virtual void FinalizeSerializedData() {}

    protected:

        RapidJsonWriter                                             m_writer = RapidJsonWriter( m_stringBuffer );
        RapidJsonStringBuffer                                       m_stringBuffer;
    };
}