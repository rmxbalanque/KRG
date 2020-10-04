#pragma once

#include "_Module/API.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Types/UUID.h"
#include "System/Core/Types/Time.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem { class TypeRegistry; struct TypeInfo; }
    class FileSystemPath;

    //-------------------------------------------------------------------------

    class ToolEntityCollection;
    class ToolEntity;
    class ToolEntityComponent;

    //-------------------------------------------------------------------------

    namespace Serialization
    {
        struct RawEntityData;
        struct RawComponentData;

        //-------------------------------------------------------------------------

        class KRG_TOOLS_ENTITY_API ToolEntityCollectionReader
        {

        public:

            enum class Stage
            {
                None,
                LoadFile,
                ParseJSON,
                ReadRawData,
                CreateCollection,
                Complete,
            };

            struct Progress
            {
                Stage           m_stage = Stage::None;
                Milliseconds    m_loadTime = -1.0f;
                Milliseconds    m_parseTime = -1.0f;
                Milliseconds    m_readDataTime = -1.0f;
                Milliseconds    m_createCollectionTime = -1.0f;
            };

        public:

            ToolEntityCollectionReader( TypeSystem::TypeRegistry const& typeRegistry );

            bool ReadCollection( FileSystemPath const& filePath, ToolEntityCollection& outCollection ) const;

            // Progress report
            //-------------------------------------------------------------------------

            template<typename T>
            void SetProgressNotifier( T&& t )
            {
                m_progressUpdateNotifier = eastl::function<void( Progress const& progress )>( t );
            }

            inline void SetProgressNotifier( eastl::function<void( Progress const& progress )> notifier ) { m_progressUpdateNotifier = notifier; }
            inline void ClearProgressNotifier() { m_progressUpdateNotifier = nullptr; }

        private:

            bool SetToolEntityComponentData( RawComponentData const* pRawComponent, ToolEntityComponent& toolComponent ) const;
            bool SetToolEntityData( RawEntityData const* pRawEntity, ToolEntity& toolEntity ) const;
            bool CreateCollection( TVector<RawEntityData*>& rawEntities, THashMap<UUID, RawEntityData*>& rawEntityMap, ToolEntityCollection& outCollection ) const;

        private:

            TypeSystem::TypeRegistry const&                                             m_typeRegistry;
            eastl::function<void( Progress const& progress )>                           m_progressUpdateNotifier;
        };
    }
}