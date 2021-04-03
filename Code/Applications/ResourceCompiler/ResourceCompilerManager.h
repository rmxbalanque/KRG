#pragma once
#include "System/TypeSystem/TypeRegistry.h"
#include "Compilers/Animation/_Module/Module.h"
#include "Compilers/Render/_Module/Module.h"
#include "Compilers/Physics/_Module/Module.h"
#include "Compilers/Entity/_Module/Module.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceCompilerManager
    {
    public:

        ResourceCompilerManager();
        ~ResourceCompilerManager();

        CompilationResult Compile( ResourceID const& resourceToCompile );

    private:

        void UpdateCompilerTypeMap( TVector<Resource::Compiler*> const& compilers );

    private:

        FileSystem::Path                                        m_sourceDataPath;
        FileSystem::Path                                        m_compiledDataPath;
        TypeSystem::TypeRegistry                              m_typeRegistry;

        Animation::ResourceCompilerModule                     m_animationModule;
        Render::ResourceCompilerModule                        m_renderModule;
        Physics::ResourceCompilerModule                       m_physicsModule;
        EntityModel::ResourceCompilerModule                   m_entityModule;

        THashMap<ResourceTypeID, Resource::Compiler*>         m_compilerTypeMap;
    };
}