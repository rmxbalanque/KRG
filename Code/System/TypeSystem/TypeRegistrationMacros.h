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

#define KRG_REGISTER_TYPE \
        friend KRG::TypeSystem::TypeInfo;\
        template<typename T> friend class KRG::TypeSystem::TypeHelpers::TTypeHelper; \
        public: \
        static KRG::TypeSystem::TypeInfo const* StaticTypeInfo; \
        static KRG::TypeSystem::TypeID GetStaticTypeID() { KRG_ASSERT( StaticTypeInfo != nullptr ); return StaticTypeInfo->m_ID; }

#define KRG_REGISTER_TYPE_WITH_VIRTUAL_TYPE_ACCESSOR( Type ) \
        KRG_REGISTER_TYPE \
        virtual KRG::TypeSystem::TypeInfo const* GetTypeInfo() const { return Type::StaticTypeInfo; }

//-------------------------------------------------------------------------
// Property Registration
//-------------------------------------------------------------------------

#define EXPOSE

//-------------------------------------------------------------------------
// Module Registration
//-------------------------------------------------------------------------

#define KRG_REGISTER_MODULE \
        public: \
        static void RegisterTypes( TypeSystem::TypeRegistry& typeRegistry ); \
        static void UnregisterTypes( TypeSystem::TypeRegistry& typeRegistry );