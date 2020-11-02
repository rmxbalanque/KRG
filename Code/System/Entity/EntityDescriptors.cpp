#include "EntityDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
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