#pragma once

#include "System/Core/Types/String.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Reflection
{
    enum class EngineLayer : uint8
    {
        System = 0,
        Engine,
        Game,
        Tools,

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
        RegisterTypeResource,
        RegisterVirtualResource,
        RegisterEntityComponent,
        RegisterSingletonEntityComponent,
        RegisterEntitySystem,
        RegisterProperty,
        ExposeProperty,

        NumMacros,
        Unknown = NumMacros,
    };

    char const* GetReflectionMacroText( ReflectionMacro macro );

    //-------------------------------------------------------------------------

    namespace Settings
    {
        constexpr static char const* const g_engineNamespace = "KRG";
        constexpr static char const* const g_moduleHeaderFilePath = "_Module\\Module.h";
        constexpr static char const* const g_autogeneratedDirectory = "_Module\\_AutoGenerated\\";
        constexpr static char const* const g_autogeneratedModuleFile = "_module.cpp";
        constexpr static char const* const g_globalAutoGeneratedDirectory = "Code\\Applications\\Shared\\_AutoGenerated\\";
        constexpr static char const* const g_engineTypeRegistrationHeaderPath = "EngineTypeRegistration.h";
        constexpr static char const* const g_toolsTypeRegistrationHeaderPath = "ToolsTypeRegistration.h";
        constexpr static char const* const g_temporaryDirectoryPath = "\\..\\_Temp\\";

        //-------------------------------------------------------------------------
        // Engine Layer and Modules
        //-------------------------------------------------------------------------

        char const* const LayerPaths[(uint8) EngineLayer::NumLayers] =
        {
            "System\\",
            "Engine\\",
            "Game\\",
            "Tools\\",
        };

        char const* const LayerProjectNamePrefixes[(uint8) EngineLayer::NumLayers] =
        {
            "KRG.System.",
            "KRG.Engine.",
            "KRG.Game.",
            "KRG.Tools.",
        };

        char const* const ModuleNames[(uint8) EngineLayer::NumLayers] =
        {
            "SystemModule",
            "EngineModule",
            "GameModule",
            "ToolsModule",
        };

        //-------------------------------------------------------------------------
        // Module exclusions
        //-------------------------------------------------------------------------

        constexpr static char const* const g_engineLayersToExclude[] = { "KRG.Applications." };
        constexpr static char const* const g_moduleNameExclusionFilters[] = { "ThirdParty" };

        //-------------------------------------------------------------------------
        // Core class names
        //-------------------------------------------------------------------------

        constexpr static char const* const g_registeredTypeInterfaceClassName = "IRegisteredType";
        constexpr static char const* const g_baseEntityClassName = "Entity";
        constexpr static char const* const g_baseEntityComponentClassName = "EntityComponent";
        constexpr static char const* const g_baseEntitySystemClassName = "IEntitySystem";
        constexpr static char const* const g_baseResourceClassName = "Resource::IResource";

        //-------------------------------------------------------------------------
        // Clang Parser Settings
        //-------------------------------------------------------------------------

        char const* const g_includePaths[] =
        {
            "Code\\",
            "Code\\System\\Core\\ThirdParty\\EA\\EABase\\include\\common\\",
            "Code\\System\\Core\\ThirdParty\\EA\\EASTL\\include\\",
            "Code\\System\\Core\\ThirdParty\\",
            "Code\\System\\Render\\ThirdParty\\imgui\\",
            "External\\PhysX\\pxshared\\include\\",
            "External\\PhysX\\physx\\include\\",
            #if KRG_ENABLE_NAVPOWER
            "External\\NavPower\\include\\"
            #endif
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

        //-------------------------------------------------------------------------

        inline EngineLayer GetLayerForFilePath( FileSystem::Path const& filePath )
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

        inline bool IsFileInSystemLayer( FileSystem::Path const& filePath ) { return filePath.GetString().find( Settings::LayerPaths[(uint8) EngineLayer::System] ) != String::npos; }
        inline bool IsFileInEngineLayer( FileSystem::Path const& filePath ) { return filePath.GetString().find( Settings::LayerPaths[(uint8) EngineLayer::Engine] ) != String::npos; }
        inline bool IsFileInGameLayer( FileSystem::Path const& filePath ) { return filePath.GetString().find( Settings::LayerPaths[(uint8) EngineLayer::Game] ) != String::npos; }
        inline bool IsFileInToolsLayer( FileSystem::Path const& filePath ) { return filePath.GetString().find( Settings::LayerPaths[(uint8) EngineLayer::Tools] ) != String::npos; }
    }
}