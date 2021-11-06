#pragma once

#include "../_Module/API.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace FileSystem { class Path; }
    namespace TypeSystem { class TypeRegistry; }

    //-------------------------------------------------------------------------

    namespace EntityModel
    {
        class EntityCollection;
        class EntityCollectionDescriptor;

        //-------------------------------------------------------------------------

        class KRG_TOOLS_ENTITY_API EntityCollectionDescriptorWriter
        {
        public:

            static bool WriteCollection( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& outFilePath, EntityCollectionDescriptor const& collection );
            static bool WriteCollection( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& outFilePath, EntityCollection const& collection );
        };
    }
}