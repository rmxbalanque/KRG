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
            inline size_t GetNumValues() const
            {
                return m_constants.size();
            }

            inline bool IsValidValue( StringID label ) const
            {
                auto const iter = m_constants.find( label );
                return iter != m_constants.end();
            }

            inline int64 GetConstantValue( StringID label ) const
            {
                auto const iter = m_constants.find( label );
                KRG_ASSERT( iter != m_constants.end() );
                return iter->second;
            }

            inline bool TryGetConstantValue( StringID label, int64& outValue ) const
            {
                auto const iter = m_constants.find( label );
                if ( iter != m_constants.end() )
                {
                    outValue = iter->second;
                    return true;
                }

                return false;
            }

            inline StringID GetConstantLabel( int64 value ) const
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

            inline bool TryGetConstantLabel( int64 value, StringID& outValue ) const
            {
                for ( auto const& pair : m_constants )
                {
                    if ( pair.second == value )
                    {
                         outValue = pair.first;
                         return true;
                    }
                }

                return false;
            }

        public:

            TypeID                                              m_ID;
            CoreTypeID                                           m_underlyingType;
            THashMap<StringID, int64>                           m_constants;
        };
    }
}
