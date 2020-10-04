#include "CodeGenerator_CPP.h"
#include "CodeGenerator_CPP_Type.h"
#include "CodeGenerator_CPP_Enum.h"
#include "Applications/Reflector/ReflectorSettingsAndUtils.h"
#include "System/TypeSystem/TypeID.h"
#include "System/Core/FileSystem/FileSystem.h"
#include "System/Core/Algorithm/TopologicalSort.h"

#include <fstream>
#include <string>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace CPP
    {
        static bool SortTypesByDependencies( TVector<TypeDescriptor>& structureTypes )
        {
            S32 const numTypes = (S32) structureTypes.size();
            if ( numTypes <= 1 )
            {
                return true;
            }

            // Create list to sort
            TVector<TopologicalSorter::Node > list;
            for ( auto i = 0; i < numTypes; i++ )
            {
                list.push_back( TopologicalSorter::Node( i ) );
            }

            for ( auto i = 0; i < numTypes; i++ )
            {
                S32 const numParents = (S32) structureTypes[i].m_parents.size();
                for ( auto p = 0; p < numParents; p++ )
                {
                    for ( auto j = 0; j < numTypes; j++ )
                    {
                        if ( i != j && structureTypes[j].m_ID == structureTypes[i].m_parents[p] )
                        {
                            list[i].m_children.push_back( &list[j] );
                        }
                    }
                }
            }

            // Try to sort
            if ( !TopologicalSorter::Sort( list ) )
            {
                return false;
            }

            // Update type list
            TVector<TypeDescriptor> sortedTypes;
            sortedTypes.reserve( numTypes );

            for ( auto& node : list )
            {
                sortedTypes.push_back( structureTypes[node.m_ID] );
            }
            structureTypes.swap( sortedTypes );

            return true;
        }

        static bool GenerateTypeRegistrationHeaderFiles( ReflectionDatabase const& database, SolutionDesc const& solution, std::stringstream& typeRegistrationStr, bool generateEngineTypeRegistration )
        {
            typeRegistrationStr.str( std::string() );
            typeRegistrationStr.clear();
            typeRegistrationStr << "//-------------------------------------------------------------------------\n";
            typeRegistrationStr << "// This is an auto-generated file - DO NOT edit\n";
            typeRegistrationStr << "//-------------------------------------------------------------------------\n\n";
            typeRegistrationStr << "#include \"System/Entity/EntityTypeHelpers.h\"\n";
            typeRegistrationStr << "#include \"System/TypeSystem/TypeRegistry.h\"\n\n";

            // Get all modules from database
            TVector<ProjectDesc> modules = database.GetAllRegisteredProjects();
            TVector<ResourceDesc> const& registeredResourceTypes = database.GetAllRegisteredResourceTypes();

            // Module includes
            //-------------------------------------------------------------------------

            for ( auto i = 0; i < modules.size(); i++ )
            {
                // Ignore tools modules
                if ( generateEngineTypeRegistration )
                {
                    if ( Reflection::Utils::IsResourceCompilerModule( modules[i].m_moduleClassName ) || Reflection::Utils::IsToolsModule( modules[i].m_moduleClassName ) )
                    {
                        VectorEraseUnsorted( modules, i );
                        i--;
                        continue;
                    }
                }

                // Ignore module-less modules
                if ( !modules[i].m_moduleHeaderID.IsValid() )
                {
                    VectorEraseUnsorted( modules, i );
                    i--;
                    continue;
                }


                // Output module include
                HeaderDesc const* pHeader = database.GetHeaderDesc( modules[i].m_moduleHeaderID );
                if ( pHeader != nullptr )
                {
                    typeRegistrationStr << "#include \"" << pHeader->m_filePath << "\"\n";
                }
                else
                {
                    return false;
                }
            }
            typeRegistrationStr << "\n";

            // Sort modules according to original project dependency order
            //-------------------------------------------------------------------------

            auto sortPredicate = [] ( ProjectDesc const& pA, ProjectDesc const& pB )
            {
                return pA.m_dependencyCount < pB.m_dependencyCount;
            };

            eastl::sort( modules.begin(), modules.end(), sortPredicate );

            typeRegistrationStr << "namespace KRG\n";
            typeRegistrationStr << "{\n";
            typeRegistrationStr << "    namespace AutoGenerated\n";
            typeRegistrationStr << "    {\n";

            // Registration function
            //-------------------------------------------------------------------------

            typeRegistrationStr << "        inline void RegisterTypes( TypeSystem::TypeRegistry& typeRegistry )\n";
            typeRegistrationStr << "        {\n";

            typeRegistrationStr << "            TypeSystem::RegisterCoreEntityTypes( typeRegistry );\n\n";

            for ( auto& module : modules )
            {
                typeRegistrationStr << "            " << module.m_moduleClassName << "::RegisterTypes( typeRegistry );\n";
            }

            typeRegistrationStr << "\n";

            for ( auto& registeredResourceType : registeredResourceTypes )
            {
                typeRegistrationStr << "            typeRegistry.RegisterResourceTypeID( TypeSystem::TypeID( \"" << registeredResourceType.m_typeID.GetAsStringID().ToString() << "\"), ResourceTypeID( \"" << registeredResourceType.m_resourceTypeID.ToString() << "\" ) );\n";
            }

            typeRegistrationStr << "        }\n\n";

            // Unregistration function
            //-------------------------------------------------------------------------

            typeRegistrationStr << "        inline void UnregisterTypes( TypeSystem::TypeRegistry& typeRegistry )\n";
            typeRegistrationStr << "        {\n";

            for ( auto iter = modules.rbegin(); iter != modules.rend(); ++iter )
            {
                typeRegistrationStr << "            " << iter->m_moduleClassName << "::UnregisterTypes( typeRegistry );\n";
            }

            typeRegistrationStr << "\n";

            for ( auto iter = registeredResourceTypes.rbegin(); iter != registeredResourceTypes.rend(); ++iter )
            {
                typeRegistrationStr << "            typeRegistry.UnregisterResourceTypeID( TypeSystem::TypeID( \"" << iter->m_typeID.GetAsStringID().ToString() << "\" ) );\n";
            }

            typeRegistrationStr << "\n            TypeSystem::UnregisterCoreEntityTypes( typeRegistry );\n";

            typeRegistrationStr << "        }\n";
            typeRegistrationStr << "    }\n";
            typeRegistrationStr << "}\n";

            return true;
        }

        //-------------------------------------------------------------------------

        bool Generator::SaveStreamToFile( FileSystemPath const& filePath, std::stringstream& stream )
        {
            bool fileContentsEqual = true;

            // Rewind stream to beginning
            stream.seekg( std::ios::beg );

            // Open existing file and compare contents to the newly generated stream
            std::fstream existingFile( filePath.c_str(), std::ios::in );
            if ( existingFile.is_open() )
            {
                std::string lineNew, lineExisting;
                while ( getline( existingFile, lineExisting ) && fileContentsEqual )
                {
                    if ( !getline( stream, lineNew ) || ( lineExisting != lineNew ) )
                    {
                        fileContentsEqual = false;
                    }
                }

                // Set different if the stream is longer than the file
                if ( fileContentsEqual && getline( stream, lineNew ) )
                {
                    fileContentsEqual = false;
                }

                existingFile.close();
            }
            else
            {
                fileContentsEqual = false;
            }

            // If the contents differ overwrite the existing file
            if ( !fileContentsEqual )
            {
                existingFile.open( filePath.c_str(), std::ios::out | std::ios::trunc );
                if ( !existingFile.is_open() )
                {
                    return false;
                }

                stream.seekg( std::ios::beg );
                existingFile << stream.rdbuf();
                existingFile.close();
            }

            return true;
        }

        void Generator::GenerateHeaderFileHeader( HeaderDesc const& hdr, String const& headerID )
        {
            m_headerFile.str( std::string() );
            m_headerFile.clear();
            m_headerFile << "#pragma once\n\n";
            m_headerFile << "//-------------------------------------------------------------------------\n";
            m_headerFile << "// This is an auto-generated file - DO NOT edit\n";
            m_headerFile << "//-------------------------------------------------------------------------\n\n";
            m_headerFile << "// HeaderID: " << headerID << "\n\n";
            m_headerFile << "#include \"../API.h\"\n";
            m_headerFile << "#include \"System/TypeSystem/TypeRegistry.h\"\n";
            m_headerFile << "#include \"System/TypeSystem/ITypeHelper.h\"\n";
            m_headerFile << "#include \"System/Core/Serialization/Serialization.h\"\n";
            m_headerFile << "#include \"" << hdr.m_filePath << "\"\n";
        }

        void Generator::GenerateCodeFileHeader( String const& generatedHeaderPath )
        {
            m_cppFile.str( std::string() );
            m_cppFile.clear();
            m_cppFile << "//-------------------------------------------------------------------------\n";
            m_cppFile << "// This is an auto-generated file - DO NOT edit\n";
            m_cppFile << "//-------------------------------------------------------------------------\n\n";
            m_cppFile << "#include \"" << generatedHeaderPath << "\"\n";
        }

        void Generator::GenerateModuleCodeFile( ProjectDesc const& prj, TVector<TypeDescriptor> const& typesInModule )
        {
            //-------------------------------------------------------------------------
            // Header

            m_moduleFile.str( std::string() );
            m_moduleFile.clear();
            m_moduleFile << "//-------------------------------------------------------------------------\n";
            m_moduleFile << "// This is an auto-generated file - DO NOT edit\n";
            m_moduleFile << "//-------------------------------------------------------------------------\n\n";
            m_moduleFile << "#include \"" << prj.GetModuleHeaderDesc().m_filePath << "\"\n";
            m_moduleFile << "#include \"System/Resource/ResourceSystem.h\"\n";

            //-------------------------------------------------------------------------
            // Includes

            TVector<FileSystemPath> autoGeneratedFiles;
            String const autoGeneratedDirectory = prj.m_path + Reflection::Settings::g_autogeneratedDirectory;
            FileSystem::GetDirectoryContents( autoGeneratedDirectory, autoGeneratedFiles, "*.cpp" );

            for ( auto& file : autoGeneratedFiles )
            {
                if ( !file.IsFilenameEqual( Reflection::Settings::g_autogeneratedModuleFile ) )
                {
                    m_moduleFile << "#include \"" << file << "\"\n";
                }
            }

            //-------------------------------------------------------------------------
            // Registration functions

            m_moduleFile << "\n//-------------------------------------------------------------------------\n\n";
            m_moduleFile << "void " << prj.m_moduleClassName << "::RegisterTypes( TypeSystem::TypeRegistry& typeRegistry )\n{\n";

            for ( auto& type : typesInModule )
            {
                if ( type.IsEnum() )
                {
                    EnumGenerator::GenerateRegistrationFunctionCall( m_moduleFile, type );
                }
                else
                {
                    TypeGenerator::GenerateRegistrationFunctionCall( m_moduleFile, type );
                }
            }

            m_moduleFile << "}\n\n";
            m_moduleFile << "void " << prj.m_moduleClassName << "::UnregisterTypes( TypeSystem::TypeRegistry& typeRegistry )\n{\n";

            for ( auto iter = typesInModule.rbegin(); iter != typesInModule.rend(); ++iter )
            {
                if ( iter->IsEnum() )
                {
                    EnumGenerator::GenerateUnregistrationFunctionCall( m_moduleFile, *iter );
                }
                else
                {
                    TypeGenerator::GenerateUnregistrationFunctionCall( m_moduleFile, *iter );
                }
            }

            m_moduleFile << "}\n\n";
        }

        //-------------------------------------------------------------------------
        // Generator
        //-------------------------------------------------------------------------

        bool Generator::LogError( char const* pErrorFormat, ... ) const
        {
            char buffer[256];

            va_list args;
            va_start( args, pErrorFormat );
            VPrintf( buffer, 256, pErrorFormat, args );
            va_end( args );

            m_errorMessage = String( buffer );
            return false;
        }

        bool Generator::Generate( ReflectionDatabase const& database, SolutionDesc const& solution )
        {
            char headerIDStringBuffer[32];

            m_pDatabase = &database;
            for ( auto& prj : solution.m_projects )
            {
                // Ignore module less projects
                if ( !prj.m_moduleHeaderID.IsValid() )
                {
                    continue;
                }

                // Ensure the auto generated directory exists
                FileSystemPath const autoGeneratedDirectory = ( prj.m_path + Reflection::Settings::g_autogeneratedDirectory );
                FileSystem::EnsurePathExists( autoGeneratedDirectory );

                // Generate list of all expected header files in the auto generated directory
                TVector<FileSystemPath> expectedFiles;
                expectedFiles.push_back( FileSystemPath( autoGeneratedDirectory + Reflection::Settings::g_autogeneratedModuleFile ) );

                for ( auto const& hdrDesc : prj.m_headerFiles )
                {
                    Printf( headerIDStringBuffer, 32, "%u", (U32) hdrDesc.m_ID );
                    expectedFiles.push_back( FileSystemPath( autoGeneratedDirectory + headerIDStringBuffer + ".h" ) );
                    expectedFiles.push_back( FileSystemPath( autoGeneratedDirectory + headerIDStringBuffer + ".cpp" ) );
                }

                // Delete any unknown files from the auto generated directory
                TVector<FileSystemPath> files;
                FileSystem::GetDirectoryContents( autoGeneratedDirectory, files );
                for ( auto const& file : files )
                {
                    if ( VectorFind( expectedFiles, file ) == expectedFiles.end() )
                    {
                        FileSystem::EraseFile( file );
                    }
                }

                // Generate code files for the dirty headers
                bool result = true;
                for ( auto& dirtyHeaderIdx : prj.m_dirtyHeaders )
                {
                    auto& hdr = prj.m_headerFiles[dirtyHeaderIdx];

                    if ( hdr.m_ID == prj.m_moduleHeaderID )
                    {
                        continue;
                    }

                    // Convert header ID to string and generate file names
                    Printf( headerIDStringBuffer, 32, "%u", (U32) hdr.m_ID );
                    String const filename_h = autoGeneratedDirectory + headerIDStringBuffer + ".h";
                    String const filename_cpp = autoGeneratedDirectory + headerIDStringBuffer + ".cpp";

                    // Generate files
                    GenerateHeaderFileHeader( hdr, headerIDStringBuffer );
                    GenerateCodeFileHeader( filename_h );

                    // Get all types for the header
                    TVector<TypeDescriptor> typesInHeader;
                    m_pDatabase->GetAllTypesForHeader( hdr.m_ID, typesInHeader );

                    TVector<TypeDescriptor> parentDescs;
                    for ( auto& type : typesInHeader )
                    {
                        // Generate enum info
                        if ( type.IsEnum() )
                        {
                            EnumGenerator::Generate( m_headerFile, m_cppFile, prj.m_exportMacro, type );
                        }
                        else // Generate type info
                        {
                            parentDescs.clear();
                            for ( auto const& parentID : type.m_parents )
                            {
                                auto pTypeDesc = m_pDatabase->GetType( parentID );
                                if ( pTypeDesc == nullptr )
                                {
                                    String const fullTypeName = type.m_namespace + type.m_name;
                                    return LogError( "Unknown base class: %s encountered for type: %s", parentID.GetAsStringID().ToString(), fullTypeName.c_str() );
                                }
                                parentDescs.push_back( *pTypeDesc );
                            }

                            TypeGenerator::Generate( database, m_headerFile, m_cppFile, prj.m_exportMacro, type, parentDescs );
                        }
                    }

                    // Save generated files
                    if ( !SaveStreamToFile( filename_h, m_headerFile ) )
                    {
                        return LogError( "Could not save header file to disk: %s", filename_h.c_str() );
                    }

                    if ( !SaveStreamToFile( filename_cpp, m_cppFile ) )
                    {
                        return LogError( "Could not save cpp file to disk: %s", filename_cpp.c_str() );
                    }
                }

                // Get project info from database as that will contain all necessary info like module class name
                ProjectDesc const* pProjectDesc = m_pDatabase->GetProjectDesc( prj.m_ID );
                if ( pProjectDesc == nullptr )
                {
                    return LogError( "Could not retrieve description for project: %s", prj.m_name.c_str() );
                }
                KRG_ASSERT( prj.m_ID == pProjectDesc->m_ID );

                // Get all types in project
                TVector<TypeDescriptor> typesInProject;
                m_pDatabase->GetAllTypesForProject( pProjectDesc->m_ID, typesInProject );
                if ( !SortTypesByDependencies( typesInProject ) )
                {
                    return LogError( "Cyclic header dependency detected in project: %s", pProjectDesc->m_name.c_str() );
                }

                // Generate and save the module file
                GenerateModuleCodeFile( *pProjectDesc, typesInProject );
                String const module_cpp = autoGeneratedDirectory + Reflection::Settings::g_autogeneratedModuleFile;
                if ( !SaveStreamToFile( module_cpp, m_moduleFile ) )
                {
                    return LogError( "Could not save module file to disk: %s", module_cpp.c_str() );
                }
            }

            // Generate and save module type registration header
            //-------------------------------------------------------------------------

            if ( !GenerateTypeRegistrationHeaderFiles( database, solution, m_engineTypeRegistrationFile, true ) )
            {
                return LogError( "Could not generate engine type registration header!" );
            }

            if ( !GenerateTypeRegistrationHeaderFiles( database, solution, m_toolsTypeRegistrationFile, false ) )
            {
                return LogError( "Could not generate tools type registration header!" );
            }

            //-------------------------------------------------------------------------

            FileSystemPath const autoGeneratedPath = solution.m_path + Reflection::Settings::g_globalAutoGeneratedDirectory;
            FileSystemPath const engineTypeRegistration_h = autoGeneratedPath + Reflection::Settings::g_engineTypeRegistrationHeaderPath;
            FileSystemPath const toolsTypeRegistration_h = autoGeneratedPath + Reflection::Settings::g_toolsTypeRegistrationHeaderPath;

            FileSystem::EnsurePathExists( autoGeneratedPath );
            if ( !SaveStreamToFile( engineTypeRegistration_h, m_engineTypeRegistrationFile ) )
            {
                return LogError( "Could not save type registration header to disk: %s", engineTypeRegistration_h.c_str() );
            }

            FileSystem::EnsurePathExists( autoGeneratedPath );
            if ( !SaveStreamToFile( toolsTypeRegistration_h, m_toolsTypeRegistrationFile ) )
            {
                return LogError( "Could not save type registration header to disk: %s", toolsTypeRegistration_h.c_str() );
            }

            return true;
        }
    }
}