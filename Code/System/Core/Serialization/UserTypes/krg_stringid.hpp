#pragma once
#ifndef CEREAL_TYPES_KRG_STRINGID_HPP_
#define CEREAL_TYPES_KRG_STRINGID_HPP_

#include <cereal/cereal.hpp>
#include <EASTL/string.h>
#include "../Types/StringID.h"

namespace cereal
{
    //! Serialization for basic_string types, if binary data is supported
    template <class Archive>
    inline void CEREAL_SAVE_FUNCTION_NAME( Archive& ar, KRG::StringID const& ID )
    {
        if ( ID.IsValid() )
        {
            // Save number of chars + the data
            char const* pDebugString = ID.c_str();
            size_type const length = strlen( pDebugString );
            ar( make_size_tag( static_cast<size_type>( length ) ) );

            for ( auto i = 0; i < length; i++ )
            {
                ar( pDebugString[i] );
            }
        }
        else
        {
            ar( make_size_tag( size_type( 0 ) ) );
        }
    }

    //! Serialization for basic_string types, if binary data is supported
    template <class Archive> 
    inline void CEREAL_LOAD_FUNCTION_NAME( Archive& ar, KRG::StringID& ID )
    {
        size_type size;
        ar( make_size_tag( size ) );

        if ( size > 0 )
        {
            KRG::String str;
            str.resize( static_cast<std::size_t>( size ) );

            for ( auto& c : str )
            {
                ar( c );
            }

            ID = KRG::StringID( str.c_str() );
        }
    }
} // namespace cereal

#endif // CEREAL_TYPES_STRING_HPP_

