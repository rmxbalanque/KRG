#ifdef _WIN32

//-------------------------------------------------------------------------
// Compilers are only run on windows machines
//-------------------------------------------------------------------------

#pragma once

#include "../_Module/API.h"
#include "ResourceDescriptor.h"
#include "Tools/Core/TypeSystem/Serialization/NativeTypeReader.h"
#include "System/Resource/ResourceHeader.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Types/Function.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    enum class CompilationResult
    {
        Success,
        SuccessWithWarnings,
        Failure,
    };

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_RESOURCE_API CompileContext
    {
        CompileContext( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataPath, FileSystem::Path const& compiledDataPath, ResourceID const& resourceToCompile );

        bool IsValid() const;

        inline bool ConvertDataPathToFilePath( DataPath const& resourcePath, FileSystem::Path& filePath ) const
        {
            if ( resourcePath.IsValid() )
            {
                filePath = DataPath::ToFileSystemPath( m_sourceDataPath, resourcePath );
                return true;
            }
            else
            {
                KRG_LOG_ERROR( "ResourceCompiler", "Invalid data path encountered: '%s'", resourcePath.c_str() );
                return false;
            }
        }

        // This function will just read a resource descriptor from a file
        template<typename T>
        inline bool TryReadResourceDescriptorFromFile( FileSystem::Path const& descriptorPath, T& outData ) const
        {
            static_assert( std::is_base_of<ResourceDescriptor, T>::value, "T must be a child of ResourceDescriptor" );
            KRG_ASSERT( IsValid() );

            TypeSystem::NativeTypeReader nativeTypeReader( m_typeRegistry );
            if ( !nativeTypeReader.ReadFromFile( descriptorPath ) )
            {
                KRG_LOG_ERROR( "ResourceCompiler", "Failed to read resource descriptor file: %s", descriptorPath.c_str() );
                return false;
            }

            nativeTypeReader >> outData;
            return true;
        }

        template<typename T>
        inline bool TryReadResourceDescriptor( T& outData ) const
        {
            return TryReadResourceDescriptorFromFile( m_inputFilePath, outData );
        }

        // This function will read a resource descriptor as well as any additionally authored data in the file
        template<typename T>
        inline bool TryReadResourceDescriptorFromFile( FileSystem::Path const& descriptorPath, T& outData, TFunction<bool( TypeSystem::NativeTypeReader& )>& readExtraDataFunction ) const
        {
            static_assert( std::is_base_of<ResourceDescriptor, T>::value, "T must be a child of ResourceDescriptor" );
            KRG_ASSERT( IsValid() );

            TypeSystem::NativeTypeReader nativeTypeReader( m_typeRegistry );
            if ( !nativeTypeReader.ReadFromFile( descriptorPath ) )
            {
                KRG_LOG_ERROR( "ResourceCompiler", "Failed to read resource descriptor file: %s", descriptorPath.c_str() );
                return false;
            }

            nativeTypeReader >> outData;
            return readExtraDataFunction( nativeTypeReader );
        }

        template<typename T>
        inline bool TryReadResourceDescriptor( T& outData, TFunction<bool( TypeSystem::NativeTypeReader& )>& readExtraDataFunction ) const
        {
            return TryReadResourceDescriptorFromFile( m_inputFilePath, outData, readExtraDataFunction );
        }

    public:

        TypeSystem::TypeRegistry const&                 m_typeRegistry;
        Platform::Target const                          m_platform = Platform::Target::PC;
        FileSystem::Path const                            m_sourceDataPath;
        FileSystem::Path const                            m_compiledDataPath;

        ResourceID const                                m_resourceID;
        FileSystem::Path const                            m_inputFilePath;
        FileSystem::Path const                            m_outputFilePath;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_RESOURCE_API Compiler
    {

    public:

        Compiler( String const& name, int32 version ) : m_name( name ), m_version( version ) {}
        virtual ~Compiler() {}
        virtual CompilationResult Compile( CompileContext const& ctx ) const = 0;

        String const& GetName() const { return m_name; }
        inline int32 GetVersion() const { return m_version; }

        // The list of resource type we can compile
        virtual TVector<ResourceTypeID> const& GetOutputTypes() const { return m_outputTypes; }

        // The list of virtual resources we produce as part of the compilation process
        virtual TVector<ResourceTypeID> const& GetVirtualTypes() const { return m_virtualTypes; }

    protected:

        Compiler& operator=( Compiler const& ) = delete;

        CompilationResult Error( char const* pFormat, ... ) const;
        CompilationResult Warning( char const* pFormat, ... ) const;
        CompilationResult Message( char const* pFormat, ... ) const;

        CompilationResult CompilationSucceeded( CompileContext const& ctx ) const;
        CompilationResult CompilationSucceededWithWarnings( CompileContext const& ctx ) const;
        CompilationResult CompilationFailed( CompileContext const& ctx ) const;

    protected:

        int32                         m_version;
        String                      m_name;
        TVector<ResourceTypeID>     m_outputTypes;
        TVector<ResourceTypeID>     m_virtualTypes;
    };
}

#endif