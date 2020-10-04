#pragma once

//-------------------------------------------------------------------------
// C++ only serialization
//-------------------------------------------------------------------------

#pragma warning(disable:4512)
#include "System/Core/ThirdParty/cereal/cereal.hpp"

//-------------------------------------------------------------------------

#define KRG_SERIALIZE_NONE( ... ) friend cereal::access; \
template<class Archive> void serialize( Archive& archive ) {} \

#define KRG_SERIALIZE_MEMBERS( ... ) friend cereal::access; \
template<class Archive> void serialize( Archive& archive ) { archive( __VA_ARGS__ ); } \

#define KRG_SERIALIZE_BASE( base ) cereal::base_class<base>( this )

#define KRG_NVP( var ) cereal::make_nvp(#var, var)
#define KRG_MAKE_NVP( name, var ) cereal::make_nvp( #name, var)

#pragma warning(default:4512)