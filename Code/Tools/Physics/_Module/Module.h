#pragma once

#include "API.h"
#include "Tools/Physics/ResourceCompilers/ResourceCompiler_PhysicsMesh.h"
#include "Tools/Physics/ResourceCompilers/ResourceCompiler_PhysicsMaterialDatabase.h"
#include "Tools/Physics/ResourceCompilers/ResourceCompiler_PhysicsRagdoll.h"
#include "Tools/Core/Resource/Compilers/ResourceCompilerRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class KRG_TOOLS_PHYSICS_API ToolsModule
    {
        KRG_REGISTER_MODULE;

    public:

        void RegisterCompilers( Resource::CompilerRegistry& compilerRegistry )
        {
            compilerRegistry.RegisterCompiler( &m_meshCompiler );
            compilerRegistry.RegisterCompiler( &m_databaseCompiler );
            compilerRegistry.RegisterCompiler( &m_ragdollCompiler );
        }

        void UnregisterCompilers( Resource::CompilerRegistry& compilerRegistry )
        {
            compilerRegistry.UnregisterCompiler( &m_meshCompiler );
            compilerRegistry.UnregisterCompiler( &m_databaseCompiler );
            compilerRegistry.UnregisterCompiler( &m_ragdollCompiler );
        }

    private:

        PhysicsMeshCompiler                 m_meshCompiler;
        RagdollCompiler                     m_ragdollCompiler;
        PhysicsMaterialDatabaseCompiler     m_databaseCompiler;
    };
}
