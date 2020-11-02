#include "TypeInfo.h"
#include "TypeDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    bool TypeInfo::IsDerivedFrom( TypeID const potentialParentTypeID ) const
    {
        if ( potentialParentTypeID == m_ID )
        {
            return true;
        }

        for ( auto& actualParentTypeInfo : m_parentTypes )
        {
            if ( actualParentTypeInfo->m_ID == potentialParentTypeID )
            {
                return true;
            }

            // Check inheritance hierarchy
            if ( actualParentTypeInfo->IsDerivedFrom( potentialParentTypeID ) )
            {
                return true;
            }
        }

        return false;
    }

    //-------------------------------------------------------------------------

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