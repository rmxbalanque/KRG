#pragma once

#include "Engine/Core/Entity/EntityCollectionDescriptor.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct EntityLoadingContext;

    //-------------------------------------------------------------------------
    // A compiled entity map template
    //-------------------------------------------------------------------------
    // This is a read-only resource that contains the serialized entities for a given map
    // This is not directly used in the game, instead we create an entity map instance from this map
    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityMapDescriptor final : public EntityCollectionDescriptor
    {
        KRG_REGISTER_RESOURCE( 'MAP', "Map" );
        KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( EntityCollectionDescriptor ), KRG_NVP( m_ID ) );

        friend class EntityCollectionCompiler;
        friend class EntityCollectionLoader;

    public:

        inline UUID GetID() const { return m_ID; }

    private:

        UUID                                m_ID;
    };
}