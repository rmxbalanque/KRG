#pragma once

#include "../_Module/API.h"
#include "System/Entity/Collections/EntityCollectionDescriptor.h"
#include "System/Resource/IResource.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    struct LoadingContext;

    //-------------------------------------------------------------------------
    // A compiled entity map template
    //-------------------------------------------------------------------------
    // This is a read-only resource that contains the serialized entities for a given map
    // This is not directly used in the game, instead we create an entity map instance from this map
    //-------------------------------------------------------------------------

    class KRG_SYSTEM_ENTITY_API EntityMapDescriptor final : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'MAP' );
        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_ID ), KRG_NVP( m_collectionDescriptor ) );

        friend class EntityMapCompiler;
        friend class EntityMapLoader;

    public:

        inline UUID GetID() const { return m_ID; }
        virtual bool IsValid() const override;

        EntityCollectionDescriptor const& GetCollectionDescriptor() const { return m_collectionDescriptor; }

    private:

        UUID                                m_ID;
        EntityCollectionDescriptor          m_collectionDescriptor;
    };
}