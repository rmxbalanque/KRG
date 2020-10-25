#pragma once

#include "../_Module/API.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Types/UUID.h"
#include "System/Core/Types/Time.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class FileSystemPath;
    namespace TypeSystem { class TypeRegistry; struct TypeInfo; }

    //-------------------------------------------------------------------------

    namespace EntityModel
    {
        class EntityCollectionDescriptor;

        //-------------------------------------------------------------------------

        class KRG_TOOLS_ENTITY_API EntityCollectionReader
        {

        public:

            bool ReadCollection( TypeSystem::TypeRegistry const& typeRegistry, FileSystemPath const& filePath, EntityCollectionDescriptor& outCollection ) const;
        };
    }
}