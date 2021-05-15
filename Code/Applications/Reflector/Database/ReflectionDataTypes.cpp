#include "ReflectionDataTypes.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Reflection
{
    ReflectedProperty const* ReflectedType::GetPropertyDescriptor( StringID propertyID ) const
    {
        KRG_ASSERT( m_ID.IsValid() && !IsAbstract() && !IsEnum() );
        for ( auto const& prop : m_properties )
        {
            if ( prop.m_propertyID == propertyID )
            {
                return &prop;
            }
        }

        return nullptr;
    }

    //-------------------------------------------------------------------------

    void ReflectedType::AddEnumConstant( ReflectedEnumConstant const& constant )
    {
        KRG_ASSERT( m_ID.IsValid() && IsEnum() );

        StringID const id( constant.m_label );
        KRG_ASSERT( m_enumConstants.find( id ) == m_enumConstants.end() );
        m_enumConstants[id] = constant;
    }

    bool ReflectedType::GetValueFromEnumLabel( StringID labelID, uint32& value ) const
    {
        KRG_ASSERT( m_ID.IsValid() && IsEnum() );

        auto const iter = m_enumConstants.find( labelID );
        if ( iter != m_enumConstants.end() )
        {
            value = iter->second.m_value;
            return true;
        }

        return false;
    }
}