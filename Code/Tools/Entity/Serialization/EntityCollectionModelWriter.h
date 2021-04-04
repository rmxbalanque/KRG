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
        class EntityCollectionDescriptor;

        //-------------------------------------------------------------------------

        class KRG_TOOLS_ENTITY_API EntityCollectionModelWriter
        {
        public:

            static bool WriteCollection( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& outFilePath, EntityCollectionDescriptor const& collection );
        };
    }
}