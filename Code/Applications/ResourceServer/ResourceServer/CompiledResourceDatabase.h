#pragma once

#include "Tools/Core/ThirdParty/sqlite/SqliteHelpers.h"
#include "System/Resource/ResourceID.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        struct CompiledResourceRecord
        {
            inline bool IsValid() const { return m_resourceID.IsValid(); }

            ResourceID      m_resourceID;
            int32             m_compilerVersion = -1;         // The compiler version used for the last compilation
            uint64             m_fileTimestamp = 0;            // The timestamp of the resource file
            uint64             m_sourceTimestampHash = 0;      // The timestamp hash of any source assets used in the compilation
        };

        //-------------------------------------------------------------------------

        class CompiledResourceDatabase : public SQLite::SQLiteDatabase
        {
        public:

            bool TryConnect( FileSystemPath const& databasePath );

            bool CleanDatabase( FileSystemPath const& databasePath );

            // Database functions
            CompiledResourceRecord GetRecord( ResourceID resourceID ) const;
            bool WriteRecord( CompiledResourceRecord const& record );

        private:

            bool CreateTables();
            bool DropTables();
        };
    }
}