#include "ResourceCompilerRegistry.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    CompilerRegistry::~CompilerRegistry()
    {
        KRG_ASSERT( m_compilers.empty() );
        KRG_ASSERT( m_compilerTypeMap.empty() );
        KRG_ASSERT( m_compilerVirtualTypeMap.empty() );
    }

    void CompilerRegistry::RegisterCompiler( Compiler const* pCompiler )
    {
        KRG_ASSERT( pCompiler != nullptr );
        KRG_ASSERT( !VectorContains( m_compilers, pCompiler ) );

        //-------------------------------------------------------------------------

        m_compilers.emplace_back( pCompiler );

        //-------------------------------------------------------------------------

        auto const& resourceTypes = pCompiler->GetOutputTypes();
        for ( auto& type : resourceTypes )
        {
            // Two compilers registering for the same resource type is not allowed
            KRG_ASSERT( m_compilerTypeMap.find( type ) == m_compilerTypeMap.end() );
            m_compilerTypeMap.insert( TPair<ResourceTypeID, Resource::Compiler const*>( type, pCompiler ) );
        }

        auto const& virtualResourceTypes = pCompiler->GetVirtualTypes();
        for ( auto& type : virtualResourceTypes )
        {
            // Two compilers registering for the same resource type is not allowed
            KRG_ASSERT( m_compilerVirtualTypeMap.find( type ) == m_compilerVirtualTypeMap.end() );
            m_compilerVirtualTypeMap.insert( TPair<ResourceTypeID, Resource::Compiler const*>( type, pCompiler ) );
        }
    }

    void CompilerRegistry::UnregisterCompiler( Compiler const* pCompiler )
    {
        KRG_ASSERT( pCompiler != nullptr );
        KRG_ASSERT( VectorContains( m_compilers, pCompiler ) );

        //-------------------------------------------------------------------------

        m_compilers.erase_first_unsorted( pCompiler );

        //-------------------------------------------------------------------------

        auto const& resourceTypes = pCompiler->GetOutputTypes();
        for ( auto& type : resourceTypes )
        {
            m_compilerTypeMap.erase( type );
        }

        auto const& virtualResourceTypes = pCompiler->GetVirtualTypes();
        for ( auto& type : virtualResourceTypes )
        {
            m_compilerVirtualTypeMap.erase( type );
        }
    }
}