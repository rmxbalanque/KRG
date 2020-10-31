#pragma once

#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Resource/ResourceTypeID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        struct CompilerInfo
        {
            String                              m_name;
            S32                                 m_version;
            TVector<ResourceTypeID>             m_outputTypes;
            TVector<ResourceTypeID>             m_virtualTypes;
            FileSystemPath                      m_sourceModule;
        };
    }
}