#include "EntityDescriptors.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    int32 EntityDescriptor::FindComponentIndex( StringID const& componentName ) const
    {
        KRG_ASSERT( componentName.IsValid() );

        int32 const numComponents = (int32) m_components.size();
        for ( int32 i = 0; i < numComponents; i++ )
        {
            if ( m_components[i].m_name == componentName )
            {
                return i;
            }
        }

        return InvalidIndex;
    }
}