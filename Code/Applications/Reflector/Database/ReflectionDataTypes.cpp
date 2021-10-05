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

    //-------------------------------------------------------------------------

    bool ReflectedResourceType::TryParseRegistrationMacroString( String const& registrationStr )
    {
        // Generate type ID string and get friendly name
        size_t const resourceIDStartIdx = registrationStr.find( '\'', 0 );
        if ( resourceIDStartIdx == String::npos )
        {
            return false;
        }

        size_t const resourceIDEndIdx = registrationStr.find( '\'', resourceIDStartIdx + 1 );
        if ( resourceIDEndIdx == String::npos )
        {
            return false;
        }

        size_t const resourceFriendlyNameStartIdx = registrationStr.find( '"', resourceIDEndIdx + 1 );
        if ( resourceFriendlyNameStartIdx == String::npos )
        {
            return false;
        }

        size_t const resourceFriendlyNameEndIdx = registrationStr.find( '"', resourceFriendlyNameStartIdx + 1 );
        if ( resourceFriendlyNameEndIdx == String::npos )
        {
            return false;
        }

        KRG_ASSERT( resourceIDStartIdx != resourceIDEndIdx );
        KRG_ASSERT( resourceFriendlyNameStartIdx != resourceFriendlyNameEndIdx );

        //-------------------------------------------------------------------------

        String const resourceTypeIDString = registrationStr.substr( resourceIDStartIdx + 1, resourceIDEndIdx - resourceIDStartIdx - 1 );
        m_resourceTypeID = ResourceTypeID( resourceTypeIDString );
        
        m_friendlyName = registrationStr.substr( resourceFriendlyNameStartIdx + 1, resourceFriendlyNameEndIdx - resourceFriendlyNameStartIdx - 1 );

        return true;
    }
}