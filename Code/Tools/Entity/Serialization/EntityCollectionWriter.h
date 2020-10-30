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

        class KRG_TOOLS_ENTITY_API EntityCollectionWriter
        {
        public:

            static bool WriteCollection( TypeSystem::TypeRegistry const& typeRegistry, FileSystemPath const& outFilePath, EntityCollectionDescriptor const& collection );
        };
    }
}