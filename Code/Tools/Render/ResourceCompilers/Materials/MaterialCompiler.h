#ifdef _WIN32
#pragma once

#include "Tools/Resource/Compilers/ResourceCompiler.h"
#include "System/Render/RenderTexture.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        struct MaterialResourceDescriptor : public Resource::ResourceDescriptor
        {
            KRG_REGISTER_TYPE;

            bool IsValid();

            EXPOSE TResourcePtr<Texture>       m_diffuseTexture;
        };

        //-------------------------------------------------------------------------

        class MaterialCompiler : public Resource::Compiler
        {
            static const int32 VERSION = 0;

        public:

            MaterialCompiler();
            virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
        };
    }
}

#endif