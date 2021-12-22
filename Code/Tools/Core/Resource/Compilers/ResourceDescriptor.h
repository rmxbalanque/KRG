#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/TypeSystem/TypeSerialization.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Resource/ResourceID.h"
#include "System/Core/Types/Function.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    struct KRG_TOOLS_CORE_API ResourceDescriptor : public IRegisteredType
    {
        KRG_REGISTER_TYPE( ResourceDescriptor );

        static void ReadCompileDependencies( String const& descriptorFileContents, TVector<ResourcePath>& outDependencies );

        virtual ~ResourceDescriptor() = default;

        // Can this descriptor be created by a user in the editor?
        virtual bool IsUserCreateableDescriptor() const { return false; }

        // What is the compiled resource type for this descriptor - Only needed for user createable descriptors
        virtual ResourceTypeID GetCompiledResourceTypeID() const { return ResourceTypeID(); }
    };

    // Tools only deserialization function
    //-------------------------------------------------------------------------

    template<typename T>
    bool TryReadResourceDescriptorFromFile( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& descriptorPath, T& outData, TFunction<bool( rapidjson::Document const& )> const& customDeserializationFunction = TFunction<bool( rapidjson::Document const& )>() )
    {
        static_assert( std::is_base_of<ResourceDescriptor, T>::value, "T must be a child of ResourceDescriptor" );

        TypeSystem::Serialization::TypeReader typeReader( typeRegistry );
        if ( !typeReader.ReadFromFile( descriptorPath ) )
        {
            KRG_LOG_ERROR( "Resource", "Failed to read resource descriptor file: %s", descriptorPath.c_str() );
            return false;
        }

        if ( !typeReader.ReadType( &outData ) )
        {
            return false;
        }

        // Allow for custom data serialization in descriptors
        if ( customDeserializationFunction != nullptr )
        {
            if ( !customDeserializationFunction( static_cast<JsonReader&>( typeReader ).GetDocument() ) )
            {
                return false;
            }
        }

        return true;
    }

    template<typename T>
    bool WriteResourceDescriptorToFile( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& descriptorPath, T const* pDescriptorData )
    {
        static_assert( std::is_base_of<ResourceDescriptor, T>::value, "T must be a child of ResourceDescriptor" );

        KRG_ASSERT( descriptorPath.IsFile() );

        TypeSystem::Serialization::TypeWriter typeWriter( typeRegistry );
        typeWriter << pDescriptorData;
        return typeWriter.WriteToFile( descriptorPath );
    }
}