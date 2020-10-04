#ifdef _WIN32
#pragma once

#include "API.h"
#include "Tools/Resource/Compilers/ResourceCompilerModule.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        class KRG_RESOURCECOMPILERS_PHYSICS_API ResourceCompilerModule : public KRG::ResourceCompilerModule
        {
            KRG_REGISTER_MODULE;

        public:

            ResourceCompilerModule();
        };
    }
}

#endif
