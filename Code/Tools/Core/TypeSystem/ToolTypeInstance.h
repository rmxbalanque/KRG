#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/TypeSystem/PropertyPath.h"
#include "System/TypeSystem/PropertyInfo.h"
#include "System/TypeSystem/TypeInfo.h"
#include "System/TypeSystem/CoreTypeIDs.h"
#include "System/TypeSystem/TypeValueConverter.h"

//-------------------------------------------------------------------------
// Tool Type Instance
//-------------------------------------------------------------------------
// These are an instance of a dynamic type as defined by the reflection system in KRG
// These instances are used for authoring and editing native KRG types in the tools (i.e. property grid)

namespace KRG::TypeSystem
{
    class TypeRegistry;

    //-------------------------------------------------------------------------
    // Simplified representation of a tool type instance
    //-------------------------------------------------------------------------
    // Often used to directly serialize/deserialize a type instance

    struct ToolPropertyInstanceDescriptor
    {
        ToolPropertyInstanceDescriptor( PropertyPath const& path, String const& value, TypeID typeID = TypeID() )
            : m_path( path )
            , m_value( value )
            , m_typeID( typeID )
        {
            KRG_ASSERT( path.IsValid() && !value.empty() );
        }

        inline bool IsOnlyRelevantForTools() const { return !m_typeID.IsValid(); }

    public:

        PropertyPath                                    m_path;
        String                                          m_value;
        TypeID                                          m_typeID; // Not serialized - but needed for certain conversions
    };

    //-------------------------------------------------------------------------

    struct ToolTypeInstanceDescriptor
    {
        TypeID                                          m_typeID;
        TVector<ToolPropertyInstanceDescriptor>                     m_properties;
    };

    //-------------------------------------------------------------------------
    // Tool Property Instance
    //-------------------------------------------------------------------------
    // Primary workhorse, access to values, children, defaults, type info, etc for a instance of a type
    // Technically types can be properties too but we want to keep the two separate for simplicities sake

    class KRG_TOOLS_CORE_API ToolPropertyInstance
    {
    public:

        // Default and property only constructors, type constructors are private
        ToolPropertyInstance() = default;
        ToolPropertyInstance( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo );
        ToolPropertyInstance( TypeRegistry const& typeRegistry, PropertyInfo const& propertyInfo, String const& stringValue );

        // Info
        //-------------------------------------------------------------------------

        virtual bool IsValid() const { return m_pTypeRegistry != nullptr && m_propertyInfo.IsValid(); }
        inline StringID const& GetID() const { return m_propertyInfo.m_ID; }
        inline char const* GetFriendlyName() const { return m_friendlyName.c_str(); }

        // Type Info
        //-------------------------------------------------------------------------

        inline TypeID const& GetTypeID() const { return m_propertyInfo.m_typeID; }
        inline PropertyInfo const& GetPropertyInfo() const { return m_propertyInfo; }
        inline TypeInfo const* GetTypeInfo() const { return m_pTypeInfo; }

        inline bool IsEnum() const { return m_propertyInfo.IsEnumProperty(); }
        inline bool IsCoreType() const { return TypeSystem::IsCoreType( m_propertyInfo.m_typeID ); }
        inline bool IsStructure() const { return m_pTypeInfo != nullptr; }
        inline bool IsResourcePtr() const { return m_propertyInfo.m_typeID == CoreTypes::ResourcePtr || m_propertyInfo.m_typeID == CoreTypes::TResourcePtr; }

        // Property Access
        //-------------------------------------------------------------------------

        inline bool HasProperties() const { return !m_childProperties.empty(); }
        inline S32 GetNumProperties() const { return (S32) m_childProperties.size(); }
        inline TVector<ToolPropertyInstance>& GetProperties() { return m_childProperties; }
        inline TVector<ToolPropertyInstance> const& GetProperties() const { return m_childProperties; }

        ToolPropertyInstance* GetProperty( PropertyPath const& path, bool allowDynamicArrayElementCreation = false );
        inline ToolPropertyInstance const* GetProperty( PropertyPath const& path ) const { return const_cast<ToolPropertyInstance*>( this )->GetProperty( path ); }

        ToolPropertyInstance* GetProperty( StringID propertyID );
        inline ToolPropertyInstance const* GetProperty( StringID propertyID ) const { return const_cast<ToolPropertyInstance*>( this )->GetProperty( propertyID ); }

