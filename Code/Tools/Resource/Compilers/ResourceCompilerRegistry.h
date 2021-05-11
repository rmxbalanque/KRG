#pragma once
#include "System/Core/Types/Containers.h"
#include "ResourceCompiler.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class KRG_TOOLS_RESOURCE_API CompilerRegistry
    {
    public:

        CompilerRegistry() = default;
        ~CompilerRegistry();

        void RegisterCompiler( Compiler const* pCompiler );
        void UnregisterCompiler( Compiler const* pCompiler );

        //-------------------------------------------------------------------------

        inline TVector<Compiler const*> const& GetRegisteredCompilers() const { return m_compilers; }

        inline bool IsVirtualType( ResourceTypeID const& typeID ) const { return m_compilerVirtualTypeMap.find( typeID ) != m_compilerVirtualTypeMap.end(); }

        inline bool HasCompilerForType( ResourceTypeID const& typeID ) const { return m_compilerTypeMap.find( typeID ) != m_compilerTypeMap.end(); }

        inline Compiler const* GetCompilerForType( ResourceTypeID const& typeID ) const
        {
            auto compilerTypeIter = m_compilerTypeMap.find( typeID );
            if ( compilerTypeIter != m_compilerTypeMap.end() )
            {
                return compilerTypeIter->second;
            }

            return nullptr;
        }

        inline int32 GetVersionForType( ResourceTypeID const& typeID ) const
        {
            auto pCompiler = GetCompilerForType( typeID );
            KRG_ASSERT( pCompiler != nullptr );
            return pCompiler->GetVersion();
        }

    private:

        TVector<Compiler const*>                            m_compilers;
        THashMap<ResourceTypeID, Compiler const*>           m_compilerTypeMap;
        THashMap<ResourceTypeID, Compiler const*>           m_compilerVirtualTypeMap;
    };
}