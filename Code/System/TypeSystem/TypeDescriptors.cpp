#include "TypeDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    StringID PropertyDescriptor::GetEnumValueID() const
    {
        KRG_ASSERT( m_byteValue.size() == 4 );

        U32 rawStringID = 0;
        rawStringID |= m_byteValue[0];
        rawStringID |= m_byteValue[1] << 8;
        rawStringID |= m_byteValue[2] << 16;
        rawStringID |= m_byteValue[3] << 24;

        return StringID( rawStringID );
    }

    void PropertyDescriptor::SetEnumValueID( StringID enumValueID )
    {
        U32 const rawStringID = enumValueID.GetID();
        m_byteValue.emplace_back( rawStringID & 0xFF );
        m_byteValue.emplace_back( ( rawStringID >> 8 ) & 0xFF );
        m_byteValue.emplace_back( ( rawStringID >> 16 ) & 0xFF );
        m_byteValue.emplace_back( ( rawStringID >> 24 ) & 0xFF );
    }

    //-------------------------------------------------------------------------

    void TypeDescriptor::RemovePropertyValue( TypeSystem::PropertyPath const& path )
    {
        for ( S32 i = (S32) m_propertyValues.size() - 1; i >= 0; i-- )
        {
            if ( m_propertyValues[i].m_path == path )
            {
                VectorEraseUnsorted( m_propertyValues, i );
            }
        }
    }
}