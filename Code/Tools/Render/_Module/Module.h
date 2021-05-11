#pragma once

#include "API.h"
#include "Tools/Render/ResourceCompilers/Shaders/ShaderCompiler.h"
#include "Tools/Render/ResourceCompilers/Mesh/StaticMeshCompiler.h"
#include "Tools/Render/ResourceCompilers/Mesh/SkeletalMeshCompiler.h"
#include "Tools/Render/ResourceCompilers/Textures/TextureCompiler.h"
#include "Tools/Render/ResourceCompilers/Materials/MaterialCompiler.h"
#include "Tools/Resource/Compilers/ResourceCompilerRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_TOOLS_RENDER_API ToolsModule
    {
        KRG_REGISTER_MODULE;

    public:

        void RegisterCompilers( Resource::CompilerRegistry& compilerRegistry )
        {
            compilerRegistry.RegisterCompiler( &m_textureCompiler );
            compilerRegistry.RegisterCompiler( &m_materialCompiler );
            compilerRegistry.RegisterCompiler( &m_shaderCompiler );
            compilerRegistry.RegisterCompiler( &m_staticMeshCompiler );
            compilerRegistry.RegisterCompiler( &m_skeletalMeshCompiler );
        }

        void UnregisterCompilers( Resource::CompilerRegistry& compilerRegistry )
        {
            compilerRegistry.UnregisterCompiler( &m_textureCompiler );
            compilerRegistry.UnregisterCompiler( &m_materialCompiler );
            compilerRegistry.UnregisterCompiler( &m_shaderCompiler );
            compilerRegistry.UnregisterCompiler( &m_staticMeshCompiler );
            compilerRegistry.UnregisterCompiler( &m_skeletalMeshCompiler );
        }

    private:

        TextureCompiler             m_textureCompiler;
        MaterialCompiler            m_materialCompiler;
        ShaderCompiler              m_shaderCompiler;
        StaticMeshCompiler          m_staticMeshCompiler;
        SkeletalMeshCompiler        m_skeletalMeshCompiler;
    };
}
