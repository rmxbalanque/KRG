#pragma once

#include "EnumInfo.h"
#include "TypeInfo.h"
#include "CoreTypeIDs.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        class PropertyPath;

        //-------------------------------------------------------------------------

        class KRG_SYSTEM_TYPESYSTEM_API TypeRegistry : public ISystem
        {
        public:

            KRG_SYSTEM_ID( TypeRegistry );

        public:

            ~TypeRegistry();

            //-------------------------------------------------------------------------
            // Type Registration
            //-------------------------------------------------------------------------

            TypeInfo const* RegisterType( TypeInfo const& type );
            void UnregisterType( TypeID typeID );

            //-------------------------------------------------------------------------
            // Type Info
            //-------------------------------------------------------------------------

            // Returns the type information for a given type ID
            TypeInfo const* GetTypeInfo( TypeID typeID ) const;

            // Returns the size of a given type
            size_t GetTypeByteSize( TypeID typeID ) const;

            // Returns the resolved property info for a given path as well as the resolved byte offset from the parent type
            ResolvedPropertyInfo ResolvePropertyPath( TypeInfo const* pTypeInfo, PropertyPath const& pathID ) const;

            bool IsTypeDerivedFrom( TypeID typeID, TypeID parentTypeID ) const;

            TVector<TypeInfo const*> GetAllTypesWithMatchingMetadata( TBitFlags<ETypeInfoMetaData> metadataFlags ) const;

            //-------------------------------------------------------------------------
            // Enums
            //-------------------------------------------------------------------------

            EnumInfo const* RegisterEnum( EnumInfo const& type );
            void UnregisterEnum( TypeID typeID );
            EnumInfo const* GetEnumInfo( TypeID enumID ) const;

            //-------------------------------------------------------------------------
            // Resources
            //-------------------------------------------------------------------------

            inline THashMap<TypeID, ResourceTypeID> const& GetRegisteredResourceTypes() const { return m_registeredResourceTypes; }
            void RegisterResourceTypeID( TypeID typeID, ResourceTypeID resourceTypeID );
            void UnregisterResourceTypeID( TypeID typeID );
            ResourceTypeID GetResourceTypeIDForType( TypeID typeID ) const;

        private:

            THashMap<TypeID, TypeInfo*>             m_registeredTypes;
            THashMap<TypeID, EnumInfo*>             m_registeredEnums;
            THashMap<TypeID, ResourceTypeID>        m_registeredResourceTypes;
        };
    }
}