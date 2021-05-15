#pragma once

#include "_Module/API.h"
#include "System/TypeSystem/PropertyPath.h"
#include "System/TypeSystem/PropertyInfo.h"
#include "System/TypeSystem/TypeDescriptors.h"
#include "System/TypeSystem/EnumInfo.h"
#include "System/TypeSystem/TypeInfo.h"
#include "System/TypeSystem/CoreTypeIDs.h"
#include "System/TypeSystem/CoreTypeConversions.h"

//-------------------------------------------------------------------------
// Tool Type Instance
//-------------------------------------------------------------------------
// These are an instance of a dynamic type as defined by the reflection system in KRG
// These instances are used for authoring and editing native KRG types in the tools (i.e. property grid)

namespace KRG::TypeSystem
{
    class TypeRegistry;

    //-------------------------------------------------------------------------
    // Tool Property Instance
    //-------------------------------------------------------------------------
    // Primary workhorse, access to values, children, defaults, type info, etc for a instance of a type
    // Technically types can be properties too but we want to keep the two separate for simplicities sake

    class KRG_TOOLS_ENTITY_API PropertyInstanceModel
    {
    public:

        // Default and property only constructors, type constructors are private
        PropertyInstanceModel() = default;
        PropertyInstanceModel( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo );
        PropertyInstanceModel( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, String const& stringValue );

        virtual ~PropertyInstanceModel() {}

        // Info
        //-------------------------------------------------------------------------

        virtual bool IsValid() const { return m_pTypeRegistry != nullptr && m_propertyInfo.IsValid(); }
        inline StringID const& GetID() const { return m_propertyInfo.m_ID; }
        inline char const* GetName() const { return m_propertyInfo.m_ID.c_str(); }
        inline char const* GetFriendlyName() const { return m_friendlyName.c_str(); }
        inline char const* GetFriendlyTypeName() const { return m_friendlyTypeName.c_str(); }

        // Type Info
        //-------------------------------------------------------------------------

        inline TypeID const& GetTypeID() const { return m_propertyInfo.m_typeID; }
        inline TypeID const& GetTemplatedArgumentTypeID() const { return m_propertyInfo.m_templateArgumentTypeID; }
        inline PropertyInfo const& GetPropertyInfo() const { return m_propertyInfo; }
        inline TypeInfo const* GetTypeInfo() const { return m_pTypeInfo; }

        inline bool IsEnum() const { return m_propertyInfo.IsEnumProperty(); }
        inline bool IsBitFlags() const { return m_propertyInfo.IsBitFlagsProperty(); }
        inline bool IsCoreType() const { return TypeSystem::IsCoreType( m_propertyInfo.m_typeID ); }
        inline bool IsStructure() const { return m_pTypeInfo != nullptr; }
        inline bool IsResourcePtr() const { return m_propertyInfo.m_typeID == CoreTypes::ResourcePtr || m_propertyInfo.m_typeID == CoreTypes::TResourcePtr; }

        // Property Access
        //-------------------------------------------------------------------------

        inline bool HasProperties() const { return !m_childProperties.empty(); }
        inline int32 GetNumProperties() const { return (int32) m_childProperties.size(); }
        inline TVector<PropertyInstanceModel>& GetProperties() { return m_childProperties; }
        inline TVector<PropertyInstanceModel> const& GetProperties() const { return m_childProperties; }

        PropertyInstanceModel* GetProperty( PropertyPath const& path, bool allowDynamicArrayElementCreation = false );
        inline PropertyInstanceModel const* GetProperty( PropertyPath const& path ) const { return const_cast<PropertyInstanceModel*>( this )->GetProperty( path ); }

        PropertyInstanceModel* GetProperty( StringID propertyID );
        inline PropertyInstanceModel const* GetProperty( StringID propertyID ) const { return const_cast<PropertyInstanceModel*>( this )->GetProperty( propertyID ); }

        // Enums
        //-------------------------------------------------------------------------

        EnumInfo const* GetEnumInfo() const;
        int64 GetEnumValue() const { return GetIntForEnumStringValue( m_value.c_str() ); }
        int32 GetNumEnumValues() const;
        TVector<char const*> GetEnumValues() const;
        int64 GetIntForEnumStringValue( char const* pString ) const;
        char const* GetStringValueForEnumValue( int64 value ) const;

        // Structures
        //-------------------------------------------------------------------------

        inline TypeID GetStructureTypeID() const
        {
            KRG_ASSERT( IsStructure() );
            return m_pTypeInfo->m_ID;
        }

        inline char const* GetStructureTypeName() const
        {
            KRG_ASSERT( IsStructure() );
            return m_pTypeInfo->GetTypeName();
        }

        // Resource values
        //-------------------------------------------------------------------------

        ResourceTypeID GetResourceTypeIDForResourceProperty() const;

        // Arrays
        //-------------------------------------------------------------------------

        inline bool IsArray() const { return m_propertyInfo.IsArrayProperty(); }
        inline bool IsStaticArray() const{ return m_propertyInfo.IsStaticArrayProperty(); }
        inline bool IsDynamicArray() const { return m_propertyInfo.IsDynamicArrayProperty(); }
        inline bool IsArrayElement() const { return m_arrayElementIdx != InvalidIndex; }

