#pragma once
#include "System/Core/Core/IntegralTypes.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------
// Physics Layers
//-------------------------------------------------------------------------
// The layers that we can add our physics objects to
// We allow a max of 32 layers
// The layers are synonymous with object types. 
// When we perform any scene queries, we will specify the layers that those queries can interact with

namespace KRG::Physics
{
    // Max allowed enum value of 32
    enum class Layers : uint8
    {
        KRG_REGISTER_ENUM

        Environment = 0,
        Navigation = 1,
        Camera = 2,
        IK = 3,
        Characters = 4,
    };

    //-------------------------------------------------------------------------

    template<typename... Args>
    inline uint32 CreateLayerMask( Args&&... args )
    {
        uint32 layerMask = 0;
        ( ( layerMask |= 1u << (uint8) std::forward<Args>( args ) ), ... );
        return layerMask;
    }
}