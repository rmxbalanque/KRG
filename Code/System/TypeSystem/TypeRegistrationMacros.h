#pragma once
#include "_Module/API.h"
#include "TypeRegistry.h"

//-------------------------------------------------------------------------
// Prevent Reflection
//-------------------------------------------------------------------------

#define KRG_REFLECTION_IGNORE_HEADER

//-------------------------------------------------------------------------
// Enum Registration
//-------------------------------------------------------------------------

#define KRG_REGISTER_ENUM 

//-------------------------------------------------------------------------
// Type Registration
//-------------------------------------------------------------------------

namespace KRG
{
    // Interface to enforce virtual destructors and type-info overrides
    class KRG_SYSTEM_TYPESYSTEM_API IRegisteredType
    {
        KRG_SERIALIZE_NONE();

    public:

        static KRG::TypeSystem::TypeInfo const* s_pTypeInfo;

    public:

        virtual ~IRegisteredType() = default;
        virtual KRG::TypeSystem::TypeInfo const* GetTypeInfo() const = 0;
        virtual KRG::TypeSystem::TypeID GetTypeID() const = 0;

        #if KRG_DEVELOPMENT_TOOLS
        virtual void PostPropertyEditValidation( TypeSystem::PropertyInfo const* pPropertyEdited ) {}
        #endif
    };
}

#define KRG_REGISTER_TYPE( TypeName ) \
        friend KRG::TypeSystem::TypeInfo;\
        template<typename T> friend class KRG::TypeSystem::TypeHelpers::TTypeHelper; \
        public: \
        static KRG::TypeSystem::TypeInfo const* s_pTypeInfo; \
        static KRG::TypeSystem::TypeID GetStaticTypeID() { KRG_ASSERT( s_pTypeInfo != nullptr ); return s_pTypeInfo->m_ID; }\
        virtual KRG::TypeSystem::TypeInfo const* GetTypeInfo() const override { KRG_ASSERT( TypeName::s_pTypeInfo != nullptr ); return TypeName::s_pTypeInfo; }\
        virtual KRG::TypeSystem::TypeID GetTypeID() const override  { KRG_ASSERT( TypeName::s_pTypeInfo != nullptr ); return TypeName::s_pTypeInfo->m_ID; }

#define KRG_REGISTER_TYPE_RESOURCE( ResourceTypeFourCC, ResourceFriendlyName, TypeName ) \
        KRG_REGISTER_RESOURCE( ResourceTypeFourCC, ResourceFriendlyName )\
        KRG_REGISTER_TYPE( TypeName )

//-------------------------------------------------------------------------
// Property Registration
//-------------------------------------------------------------------------
// TODO: extend clang so that we can use attributes instead of this macro shit

// Register this property but dont expose it to the property grid/editors
#define KRG_REGISTER

// Register this property and expose it to the property grid/editors
#define KRG_EXPOSE

//-------------------------------------------------------------------------
// Module Registration
//-------------------------------------------------------------------------

#define KRG_REGISTER_MODULE \
        public: \
        static void RegisterTypes( TypeSystem::TypeRegistry& typeRegistry ); \
        static void UnregisterTypes( TypeSystem::TypeRegistry& typeRegistry );