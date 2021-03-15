#pragma once

#include "_Module/API.h"
#include "PropertyInfo.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class TypeRegistry;
}

//-------------------------------------------------------------------------
// Core Type Serialization
//-------------------------------------------------------------------------
// Basic serialization of core types to/from string and binary representations
// Primarily used to serialize core type properties
// Each core type needs to implement a serializer function specializations
// This also handles enums and bitflags as core types

namespace KRG::TypeSystem::Conversion
{
    KRG_SYSTEM_TYPESYSTEM_API void ConvertStringValueToTypeValue( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue, void* pValue );
    KRG_SYSTEM_TYPESYSTEM_API void ConvertTypeValueToStringValue( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, void const* pValue, String& strValue );
    KRG_SYSTEM_TYPESYSTEM_API void ConvertBinaryToTypeValue( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, TVector<Byte> const& byteArray, void* pValue );
    KRG_SYSTEM_TYPESYSTEM_API void ConvertTypeValueToBinary( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, void const* pValue, TVector<Byte>& byteArray );
    KRG_SYSTEM_TYPESYSTEM_API void ConvertStringValueToBinary( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue, TVector<Byte>& byteArray );
    KRG_SYSTEM_TYPESYSTEM_API bool IsValidStringValueForType( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue );

    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE void ConvertStringValueToTypeValue( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, String const& strValue, void* pValue )
    {
        ConvertStringValueToTypeValue( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, strValue, pValue );
    }

    KRG_FORCE_INLINE void ConvertTypeValueToStringValue( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, void const* pValue, String& strValue )
    {
        ConvertTypeValueToStringValue( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, pValue, strValue );
    }

    KRG_FORCE_INLINE void ConvertBinaryToTypeValue( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, TVector<Byte> const& byteArray, void* pValue )
    {
        ConvertBinaryToTypeValue( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, byteArray, pValue );
    }

    KRG_FORCE_INLINE void ConvertTypeValueToBinary( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, void const* pValue, TVector<Byte>& byteArray )
    {
        ConvertTypeValueToBinary( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, pValue, byteArray );
    }

    KRG_FORCE_INLINE void ConvertStringValueToBinary( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, String const& strValue, TVector<Byte>& byteArray )
    {
        ConvertStringValueToBinary( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, strValue, byteArray );
    }

    KRG_FORCE_INLINE bool IsValidStringValueForType( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, String const& strValue )
    {
        return IsValidStringValueForType( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, strValue );
    }
}