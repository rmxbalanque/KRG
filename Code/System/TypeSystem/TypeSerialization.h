#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Core/ThirdParty/KRG_RapidJson.h"
#include "System/TypeSystem/TypeDescriptors.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem 
{
    class TypeRegistry;
    class TypeInstanceModel;
}

//-------------------------------------------------------------------------
// Type Serialization
//-------------------------------------------------------------------------
// This file contain basic facilities to convert between JSON and the various type representations we have

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::TypeSystem::Serialization
{
    constexpr static char const* const s_typeIDKey = "TypeID";

    // Descriptors
    //-------------------------------------------------------------------------

    KRG_SYSTEM_TYPESYSTEM_API bool ReadTypeDescriptorFromJSON( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue, TypeDescriptor& outDesc );
    KRG_SYSTEM_TYPESYSTEM_API void WriteTypeDescriptorToJSON( TypeRegistry const& typeRegistry, RapidJsonWriter& writer, TypeDescriptor const& type );

    // Type Instances
    //-------------------------------------------------------------------------

    // Read the data for a native type from JSON - expect a fully created type to be supplied and will override the values
    KRG_SYSTEM_TYPESYSTEM_API bool ReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue, IRegisteredType* pTypeInstance );

    // Read the data for a native type from JSON - expect a fully created type to be supplied and will override the values
    KRG_SYSTEM_TYPESYSTEM_API bool ReadNativeTypeFromString( TypeRegistry const& typeRegistry, String const& jsonString, IRegisteredType* pTypeInstance );

    // Write the property data for a supplied native type to JSON
    KRG_SYSTEM_TYPESYSTEM_API void WriteNativeType( TypeRegistry const& typeRegistry, IRegisteredType const* pTypeInstance, RapidJsonWriter& writer );

    // Write the property data for a supplied native type to JSON
    KRG_SYSTEM_TYPESYSTEM_API void WriteNativeTypeToString( TypeRegistry const& typeRegistry, IRegisteredType const* pTypeInstance, String& outString );

    // Create a new instance of a type from a supplied JSON version
    KRG_SYSTEM_TYPESYSTEM_API IRegisteredType* CreateAndReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue );

    // Create a new instance of a type from a supplied JSON version
    template<typename T>
    T* CreateAndReadNativeType( TypeRegistry const& typeRegistry, RapidJsonValue const& typeObjectValue )
    {
        auto pCreatedType = CreateAndReadNativeType( typeRegistry, typeObjectValue );
        return Cast<T>( pCreatedType );
    }

    //-------------------------------------------------------------------------
    // Native Type Serialization : Reading
    //-------------------------------------------------------------------------
    // Supports multiple compound types in a single archive
    // An archive is either a single serialized type or an array of serialized types
    // Each type is serialized as a JSON object with a 'TypeID' property containing the type ID of the serialized type

    class KRG_SYSTEM_TYPESYSTEM_API TypeReader : public JsonReader
    {
    public:

        TypeReader( TypeRegistry const& typeRegistry );

        // Get number of types serialized in the read json file
        inline int32 GetNumSerializedTypes() const { return m_numSerializedTypes; }

        // Descriptor
        //-------------------------------------------------------------------------

        inline bool ReadType( TypeDescriptor& typeDesc )
        {
            return ReadTypeDescriptorFromJSON( m_typeRegistry, GetObjectValueToBeDeserialized(), typeDesc );
        }

        inline TypeReader const& operator>>( TypeDescriptor& typeDesc )
        {
            bool const result = ReadType( typeDesc );
            KRG_ASSERT( result );
            return *this;
        }

        // Native
        //-------------------------------------------------------------------------
        // Do not try to serialize core-types using this reader

        inline bool ReadType( IRegisteredType* pType )
        {
            return ReadNativeType( m_typeRegistry, GetObjectValueToBeDeserialized(), pType );
        }

        inline TypeReader const& operator>>( IRegisteredType* pType )
        {
            bool const result = ReadType( pType );
            KRG_ASSERT( result );
            return *this;
        }

        inline TypeReader const& operator>>( IRegisteredType& type )
        {
            bool const result = ReadType( &type );
            KRG_ASSERT( result );
            return *this;
        }

    private:

        virtual void Reset() override final;
        virtual void OnFileReadSuccess() override final;

        RapidJsonValue const& GetObjectValueToBeDeserialized();

    private:

        TypeRegistry const&                                         m_typeRegistry;
        int32                                                       m_numSerializedTypes = 0;
        int32                                                       m_deserializedTypeIdx = 0;
    };

    //-------------------------------------------------------------------------
    // // Native Type Serialization : Writing
    //-------------------------------------------------------------------------
    // Supports multiple compound types in a single archive
    // An archive is either a single serialized type or an array of serialized types
    // Each type is serialized as a JSON object with a 'TypeID' property containing the type ID of the serialized type

    class KRG_SYSTEM_TYPESYSTEM_API TypeWriter final : public JsonWriter
    {
    public:

        TypeWriter( TypeRegistry const& typeRegistry );

        // Reset all serialized data without writing to disk
        void Reset() override;

        // Native
        //-------------------------------------------------------------------------
        // Do not try to serialize core-types using this writer

        template<typename T>
        inline TypeWriter& operator<<( T const* pType )
        {
            PreSerializeType();
            WriteNativeType( m_typeRegistry, pType, m_writer );
            m_numTypesSerialized++;
            return *this;
        }

        // Descriptor
        //-------------------------------------------------------------------------

        inline TypeWriter& operator<< ( TypeDescriptor const& typeDesc )
        {
            PreSerializeType();
            WriteTypeDescriptorToJSON( m_typeRegistry, m_writer, typeDesc );
            m_numTypesSerialized++;
            return *this;
        }

    private:

        using JsonWriter::GetWriter;

        void PreSerializeType();
        virtual void FinalizeSerializedData() override final;

    private:

        TypeRegistry const&                                         m_typeRegistry;
        int32                                                       m_numTypesSerialized = 0;
    };
}
#endif