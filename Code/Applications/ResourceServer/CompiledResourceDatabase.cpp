#include "CompiledResourceDatabase.h"
#include "System/Core/FileSystem/FileSystem.h"
#include <sqlite3.h>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        bool CompiledResourceDatabase::TryConnect( FileSystem::Path const& databasePath )
        {
            KRG_ASSERT( databasePath.IsFilePath() );

            if ( !Connect( databasePath, false ) )
            {
                return false;
            }

            if ( !CreateTables() )
            {
                return false;
            }

            return true;
        }

        bool CompiledResourceDatabase::CleanDatabase( FileSystem::Path const& databasePath )
        {
            KRG_ASSERT( databasePath.IsFilePath() );

            if ( !Connect( databasePath, false ) )
            {
                return false;
            }

            if ( !DropTables() )
            {
                return false;
            }

            return Disconnect();
        }

        //-------------------------------------------------------------------------

        bool CompiledResourceDatabase::CreateTables()
        {
            KRG_ASSERT( m_pDatabase != nullptr );

            if ( !ExecuteSimpleQuery( "CREATE TABLE IF NOT EXISTS `CompiledResources` ( `ResourcePath` TEXT UNIQUE,`ResourceType` INTEGER,`CompilerVersion` INTEGER,`FileTimestamp` INTEGER, `SourceTimestampHash` INTEGER, PRIMARY KEY( ResourcePath, ResourceType ) );" ) )
            {
                return false;
            }

            return true;
        }

        bool CompiledResourceDatabase::DropTables()
        {
            KRG_ASSERT( m_pDatabase != nullptr );
            char* pErrorMessage = nullptr;

            if ( !ExecuteSimpleQuery( "DROP TABLE IF EXISTS `CompiledResources`;" ) )
            {
                return false;
            }

            return true;
        }

        //-------------------------------------------------------------------------

        CompiledResourceRecord CompiledResourceDatabase::GetRecord( ResourceID resourceID ) const
        {
            CompiledResourceRecord record;

            //-------------------------------------------------------------------------

            sqlite3_stmt* pStatement = nullptr;
            FillStatementBuffer( "SELECT * FROM `CompiledResources` WHERE `ResourcePath` = \"%s\" AND `ResourceType` = %d;", resourceID.GetPath().c_str(), (uint32) resourceID.GetResourceTypeID() );
            if ( IsValidSQLiteResult( sqlite3_prepare_v2( m_pDatabase, m_statementBuffer, -1, &pStatement, nullptr ) ) )
            {
                while ( sqlite3_step( pStatement ) == SQLITE_ROW )
                {
                    String resourcePath = ( char const*) sqlite3_column_text( pStatement, 0 );
                    int32 resourceType = sqlite3_column_int( pStatement, 1 );
                    record.m_resourceID = ResourceID( resourcePath );

                    record.m_compilerVersion = sqlite3_column_int( pStatement, 2 );
                    record.m_fileTimestamp = sqlite3_column_int64( pStatement, 3 );
                    record.m_sourceTimestampHash = sqlite3_column_int64( pStatement, 4 );
                }

                IsValidSQLiteResult( sqlite3_finalize( pStatement ) );
            }

            return record;
        }

        bool CompiledResourceDatabase::WriteRecord( CompiledResourceRecord const& record )
        {
            return ExecuteSimpleQuery( "INSERT OR REPLACE INTO `CompiledResources` ( `ResourcePath`, `ResourceType`, `CompilerVersion`, `FileTimestamp`, `SourceTimestampHash` ) VALUES ( \"%s\", %d, %d, %llu, %llu );", record.m_resourceID.GetPath().c_str(), (uint32) record.m_resourceID.GetResourceTypeID(), record.m_compilerVersion, record.m_fileTimestamp, record.m_sourceTimestampHash  );
        }
    }
}