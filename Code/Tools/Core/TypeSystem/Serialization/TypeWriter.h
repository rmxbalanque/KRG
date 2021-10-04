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
    class KRG_TOOLS_CORE_API TypeWriter final : public JsonWriter
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
            WriteNativeType( m_typeRegistry, m_writer, pType );
            m_numTypesSerialized++;
            return *this;
        }

        // Descriptor
        //-------------------------------------------------------------------------

        inline TypeWriter& operator<< ( TypeDescriptor const& typeDesc )
        {
            PreSerializeType();
            WriteTypeDescriptor( m_typeRegistry, m_writer, typeDesc );
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