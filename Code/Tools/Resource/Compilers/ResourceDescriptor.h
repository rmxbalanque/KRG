#ifdef _WIN32

#pragma once

#include "../_Module/API.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Resource/ResourceID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        struct KRG_TOOLS_RESOURCE_API ResourceDescriptor
        {
            KRG_REGISTER_TYPE;
            static void ReadCompileDependencies( String const& descriptorFileContents, TVector<DataPath>& outDependencies );
        };
    }
}

#endif