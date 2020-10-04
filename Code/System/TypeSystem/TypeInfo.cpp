#include "TypeInfo.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        bool TypeInfo::IsDerivedFrom( TypeID const parentTypeID ) const
        {
            if ( parentTypeID == m_ID )
            {
                return true;
            }

            for ( auto& parentTypeInfo : m_parentTypes )
            {
                if ( parentTypeInfo->m_ID == parentTypeID )
                {
                    return true;
                }

                // Check inheritance hierarchy
                if ( parentTypeInfo->IsDerivedFrom( parentTypeID ) )
                {
                    return true;
                }
            }

            return false;
        }

        PropertyInfo const* TypeInfo::GetPropertyInfo( StringID propertyID ) const
        {
            PropertyInfo const* pProperty = nullptr;

            auto propertyIter = m_properties.find( propertyID );
            if ( propertyIter != m_properties.end() )
            {
                pProperty = &propertyIter->second;
            }

            return pProperty;
        }
    }
}