#include "EntityDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
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

    S32 EntityDescriptor::FindComponentIndex( UUID const& componentID ) const
    {
        KRG_ASSERT( componentID.IsValid() );

        S32 const numComponents = (S32) m_components.size();
        for ( S32 i = 0; i < numComponents; i++ )
        {
            if ( m_components[i].m_ID == componentID )
            {
                return i;
            }
        }

        return InvalidIndex;
    }
}