#include "ToolEntityCollection.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    ToolEntityCollection::ToolEntityCollection( TypeSystem::TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {}

    TVector<ToolEntityComponent const*> ToolEntityCollection::GetAllComponentsOfType( TypeSystem::TypeID componentTypeID, bool allowDerivedTypes ) const
    {
        TVector<ToolEntityComponent const*> foundComponents;

        for ( auto const& pEntity : m_entities )
        {
            KRG_ASSERT( pEntity != nullptr );
            pEntity->GetAllComponentsOfType( componentTypeID, allowDerivedTypes, foundComponents );
        }

        return foundComponents;
    }

    ToolEntityCollection::~ToolEntityCollection()
    {
        for ( auto& pEntity : m_entities )
        {
            KRG::Delete( pEntity );
        }
    }
}