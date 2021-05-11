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
    class KRG_TOOLS_CORE_API TypeWriter final : public JsonFileWriter
    {
    public:

        TypeWriter( TypeRegistry const& typeRegistry );

        // Reset all serialized data without writing to disk
        void Reset();

        // Native
        //-------------------------------------------------------------------------
        // Do not try to serialize core-types using this writer

        template<typename T>
        inline TypeWriter& operator<<( T const* pType )
        {
            PreSerializeType();
            WriteNativeType( m_typeRegistry, m_writer, T::GetStaticTypeID(), pType );
            m_numTypesSerialized++;
            return *this;
        }

        template<typename T>
        inline TypeWriter& operator<<( T const& pType )
        {
            PreSerializeType();
            WriteNativeType( m_typeRegistry, m_writer, T::GetStaticTypeID(), &pType );
            m_numTypesSerialized++;
            return *this;
        }

        // Descriptor
        //-------------------------------------------------------------------------

        template<>
        inline TypeWriter& operator<< <TypeDescriptor>( TypeDescriptor const& typeDesc )
        {
            PreSerializeType();
            WriteTypeDescriptor( m_typeRegistry, m_writer, typeDesc );
            m_numTypesSerialized++;
            return *this;
        }

        // Type Model
        //-------------------------------------------------------------------------

        template<>
        inline TypeWriter& operator<< <TypeInstanceModel>( TypeInstanceModel const& type )
        {
            PreSerializeType();
            WriteTypeModel( m_typeRegistry, m_writer, type );
            m_numTypesSerialized++;
            return *this;
        }

    private:

        using JsonFileWriter::GetWriter;

        void PreSerializeType();
        virtual void FinalizeSerializedData() override final;

    private:

        TypeRegistry const&                                         m_typeRegistry;
        int32                                                       m_numTypesSerialized = 0;
    };
}