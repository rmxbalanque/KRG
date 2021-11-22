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
    KRG_SYSTEM_TYPESYSTEM_API bool ConvertStringToNativeType( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue, void* pValue );
    KRG_SYSTEM_TYPESYSTEM_API bool ConvertNativeTypeToString( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, void const* pValue, String& strValue );
    KRG_SYSTEM_TYPESYSTEM_API bool ConvertBinaryToNativeType( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, TVector<Byte> const& byteArray, void* pValue );
    KRG_SYSTEM_TYPESYSTEM_API bool ConvertNativeTypeToBinary( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, void const* pValue, TVector<Byte>& byteArray );
    KRG_SYSTEM_TYPESYSTEM_API bool ConvertStringToBinary( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue, TVector<Byte>& byteArray );
    KRG_SYSTEM_TYPESYSTEM_API bool IsValidStringValueForType( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue );

    //-------------------------------------------------------------------------

    KRG_FORCE_INLINE bool ConvertStringToNativeType( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, String const& strValue, void* pValue )
    {
        return ConvertStringToNativeType( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, strValue, pValue );
    }

    KRG_FORCE_INLINE bool ConvertNativeTypeToString( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, void const* pValue, String& strValue )
    {
        return ConvertNativeTypeToString( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, pValue, strValue );
    }

    KRG_FORCE_INLINE bool ConvertBinaryToNativeType( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, TVector<Byte> const& byteArray, void* pValue )
    {
        return ConvertBinaryToNativeType( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, byteArray, pValue );
    }

    KRG_FORCE_INLINE bool ConvertNativeTypeToBinary( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, void const* pValue, TVector<Byte>& byteArray )
    {
        return ConvertNativeTypeToBinary( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, pValue, byteArray );
    }

    KRG_FORCE_INLINE bool ConvertStringToBinary( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, String const& strValue, TVector<Byte>& byteArray )
    {
        return ConvertStringToBinary( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, strValue, byteArray );
    }

    KRG_FORCE_INLINE bool IsValidStringValueForType( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, String const& strValue )
    {
        return IsValidStringValueForType( typeRegistry, propertyInfo.m_typeID, propertyInfo.m_templateArgumentTypeID, strValue );
    }

    //-------------------------------------------------------------------------

    // Convert a comma separated string of floats into an array of floats
    KRG_SYSTEM_TYPESYSTEM_API void StringToFloatArray( String const& str, int32 const numFloats, float* pFloats );

    // Convert an array of floats into a comma separated string of floats
    KRG_SYSTEM_TYPESYSTEM_API void FloatArrayToString( float const* pFloats, int32 const numFloats, String& strValue );

    // Convert a comma separated string of ints into an array of ints
    KRG_SYSTEM_TYPESYSTEM_API void StringToIntArray( String const& str, int32 const numInts, int32* pInts );

    // Convert an array of ints into a comma separated string of ints
    KRG_SYSTEM_TYPESYSTEM_API void IntArrayToString( int32 const* pInts, int32 const numInts, String& strValue );
}