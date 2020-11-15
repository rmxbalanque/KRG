#include "ResourceRecord.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        ResourceRecord::~ResourceRecord()
        {
            KRG_ASSERT( m_pResource == nullptr && !HasReferences() );
        }
    }
}