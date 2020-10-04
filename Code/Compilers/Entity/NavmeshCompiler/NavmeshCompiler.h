#pragma once

#include "Tools/Resource/Compilers/ResourceCompiler.h"
#include "System/Entity/Map/EntityMap.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace AI
    {
        struct NavmeshResourceDescriptor : public Resource::ResourceDescriptor
        {
            KRG_REGISTER_TYPE;

            EXPOSE TResourcePtr<EntityMap>         m_map;
        };
    }

    //-------------------------------------------------------------------------

    class NavmeshCompiler : public Resource::Compiler
    {
        static const S32 VERSION = 1;

    public:

        NavmeshCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}
