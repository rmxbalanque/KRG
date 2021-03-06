#if _WIN32
#include "ReflectionDataTypes.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace Reflection
        {
            PropertyDescriptor const* TypeDescriptor::GetPropertyDescriptor( StringID propertyID ) const
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

            void TypeDescriptor::AddValue( EnumConstantDescriptor const& constant )
            {
                KRG_ASSERT( m_ID.IsValid() && IsEnum() );

                StringID const id( constant.m_label );
                KRG_ASSERT( m_constants.find( id ) == m_constants.end() );
                m_constants[id] = constant;
            }

            bool TypeDescriptor::GetValueFromLabel( StringID labelID, uint32& value ) const
            {
                KRG_ASSERT( m_ID.IsValid() && IsEnum() );

                auto const iter = m_constants.find( labelID );
                if ( iter != m_constants.end() )
                {
                    value = iter->second.m_value;
                    return true;
                }

                return false;
            }
        }
    }
}

#endif