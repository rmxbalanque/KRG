#include "EntityDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    int32 EntityDescriptor::FindComponentIndex( UUID const& componentID ) const
    {
        KRG_ASSERT( componentID.IsValid() );

        int32 const numComponents = (int32) m_components.size();
        for ( int32 i = 0; i < numComponents; i++ )
        {
            if ( m_components[i].m_ID == componentID )
            {
                return i;
            }
        }

        return InvalidIndex;
    }
}