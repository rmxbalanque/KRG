#pragma once

#include "Tools/Resource/Compilers/ResourceCompiler.h"
#include "System/Render/RenderTexture.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    struct MaterialResourceDescriptor : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( MaterialResourceDescriptor );

        bool IsValid();

        EXPOSE TResourcePtr<Texture>       m_diffuseTexture;
    };

    //-------------------------------------------------------------------------

    class MaterialCompiler : public Resource::Compiler
    {
        static const int32 s_version = 0;

    public:

        MaterialCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}