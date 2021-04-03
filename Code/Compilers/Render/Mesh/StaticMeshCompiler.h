#ifdef _WIN32
#pragma once

#include "MeshCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    struct StaticMeshResourceDescriptor : public MeshResourceDescriptor
    {
        KRG_REGISTER_TYPE;
    };

    //-------------------------------------------------------------------------

    class StaticMeshCompiler : public MeshCompiler
    {
        static const int32 VERSION = 2;

    public:

        StaticMeshCompiler();
        virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
    };
}

#endif