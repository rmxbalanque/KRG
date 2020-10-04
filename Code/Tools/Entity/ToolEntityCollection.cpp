#include "ToolEntityCollection.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ToolEntityCollection::ToolEntityCollection( TypeSystem::TypeRegistry const& typeRegistry )
        : m_typeRegistry( typeRegistry )
    {}

    TVector<ToolEntityComponent const*> ToolEntityCollection::GetAllComponentsOfType( TypeSystem::TypeID componentTypeID, bool allowDerivedTypes ) const
    {
        TVector<ToolEntityComponent const*> foundComponents;

        for ( auto const& entity : m_entities )
        {
            for ( auto const& component : entity.GetComponents() )
            {
                KRG_ASSERT( component.IsValid() );
                if ( component.GetTypeID() == componentTypeID || ( allowDerivedTypes && m_typeRegistry.IsTypeDerivedFrom( component.GetTypeID(), componentTypeID ) ) )
                {
                    foundComponents.emplace_back( &component );
                }
            }
        }

        return foundComponents;
    }
}