        inline int32 GetArrayElementIndex() const
        {
            KRG_ASSERT( m_arrayElementIdx != InvalidIndex );
            return m_arrayElementIdx;
        }

        inline int32 GetNumArrayElements() const
        {
            KRG_ASSERT( IsArray() );
            return (int32) m_childProperties.size();
        }

        inline int32 GetNumDefaultArrayElements() const
        {
            KRG_ASSERT( IsArray() );
            return m_propertyInfo.m_arraySize;
        }

        inline bool IsValidArrayElementIndex( int32 elementIdx ) const
        {
            KRG_ASSERT( IsArray() );
            return elementIdx >= 0 && elementIdx < (int32) m_childProperties.size();
        }

        inline PropertyInstanceModel& GetArrayElement( int32 elementIdx )
        {
            KRG_ASSERT( IsArray() );
            return m_childProperties[elementIdx];
        }

        inline PropertyInstanceModel const& GetArrayElement( int32 elementIdx ) const
        {
            KRG_ASSERT( IsArray() );
            return m_childProperties[elementIdx];
        }

        void SetNumArrayElements( int32 numElements );

        inline void AddArrayElement()
        {
            KRG_ASSERT( IsDynamicArray() );
            AddArrayElementInternal();
        }

        void RemoveArrayElement( int32 elementIdx );

        // Values
        //-------------------------------------------------------------------------

        inline bool IsValueSet() const
        {
            KRG_ASSERT( ( IsCoreType() || IsEnum() ) && !IsArray() );
            return !m_value.empty();
        }

        inline String const& GetStringValue() const
        {
            KRG_ASSERT( ( IsCoreType() || IsEnum() ) && !IsArray() );
            return m_value;
        }

        void SetStringValue( String const& stringValue );

        template<typename T>
        inline T GetValue() const
        {
            KRG_ASSERT( TypeSystem::ValidateTypeAgainstTypeID<T>( m_propertyInfo.m_typeID ) );
            KRG_ASSERT( IsCoreType() && !IsArray() );

            T outValue;
            Conversion::ConvertStringToNativeType( *m_pTypeRegistry, m_propertyInfo, m_value, &outValue );
            return outValue;
        }

        template<typename T>
        inline void SetValue( T const& value )
        {
            KRG_ASSERT( TypeSystem::ValidateTypeAgainstTypeID<T>( m_propertyInfo.m_typeID ) );
            KRG_ASSERT( IsCoreType() && !IsArray() );
            Conversion::ConvertNativeTypeToString( *m_pTypeRegistry, m_propertyInfo, &value, m_value );
        }

        template<typename T>
        inline T GetDefaultValue() const
        {
            KRG_ASSERT( IsCoreType() && !IsArray() );
            return *reinterpret_cast<T const*>( m_propertyInfo.m_pDefaultValue );
        }

        bool IsDefaultValue() const;
        void ResetToDefaultValue();

    protected:

        PropertyInstanceModel( TypeRegistry const& typeRegistry, TypeInfo const* pTypeInfo );

    private:

        void CreateDefaultArrayElements();
        void AddArrayElementInternal();

    protected:

        TypeRegistry const*                                 m_pTypeRegistry = nullptr;
        PropertyInfo                                        m_propertyInfo; // Only valid for actual properties
        TypeInfo const*                                     m_pTypeInfo = nullptr;  // If this is a struct/class instance, we stored the type info here
        int32                                               m_arrayElementIdx = InvalidIndex; // Only set if this is an element in an array
        String                                              m_friendlyName;
        String                                              m_friendlyTypeName;
        String                                              m_value;
        String                                              m_defaultValue;
        TVector<PropertyInstanceModel>                      m_childProperties;
    };

    //-------------------------------------------------------------------------
    // Tool Type Instance
    //-------------------------------------------------------------------------

    class KRG_TOOLS_ENTITY_API TypeInstanceModel final : private PropertyInstanceModel
    {

    public:

        TypeInstanceModel() = default;
        TypeInstanceModel( TypeRegistry const& typeRegistry, TypeInfo const* pTypeInfo );
        TypeInstanceModel( TypeRegistry const& typeRegistry, TypeDescriptor const& typeDescriptor );

        virtual bool IsValid() const final { return m_pTypeInfo != nullptr && m_pTypeInfo->m_ID.IsValid(); }
        inline TypeID const& GetTypeID() const { return m_pTypeInfo->m_ID; }

        // Create a type descriptor of this instance for easy serialization
        TypeDescriptor GetDescriptor() const;

        // Get as a property instance - needed in some tooling to track hierarchy
        PropertyInstanceModel const* GetAsPropertyInstance() const { return static_cast<PropertyInstanceModel const*>( this ); }

        // Get as a property instance - needed in some tooling to track hierarchy
        PropertyInstanceModel* GetAsPropertyInstance() { return static_cast<PropertyInstanceModel*>( this ); }

        //-------------------------------------------------------------------------

        using PropertyInstanceModel::GetFriendlyTypeName;

        using PropertyInstanceModel::GetTypeInfo;
        using PropertyInstanceModel::GetProperty;

        using PropertyInstanceModel::HasProperties;
        using PropertyInstanceModel::GetProperties;
    };
}