        // Enums
        //-------------------------------------------------------------------------

        TVector<char const*> GetEnumValues() const;
        bool IsValidEnumStringValue( char const* pString );
        S64 GetIntForEnumStringValue( char const* pString );
        char const* GetStringValueForEnumValue( S64 value );

        // Resource values
        //-------------------------------------------------------------------------

        ResourceTypeID GetResourceTypeIDForResourceProperty() const;

        // Arrays
        //-------------------------------------------------------------------------

        inline bool IsArray() const { return m_propertyInfo.IsArrayProperty(); }
        inline bool IsStaticArray() const{ return m_propertyInfo.IsStaticArrayProperty(); }
        inline bool IsDynamicArray() const { return m_propertyInfo.IsDynamicArrayProperty(); }
        inline bool IsArrayElement() const { return m_arrayElementIdx != InvalidIndex; }

        inline S32 GetArrayElementIndex() const
        {
            KRG_ASSERT( m_arrayElementIdx != InvalidIndex );
            return m_arrayElementIdx;
        }

        inline S32 GetNumArrayElements() const
        {
            KRG_ASSERT( IsArray() );
            return (S32) m_childProperties.size();
        }

        inline S32 GetNumDefaultArrayElements() const
        {
            KRG_ASSERT( IsArray() );
            return m_propertyInfo.m_arraySize;
        }

        inline bool IsValidArrayElementIndex( S32 elementIdx ) const
        {
            KRG_ASSERT( IsArray() );
            return elementIdx >= 0 && elementIdx < (S32) m_childProperties.size();
        }

        inline ToolPropertyInstance& GetArrayElement( S32 elementIdx )
        {
            KRG_ASSERT( IsArray() );
            return m_childProperties[elementIdx];
        }

        inline ToolPropertyInstance const& GetArrayElement( S32 elementIdx ) const
        {
            KRG_ASSERT( IsArray() );
            return m_childProperties[elementIdx];
        }

        void SetNumArrayElements( S32 numElements );

        inline void AddArrayElement()
        {
            KRG_ASSERT( IsDynamicArray() );
            AddArrayElementInternal();
        }

        void RemoveArrayElement( S32 elementIdx );

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
            KRG_ASSERT( IsCoreType() && !IsArray() );

            T outValue;
            TypeValueConverter::StringToValue( m_value, outValue );
            return outValue;
        }

        template<typename T>
        inline void SetValue( T const& value )
        {
            KRG_ASSERT( IsCoreType() && !IsArray() );
            TypeValueConverter::ValueToString( value, m_value );
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

        ToolPropertyInstance( TypeRegistry const& typeRegistry, TypeInfo const* pTypeInfo );

    private:

        void CreateDefaultArrayElements();
        void AddArrayElementInternal();

    protected:

        TypeRegistry const*                             m_pTypeRegistry = nullptr;
        PropertyInfo                                    m_propertyInfo; // Only valid for actual properties
        TypeInfo const*                                 m_pTypeInfo = nullptr;  // If this is a struct/class instance, we stored the type info here
        S32                                                         m_arrayElementIdx = InvalidIndex; // Only set if this is an element in an array
        String                                                      m_friendlyName;
        String                                                      m_value;
        String                                                      m_defaultValue;
        TVector<ToolPropertyInstance>                               m_childProperties;
    };

    //-------------------------------------------------------------------------
    // Tool Type Instance
    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API ToolTypeInstance final : private ToolPropertyInstance
    {
    public:

        ToolTypeInstance() = default;
        ToolTypeInstance( TypeRegistry const& typeRegistry, TypeInfo const* pTypeInfo );
        ToolTypeInstance( TypeRegistry const& typeRegistry, ToolTypeInstanceDescriptor const& typeDescriptor );

        virtual bool IsValid() const final { return m_pTypeInfo != nullptr && m_pTypeInfo->m_ID.IsValid(); }
        inline TypeID const& GetTypeID() const { return m_pTypeInfo->m_ID; }

        // Create a simplified descriptor of this type instance for easy serialization
        ToolTypeInstanceDescriptor GetDescriptor() const;

        using ToolPropertyInstance::GetTypeInfo;
        using ToolPropertyInstance::GetProperty;

        using ToolPropertyInstance::HasProperties;
        using ToolPropertyInstance::GetProperties;
    };
}