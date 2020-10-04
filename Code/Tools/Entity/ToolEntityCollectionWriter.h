#pragma once

#include "_Module/API.h"
#include "System/Core/Types/Containers.h"
#include "Tools/Core/Thirdparty/KRG_RapidJson.h"
#include "System/Core/Types/Time.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        class TypeRegistry; 
        struct TypeInfo; 
        class PropertyPath;
    }

    //-------------------------------------------------------------------------

    class FileSystemPath;
    class ToolEntityCollection;
    class ToolEntity;
    class ToolEntityComponent;
    class ToolEntitySystem;
    class DynamicPropertyInstance;
    class DynamicTypeInstance;

    //-------------------------------------------------------------------------

    namespace Serialization
    {
        class KRG_TOOLS_ENTITY_API ToolEntityCollectionWriter
        {
            enum class Stage
            {
                None,
                GenerateJSON,
                WriteFile,
                Complete,
            };

            struct Progress
            {
                Stage           m_stage = Stage::None;
                Milliseconds    m_GenerateJSONTime = -1.0f;
                Milliseconds    m_writeTime = -1.0f;
            };

        public:

            ToolEntityCollectionWriter( TypeSystem::TypeRegistry const& typeRegistry );

            bool WriteCollection( ToolEntityCollection const& collection, FileSystemPath const& outFilePath ) const;

            template<typename T>
            void SetProgressNotifier( T&& t )
            {
                m_progressUpdateNotifier = eastl::function<void( Progress const& progress )>( t );
            }

            inline void SetProgressNotifier( eastl::function<void( Progress const& progress )> notifier ) { m_progressUpdateNotifier = notifier; }
            inline void ClearProgressNotifier() { m_progressUpdateNotifier = nullptr; }

        private:

            bool WritePropertyInstance( rapidjson::PrettyWriter<RapidJsonStringBuffer>& writer, TypeSystem::PropertyPath& currentPath, DynamicPropertyInstance const& propertyInstance ) const;
            bool WriteTypeInstance( rapidjson::PrettyWriter<RapidJsonStringBuffer>& writer, TypeSystem::PropertyPath& currentPath, DynamicTypeInstance const& typeInstance ) const;

            bool WriteComponent( rapidjson::PrettyWriter<RapidJsonStringBuffer>& writer, ToolEntityComponent const& component ) const;
            bool WriteSystem( rapidjson::PrettyWriter<RapidJsonStringBuffer>& writer, ToolEntitySystem const& system ) const;
            bool WriteEntity( rapidjson::PrettyWriter<RapidJsonStringBuffer>& writer, ToolEntity const& entity ) const;

        private:

            TypeSystem::TypeRegistry const&                                             m_typeRegistry;
            eastl::function<void( Progress const& progress )>                           m_progressUpdateNotifier;
        };
    }
}