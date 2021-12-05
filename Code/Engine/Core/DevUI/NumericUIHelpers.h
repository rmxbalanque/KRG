#pragma once
#include "Engine/Core/_Module/API.h"
#include "System/Core/Math/Transform.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    KRG_ENGINE_CORE_API bool InputFloat2( char const* pID, Float2& value, float width = -1, bool readOnly = false );
    KRG_ENGINE_CORE_API bool InputFloat3( char const* pID, Float3& value, float width = -1, bool readOnly = false );
    KRG_ENGINE_CORE_API bool InputFloat4( char const* pID, Float4& value, float width = -1, bool readOnly = false );
    KRG_ENGINE_CORE_API bool InputFloat4( char const* pID, Vector& value, float width = -1, bool readOnly = false );

    //-------------------------------------------------------------------------

    KRG_ENGINE_CORE_API void DisplayVector2( Vector const& v, float width = -1 );
    KRG_ENGINE_CORE_API void DisplayVector3( Vector const& v, float width = -1 );
    KRG_ENGINE_CORE_API void DisplayVector4( Vector const& v, float width = -1 );
    KRG_FORCE_INLINE void DisplayVector( Float2 const& v, float width = -1 ) { return DisplayVector2( v, width ); }
    KRG_FORCE_INLINE void DisplayVector( Float3 const& v, float width = -1 ) { return DisplayVector3( v, width ); }
    KRG_FORCE_INLINE void DisplayVector( Float4 const& v, float width = -1 ) { return DisplayVector4( v, width ); }
    KRG_ENGINE_CORE_API void DisplayRotation( Quaternion const& q, float width = -1 );
    KRG_ENGINE_CORE_API void DisplayTransform( Transform const& t, float width = -1 );
}
#endif