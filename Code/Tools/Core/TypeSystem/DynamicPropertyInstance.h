#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/CoreTypeIDs.h"
#include "System/TypeSystem/TypeValueConverter.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_TOOLS_CORE_API DynamicPropertyInstance
    {
    public:

        DynamicPropertyInstance() = default;

        DynamicPropertyInstance( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::PropertyInfo const& propertyInfo );

        DynamicPropertyInstance( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::PropertyInfo const& propertyInfo, String const& stringValue );

        // Info
        //-------------------------------------------------------------------------

        virtual bool IsValid() const
        {
            return m_pTypeRegistry != nullptr && m_propertyInfo.IsValid();
        }

        inline StringID const& GetID() const
        {
            return m_propertyInfo.m_ID;
        }

        inline char const* GetFriendlyName() const
        {
            return m_friendlyName.c_str();
        }

        // Type Info
        //-------------------------------------------------------------------------

        inline TypeSystem::TypeID const& GetTypeID() const
        {
            return m_propertyInfo.m_typeID;
        }

        inline bool IsEnumProperty() const
        {
            return m_propertyInfo.IsEnumProperty();
        }

        inline bool IsCoreTypeProperty() const
        {
            return TypeSystem::IsCoreType( m_propertyInfo.m_typeID );
        }

        inline bool IsStructureProperty() const
        {
            return m_pTypeInfo != nullptr;
        }

        inline bool IsTypeInstanceProperty() const
        {
            return !TypeSystem::IsCoreType( m_propertyInfo.m_typeID );
        }

        inline bool IsResourcePtrProperty() const
        {
            return m_propertyInfo.m_typeID == TypeSystem::CoreTypes::ResourcePtr || m_propertyInfo.m_typeID == TypeSystem::CoreTypes::TResourcePtr;
        }

        inline ResourceTypeID GetResourceTypeIDForResourceProperty() const
        {
            KRG_ASSERT( IsResourcePtrProperty() || m_propertyInfo.m_typeID == TypeSystem::CoreTypes::ResourceID );

            ResourceTypeID resourceTypeID;
            if ( m_propertyInfo.m_typeID == TypeSystem::CoreTypes::TResourcePtr )
            {
                resourceTypeID = m_pTypeRegistry->GetResourceTypeIDForType( m_propertyInfo.m_templateArgumentTypeID );
            }

            return resourceTypeID;
        }

        inline TypeSystem::PropertyInfo const& GetPropertyInfo() const
        {
            return m_propertyInfo;
        }

        inline TypeSystem::TypeInfo const* GetTypeInfo() const
        {
            return m_pTypeInfo;
        }

        // Property Access
        //-------------------------------------------------------------------------

        inline bool HasProperties() const
        {
            return !m_childProperties.empty();
        }

        inline S32 GetNumProperties() const
        {
            return (S32) m_childProperties.size();
        }

        inline TVector<DynamicPropertyInstance>& GetProperties()
        {
            return m_childProperties;
        }

        inline TVector<DynamicPropertyInstance> const& GetProperties() const
        {
            return m_childProperties;
        }

        inline DynamicPropertyInstance const* GetProperty( StringID propertyID ) const
        {
            KRG_ASSERT( !m_propertyInfo.IsArrayProperty() );

            for ( auto& childProperty : m_childProperties )
            {
                if ( childProperty.GetID() == propertyID )
                {
                    return &childProperty;
                }
            }

            return nullptr;
        }

        inline DynamicPropertyInstance* GetProperty( StringID propertyID )
        {
            return const_cast<DynamicPropertyInstance*>( const_cast<DynamicPropertyInstance const*>( this )->GetProperty( propertyID ) );
        }

        DynamicPropertyInstance* GetProperty( TypeSystem::PropertyPath const& path, bool allowDynamicArrayElementCreation = false );

        inline DynamicPropertyInstance const* GetProperty( TypeSystem::PropertyPath const& path ) const
        {
            return const_cast<DynamicPropertyInstance*>( this )->GetProperty( path );
        }

        // Enums
        //-------------------------------------------------------------------------

        TVector<char const*> GetEnumValues() const;

        bool IsValidEnumStringValue( char const* pString );

        S64 GetIntForEnumStringValue( char const* pString );

        char const* GetStringForEnumValue( S64 value );

        // Arrays
        //-------------------------------------------------------------------------

        inline bool IsArrayProperty() const
        {
            return m_propertyInfo.IsArrayProperty();
        }

        inline bool IsStaticArrayProperty() const
        {
            return m_propertyInfo.IsStaticArrayProperty();
        }

        inline bool IsDynamicArrayProperty() const
        {
            return m_propertyInfo.IsDynamicArrayProperty();
        }

        inline bool IsArrayElementProperty() const
        {
            return m_arrayElementIdx != InvalidIndex;
        }

        inline S32 GetArrayElementIndex() const
        {
            KRG_ASSERT( m_arrayElementIdx != InvalidIndex );
            return m_arrayElementIdx;
        }

        inline S32 GetNumArrayElements() const
        {
            KRG_ASSERT( IsArrayProperty() );
            return (S32) m_childProperties.size();
        }

        inline S32 GetNumDefaultArrayElements() const
        {
            KRG_ASSERT( IsArrayProperty() );
            return m_propertyInfo.m_arraySize;
        }

        inline bool IsValidArrayElementIndex( S32 elementIdx ) const
        {
            KRG_ASSERT( IsArrayProperty() );
            return elementIdx >= 0 && elementIdx < (S32) m_childProperties.size();
        }

        inline DynamicPropertyInstance& GetArrayElement( S32 elementIdx )
        {
            KRG_ASSERT( IsArrayProperty() );
            return m_childProperties[elementIdx];
        }

        inline DynamicPropertyInstance const& GetArrayElement( S32 elementIdx ) const
        {
            KRG_ASSERT( IsArrayProperty() );
            return m_childProperties[elementIdx];
        }

        void SetNumArrayElements( S32 numElements );

        inline void AddArrayElement()
        { 
            KRG_ASSERT( IsDynamicArrayProperty() );
            AddArrayElementInternal();
        }

        void RemoveArrayElement( S32 elementIdx );

        // Values
        //-------------------------------------------------------------------------

        inline bool IsValueSet() const
        {
            KRG_ASSERT( ( IsCoreTypeProperty() || IsEnumProperty() ) && !IsArrayProperty() );

            return !m_value.empty();
        }

        inline String const& GetStringValue() const
        {
            KRG_ASSERT( ( IsCoreTypeProperty() || IsEnumProperty() ) && !IsArrayProperty() );

            return m_value;
        }

        inline void SetStringValue( String const& stringValue )
        {
            KRG_ASSERT( !IsStaticArrayProperty() && !IsStructureProperty() );

            if ( IsDynamicArrayProperty() )
            {
                m_value = stringValue;
                S32 const numElements = StringUtils::StrToS32( stringValue );
                SetNumArrayElements( numElements );
            }
            else if ( IsEnumProperty() )
            {
                IsValidEnumStringValue( stringValue.c_str() );
            }
            else
            {
                KRG_ASSERT( TypeSystem::IsValidStringValueForType( GetTypeID(), stringValue ) );
            }

            m_value = stringValue;
        }

        template<typename T>
        inline T GetValue() const
        {
            KRG_ASSERT( IsCoreTypeProperty() && !IsArrayProperty() );

            T outValue;
            TypeSystem::TypeValueConverter::FromString( m_value, outValue );
            return outValue;
        }

        template<typename T>
        inline T GetDefaultValue() const
        {
            KRG_ASSERT( IsCoreTypeProperty() && !IsArrayProperty() );
            return *reinterpret_cast<T const*>( m_propertyInfo.m_pDefaultValue );
        }

        template<typename T>
        inline void SetValue( T const& value )
        {
            KRG_ASSERT( IsCoreTypeProperty() && !IsArrayProperty() );
            TypeSystem::TypeValueConverter::ToString( value, m_value );
        }

        bool IsDefaultValue() const;

        void ResetToDefaultValue();

    protected:

        DynamicPropertyInstance( TypeSystem::TypeRegistry const& typeRegistry, TypeSystem::TypeInfo const* pTypeInfo );

    private:

        void CreateDefaultArrayElements();
        void AddArrayElementInternal();

    protected:

        TypeSystem::TypeRegistry const*                             m_pTypeRegistry = nullptr;
        TypeSystem::PropertyInfo                                    m_propertyInfo;
        TypeSystem::TypeInfo const*                                 m_pTypeInfo = nullptr;  // If this is not a core type property, we stored the type info here
        S32                                                         m_arrayElementIdx = InvalidIndex; // Only set if this is an element in an array
        String                                                      m_friendlyName;
        String                                                      m_value;
        String                                                      m_defaultValue;
        TVector<DynamicPropertyInstance>                            m_childProperties;
    };
}