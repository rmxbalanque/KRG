#if _WIN32
#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

struct sqlite3;
struct sqlite3_stmt;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace SQLite
    {
        class KRG_TOOLS_CORE_API SQLiteDatabase
        {
        protected:

            static uint32 const constexpr s_defaultStatementBufferSize = 8096;

        public:

            virtual ~SQLiteDatabase();

            bool IsConnected() const { return m_pDatabase != nullptr; }
            inline bool HasErrorOccurred() const { return !m_errorMessage.empty(); }
            inline String const& GetError() const { return m_errorMessage; }

        protected:

            bool Connect( FileSystem::Path const& databasePath, bool readOnlyAccess = false, bool useMutex = false );
            bool Disconnect();

            bool IsValidSQLiteResult( int result, char const* pErrorMessage = nullptr ) const;

            void FillStatementBuffer( char const* pFormat, ... ) const;
            bool ExecuteSimpleQuery( char const* pFormat, ... ) const;

            bool BeginTransaction() const;
            bool EndTransaction() const;

        protected:

            sqlite3*                            m_pDatabase = nullptr;
            mutable String                      m_errorMessage;
            mutable char                        m_statementBuffer[s_defaultStatementBufferSize] = { 0 };
        };
    }
}

#endif