#include "PhysicsMaterialDatabaseCompiler.h"
#include "Engine/Physics/PhysicsMaterialDatabase.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        PhysicsMaterialDatabaseCompiler::PhysicsMaterialDatabaseCompiler()
            : Resource::Compiler( "PhysicsMaterialCompiler", VERSION )
        {
            m_outputTypes.push_back( PhysicsMaterialDatabase::GetStaticResourceTypeID() );
        }

        Resource::CompilationResult PhysicsMaterialDatabaseCompiler::Compile( Resource::CompileContext const& ctx ) const
        {
            PhysicsMaterialDatabaseResourceDescriptor resourceDescriptor;
            if ( !ctx.TryReadResourceDescriptor( resourceDescriptor ) )
            {
                return Error( "Failed to read resource descriptor from input file: %s", ctx.m_inputFilePath.c_str() );
            }

            // Read material libraries
            //-------------------------------------------------------------------------

            TVector<PhysicsMaterialSettings> materialSettings;

            for ( auto const& libraryDataPath : resourceDescriptor.m_materialLibraries )
            {
                FileSystem::Path libraryFilePath;
                if ( !ctx.ConvertDataPathToFilePath( libraryDataPath, libraryFilePath ) )
                {
                    return Error( "Failed to convert data path to filepath: %s", libraryDataPath.c_str() );
                }

                //-------------------------------------------------------------------------

                TypeSystem::Serialization::TypeReader typeReader( ctx.m_typeRegistry );
                if ( typeReader.ReadFromFile( libraryFilePath ) )
                {
                    int32 const numSerializedTypes = typeReader.GetNumSerializedTypes();
                    if ( numSerializedTypes == 0 )
                    {
                        KRG_LOG_ERROR( "ResourceCompiler", "Empty physics material library encountered: %s", libraryFilePath.c_str() );
                        return CompilationFailed( ctx );
                    }

                    for ( auto i = 0; i < numSerializedTypes; i++ )
                    {
                        PhysicsMaterialSettings material;
                        typeReader >> material;

                        // Validity check
                        if ( !material.IsValid() )
                        {
                            KRG_LOG_ERROR( "ResourceCompiler", "Invalid physics material encountered in library: %s", libraryFilePath.c_str() );
                            return CompilationFailed( ctx );
                        }

                        // Duplicate check
                        auto DuplicateCheck = [] ( PhysicsMaterialSettings const& materialSettings, StringID const& ID )
                        {
                            return materialSettings.m_ID == ID;
                        };

                        if ( eastl::find( materialSettings.begin(), materialSettings.end(), material.m_ID, DuplicateCheck ) != materialSettings.end() )
                        {
                            KRG_LOG_ERROR( "ResourceCompiler", "Duplicate physics material ID '%s' detected", material.m_ID.c_str() );
                            return CompilationFailed( ctx );
                        }

                        // Add valid material
                        materialSettings.emplace_back( material );
                    }
                }
                else
                {
                    KRG_LOG_ERROR( "ResourceCompiler", "Failed to read physics material library file: %s", libraryFilePath.c_str() );
                    return CompilationFailed( ctx );
                }
            }

            // Serialize
            //-------------------------------------------------------------------------

            FileSystem::EnsurePathExists( ctx.m_outputFilePath );
            Serialization::BinaryFileArchive archive( Serialization::Mode::Write, ctx.m_outputFilePath );
            if ( archive.IsValid() )
            {
                Resource::ResourceHeader hdr( VERSION, PhysicsMaterialDatabase::GetStaticResourceTypeID() );
                archive << hdr << materialSettings;
                return CompilationSucceeded( ctx );
            }
            else
            {
                return CompilationFailed( ctx );
            }
        }
    }
}
