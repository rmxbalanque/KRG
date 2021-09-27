#pragma  once

#include "Tools/Physics/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        struct KRG_TOOLS_PHYSICS_API PhysicsMaterialDatabaseResourceDescriptor : public Resource::ResourceDescriptor
        {
            KRG_REGISTER_TYPE( PhysicsMaterialDatabaseResourceDescriptor );

            KRG_EXPOSE TVector<DataPath>         m_materialLibraries;
        };

        //-------------------------------------------------------------------------

        class PhysicsMaterialDatabaseCompiler : public Resource::Compiler
        {
            static const int32 s_version = 0;

        public:

            PhysicsMaterialDatabaseCompiler();
            virtual Resource::CompilationResult Compile( Resource::CompileContext const& ctx ) const override;
        };
    }
}