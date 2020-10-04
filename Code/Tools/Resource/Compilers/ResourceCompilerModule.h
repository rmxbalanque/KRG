#ifdef _WIN32 

//-------------------------------------------------------------------------
// Compilers are only run on windows machines
//-------------------------------------------------------------------------

#pragma once

#include "../_Module/API.h"
#include "ResourceCompiler.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/TypeSystem/TypeRegistry.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_TOOLS_RESOURCE_API ResourceCompilerModule
    {

    public:

        ResourceCompilerModule( char const* pModuleName );
        virtual ~ResourceCompilerModule();

        inline TVector<Resource::Compiler*> const& GetRegisteredCompilers() const { return m_registeredCompilers; }

    protected:

        inline void AddCompiler( Resource::Compiler* pCompiler )
        {
            KRG_ASSERT( VectorFindIndex( m_registeredCompilers, pCompiler ) == InvalidIndex ); 
            m_registeredCompilers.push_back( pCompiler );
        }
       
    private:

        TVector<Resource::Compiler*>                    m_registeredCompilers;
        String                                          m_moduleName;
    };

    //-------------------------------------------------------------------------

    typedef ResourceCompilerModule* ( __cdecl *GetResourceCompilerModuleFuncPtr )();
}

#endif
