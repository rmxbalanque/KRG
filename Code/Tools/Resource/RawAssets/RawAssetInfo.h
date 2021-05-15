#pragma once

#include "../_Module/API.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Types/String.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Time/Time.h"

//-------------------------------------------------------------------------
// Provide info about a raw file
//-------------------------------------------------------------------------
// These are various info data structures allowing our tools to get information regarding the contents of a raw file

namespace KRG::RawAssets
{
    struct KRG_TOOLS_RESOURCE_API RawMeshInfo
    {
        String      m_name;
        String      m_materialName;
        bool        m_isSkinned = false;
    };

    struct KRG_TOOLS_RESOURCE_API RawAnimationInfo
    {
        String      m_name;
        Seconds     m_duration = 0;
        float       m_frameRate = 0;
    };

    struct KRG_TOOLS_RESOURCE_API RawSkeletonInfo
    {
        String      m_name;
    };

    //-------------------------------------------------------------------------

    struct RawAssetInfo
    {
        inline void Reset()
        {
            m_meshes.clear();
            m_animations.clear();
            m_skeletons.clear();
        }

        TVector<RawMeshInfo>            m_meshes;
        TVector<RawAnimationInfo>       m_animations;
        TVector<RawSkeletonInfo>        m_skeletons;
    };
}