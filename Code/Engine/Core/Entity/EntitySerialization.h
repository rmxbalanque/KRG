#pragma once
#include "Engine/Core/_Module/API.h"
#include "System/Core/ThirdParty/KRG_RapidJson.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Entity;
    namespace FileSystem { class Path; }
    namespace TypeSystem { class TypeRegistry; }
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct EntityDescriptor;
    class EntityMap;
    class EntityCollectionDescriptor;
}

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::EntityModel::Serialization
{
    KRG_ENGINE_CORE_API bool ReadEntityDescriptor( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& entityValue, EntityDescriptor& outEntityDesc );
    KRG_ENGINE_CORE_API bool ReadEntityCollectionFromJson( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& entitiesArrayValue, EntityCollectionDescriptor& outCollectionDesc );
    KRG_ENGINE_CORE_API bool ReadEntityCollectionFromFile( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& filePath, EntityCollectionDescriptor& outCollectionDesc );

    //-------------------------------------------------------------------------

    KRG_ENGINE_CORE_API bool WriteEntityToJson( TypeSystem::TypeRegistry const& typeRegistry, EntityDescriptor const& entityDesc, RapidJsonWriter& writer );
    KRG_ENGINE_CORE_API bool WriteEntityToJson( TypeSystem::TypeRegistry const& typeRegistry, Entity const* pEntity, RapidJsonWriter& writer );
    KRG_ENGINE_CORE_API bool WriteEntityCollectionToJson( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& collection, RapidJsonWriter& writer );
    KRG_ENGINE_CORE_API bool WriteMapToJson( TypeSystem::TypeRegistry const& typeRegistry, EntityMap const& map, RapidJsonWriter& writer );
    KRG_ENGINE_CORE_API bool WriteEntityCollectionToFile( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& collection, FileSystem::Path const& outFilePath );
    KRG_ENGINE_CORE_API bool WriteMapToFile( TypeSystem::TypeRegistry const& typeRegistry, EntityMap const& map, FileSystem::Path const& outFilePath );
}
#endif