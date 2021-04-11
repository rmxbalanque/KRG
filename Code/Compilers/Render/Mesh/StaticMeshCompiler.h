#ifdef _WIN32
#pragma once

#include "Compilers/Render/_Module/API.h"
#include "MeshCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    struct KRG_RESOURCECOMPILERS_RENDER_API StaticMeshResourceDescriptor : public MeshResourceDescriptor
    {
        KRG_REGISTER_TYPE;
    };

    //-------------------------------------------------------------------------

    class StaticMeshCompiler : public MeshCompiler
    {
        static const int32 VERSION = 3;

    public:

        StaticMeshCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}

#endif