#pragma once

#include "TypeID.h"
#include "CoreTypeIDs.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        struct EnumInfo
        {
            inline bool IsValidValue( StringID label ) const
            {
                auto const iter = m_constants.find( label );
                return iter != m_constants.end();
            }

            inline S64 GetConstantValue( StringID label ) const
            {
                auto const iter = m_constants.find( label );
                KRG_ASSERT( iter != m_constants.end() );
                return iter->second;
            }

            inline StringID GetConstantLabel( S64 value ) const
            {
                for ( auto const& pair : m_constants )
                {
                    if ( pair.second == value )
                    {
                        return pair.first;
                    }
                }

                KRG_UNREACHABLE_CODE();
                return StringID();
            }

        public:

            TypeID                                              m_ID;
            CoreTypes                                           m_underlyingType;
            THashMap<StringID, S64>                             m_constants;
        };
    }
}
