#pragma once

#include "System/Core/Types/String.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            enum class EngineLayer : uint8
            {
                System = 0,
                Engine,
                Game,
                Tools,
                Resource,

                NumLayers,
                Unknown
            };

            //-------------------------------------------------------------------------

            enum class ReflectionMacro
            {
                IgnoreHeader,
                RegisterModule,
                RegisterEnum,
                RegisterType,
                RegisterResource,
                RegisterVirtualResource,
                RegisterEntityComponent,
                RegisterEntitySystem,
                ExposeProperty,

                NumMacros,
                Unknown = NumMacros,
            };

            char const* GetReflectionMacroText( ReflectionMacro macro );

            //-------------------------------------------------------------------------

            namespace Settings
            {
                static char const* const g_moduleHeaderFilePath = "_Module\\Module.h";
                static char const* const g_autogeneratedDirectory = "_Module\\_AutoGenerated\\";
                static char const* const g_autogeneratedModuleFile = "_module.cpp";
                static char const* const g_globalAutoGeneratedDirectory = "Code\\Applications\\Shared\\_AutoGenerated\\";
                static char const* const g_engineTypeRegistrationHeaderPath = "EngineTypeRegistration.h";
                static char const* const g_toolsTypeRegistrationHeaderPath = "ToolsTypeRegistration.h";
                static char const* const g_temporaryFolderPath = "\\..\\_Temp\\";

                //-------------------------------------------------------------------------
                // Engine Layer and Modules
                //-------------------------------------------------------------------------

                static char const* const g_engineNamespace = "KRG";

                char const* const LayerPaths[(uint8) EngineLayer::NumLayers] =
                {
                    "System\\",
                    "Engine\\",
                    "Game\\",
                    "Tools\\",
                    "Compilers\\",
                };

                char const* const LayerProjectNamePrefixes[(uint8) EngineLayer::NumLayers] =
                {
                    "KRG.System.",
                    "KRG.Engine.",
                    "KRG.Game.",
                    "KRG.Tools.",
                    "KRG.ResourceCompilers.",
                };

                char const* const ModuleNames[(uint8) EngineLayer::NumLayers] =
                {
                    "SystemModule",
                    "EngineModule",
                    "GameModule",
                    "ToolsModule",
                    "ResourceCompilerModule",
                };

                //-------------------------------------------------------------------------
                // Module exclusions
                //-------------------------------------------------------------------------

                static char const* const g_engineLayersToExclude[] = { "KRG.Applications." };
                static char const* const g_moduleNameExclusionFilters[] = { "ThirdParty" };

                //-------------------------------------------------------------------------
                // Core class names
                //-------------------------------------------------------------------------

                static char const* const g_baseEntityClassName = "Entity";
                static char const* const g_baseEntityComponentClassName = "EntityComponent";
                static char const* const g_baseEntitySystemClassName = "IEntitySystem";
                static char const* const g_baseResourceClassName = "Resource::IResource";

                //-------------------------------------------------------------------------
                // Clang Parser Settings
                //-------------------------------------------------------------------------

                char const* const g_includePaths[] =
                {
                    "Code\\",
                    "Code\\System\\Core\\ThirdParty\\EA\\EABase\\include\\common\\",
                    "Code\\System\\Core\\ThirdParty\\EA\\EASTL\\include\\",
                    "Code\\System\\Core\\ThirdParty\\",
                    "External\\PhysX\\pxshared\\include\\",
                    "External\\PhysX\\physx\\include\\",
                    "External\\NavPower\\include\\"
                };
            }

            //-------------------------------------------------------------------------

            namespace Utils
            {
                inline EngineLayer GetLayerForModule( String const& moduleName )
                {
                    for ( auto i = 0u; i < (uint8) EngineLayer::NumLayers; i++ )
                    {
                        if ( moduleName == Settings::ModuleNames[i] )
                        {
                            return (EngineLayer) i;
                        }
                    }

                    return EngineLayer::Unknown;
                }

                inline bool IsValidModuleName( String const& moduleName )
                {
                    return GetLayerForModule( moduleName ) != EngineLayer::Unknown;
                }

                inline String GetModuleNameErrorMessage()
                {
                    String message = "Only [ ";

                    for ( auto i = 0u; i < (uint8) EngineLayer::NumLayers; i++ )
                    {
                        bool const isLastName = i == (uint8) EngineLayer::NumLayers - 1;
                        message += "\"";
                        message += Settings::ModuleNames[i];
                        message += isLastName ? "\" " : "\", ";
                    }

                    message += "] module names allowed";
                    return message;
                }

                inline bool IsSystemModule( String const& moduleName ) { return moduleName.find( Settings::ModuleNames[(uint8) EngineLayer::System] ) != String::npos; }
                inline bool IsEngineModule( String const& moduleName ) { return moduleName.find( Settings::ModuleNames[(uint8) EngineLayer::Engine] ) != String::npos; }
                inline bool IsGameModule( String const& moduleName ) { return moduleName.find( Settings::ModuleNames[(uint8) EngineLayer::Game] ) != String::npos; }
                inline bool IsToolsModule( String const& moduleName ) { return moduleName.find( Settings::ModuleNames[(uint8) EngineLayer::Tools] ) != String::npos; }
                inline bool IsResourceCompilerModule( String const& moduleName ) { return moduleName.find( Settings::ModuleNames[(uint8) EngineLayer::Resource] ) != String::npos; }

                //-------------------------------------------------------------------------

                inline EngineLayer GetLayerForFilePath( FileSystemPath const& filePath )
                {
                    String const& pathString = filePath.GetString();

                    for ( auto i = 0u; i < (uint8) EngineLayer::NumLayers; i++ )
                    {
                        if ( pathString.find( Settings::LayerPaths[i] ) != String::npos )
                        {
                            return (EngineLayer) i;
                        }
                    }

                    return EngineLayer::Unknown;
                }

                inline bool IsFileInSystemLayer( FileSystemPath const& filePath ) { return filePath.GetString().find( Settings::LayerPaths[(uint8) EngineLayer::System] ) != String::npos; }
                inline bool IsFileInEngineLayer( FileSystemPath const& filePath ) { return filePath.GetString().find( Settings::LayerPaths[(uint8) EngineLayer::Engine] ) != String::npos; }
                inline bool IsFileInGameLayer( FileSystemPath const& filePath ) { return filePath.GetString().find( Settings::LayerPaths[(uint8) EngineLayer::Game] ) != String::npos; }
                inline bool IsFileInToolsLayer( FileSystemPath const& filePath ) { return filePath.GetString().find( Settings::LayerPaths[(uint8) EngineLayer::Tools] ) != String::npos; }
                inline bool IsFileInResourceCompilerLayer( FileSystemPath const& filePath ) { return filePath.GetString().find( Settings::LayerPaths[(uint8) EngineLayer::Resource] ) != String::npos; }
            }
        }
    }
}