#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Resource/ResourceID.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    struct KRG_TOOLS_CORE_API ResourceDescriptor : public IRegisteredType
    {
        KRG_REGISTER_TYPE( ResourceDescriptor );

        virtual ~ResourceDescriptor() = default;
        static void ReadCompileDependencies( String const& descriptorFileContents, TVector<DataPath>& outDependencies );
    };
}