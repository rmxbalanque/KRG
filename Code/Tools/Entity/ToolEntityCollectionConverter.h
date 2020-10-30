#pragma once

#include "_Module/API.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityCollectionDescriptor;
    class ToolEntityCollection;

    //-------------------------------------------------------------------------

    class KRG_TOOLS_ENTITY_API ToolEntityCollectionConverter
    {
    public:

        static bool ConvertToToolsFormat( TypeSystem::TypeRegistry const& typeRegistry, EntityCollectionDescriptor const& inCollection, ToolEntityCollection& outCollection );
        static bool ConvertFromToolsFormat( TypeSystem::TypeRegistry const& typeRegistry, ToolEntityCollection const& inCollection, EntityCollectionDescriptor& outCollection );
    };
}