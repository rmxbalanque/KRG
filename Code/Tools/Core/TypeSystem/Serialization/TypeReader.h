#pragma once

#include "Tools/Core/_Module/API.h"
#include "TypeSerialization.h"
#include "System/Core/FileSystem/FileSystemPath.h"

// Native type serialization
//-------------------------------------------------------------------------
// Supports multiple compound types in a single archive
// An archive is either a single serialized type or an array of serialized types
// Each type is serialized as a JSON object with a 'TypeID' property containing the type ID of the serialized type

namespace KRG::TypeSystem::Serialization
{
    class KRG_TOOLS_CORE_API TypeReader : public JsonFileReader
    {
    public:

        TypeReader( TypeRegistry const& typeRegistry );

        // Get number of types serialized in the read json file
        inline int32 GetNumSerializedTypes() const { return m_numSerializedTypes; }

        // Descriptor
        //-------------------------------------------------------------------------

        inline bool ReadType( TypeDescriptor& typeDesc )
        {
            return ReadTypeDescriptor( m_typeRegistry, GetObjectValueToBeDeserialized(), typeDesc );
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

        template<typename T>
        inline bool ReadType( T* pType )
        {
            return ReadNativeType( m_typeRegistry, GetObjectValueToBeDeserialized(), T::GetStaticTypeID(), pType );
        }

        template<typename T>
        inline TypeReader const& operator>>( T* pType )
        {
            bool const result = ReadType<T>( pType );
            KRG_ASSERT( result );
            return *this;
        }

        template<typename T>
        inline TypeReader const& operator>>( T& pType )
        {
            bool const result = ReadType<T>( &pType );
            KRG_ASSERT( result );
            return *this;
        }

    private:

        using JsonFileReader::GetDocument;

        virtual void Reset() override final;
        virtual void OnFileReadSuccess() override final;

        rapidjson::Value const& GetObjectValueToBeDeserialized();

    private:

        TypeRegistry const&                                         m_typeRegistry;
        int32                                                       m_numSerializedTypes = 0;
        int32                                                       m_deserializedTypeIdx = 0;
    };
}