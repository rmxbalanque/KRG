#ifdef _WIN32
#pragma once

#include "API.h"
#include "Tools/Resource/Compilers/ResourceCompilerModule.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace EntityModel
    {
        class KRG_RESOURCECOMPILERS_ENTITY_API ResourceCompilerModule : public KRG::ResourceCompilerModule
        {
            KRG_REGISTER_MODULE;

        public:

            ResourceCompilerModule();
        };
    }
}

#endif
