#pragma once

#include "../_Module/API.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class FileSystemPath;
    namespace TypeSystem { class TypeRegistry; }

    //-------------------------------------------------------------------------

    namespace EntityModel
    {
        class EntityCollectionDescriptor;

        //-------------------------------------------------------------------------

        class KRG_TOOLS_ENTITY_API EntityCollectionReader
        {

        public:

            static bool ReadCollection( TypeSystem::TypeRegistry const& typeRegistry, FileSystemPath const& filePath, EntityCollectionDescriptor& outCollection );
        };
    }
}