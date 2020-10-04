#ifdef _WIN32
#pragma once

#include "API.h"
#include "Tools/Resource/Compilers/ResourceCompilerModule.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        class KRG_RESOURCECOMPILERS_RENDER_API ResourceCompilerModule : public KRG::ResourceCompilerModule
        {
            KRG_REGISTER_MODULE;

        public:

            ResourceCompilerModule();
        };
    }
}

#endif
