#ifdef _WIN32
#pragma  once

#include "_Module/API.h"
#include "Tools/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        struct KRG_RESOURCECOMPILERS_PHYSICS_API PhysicsMaterialDatabaseResourceDescriptor : public Resource::ResourceDescriptor
        {
            KRG_REGISTER_TYPE;

            EXPOSE TVector<DataPath>         m_materialLibraries;
        };

        //-------------------------------------------------------------------------

        class PhysicsMaterialDatabaseCompiler : public Resource::Compiler
        {
            static const int32 VERSION = 0;

        public:

            PhysicsMaterialDatabaseCompiler();
            virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
        };
    }
}

#endif