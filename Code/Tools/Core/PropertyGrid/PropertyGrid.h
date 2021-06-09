#pragma once

#include "Tools/Core/_Module/API.h"
#include "PropertyGridEditors.h"
#include "System/DevTools/ImguiX.h"
#include "System/TypeSystem/TypeInfo.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------
// Property grid
//-------------------------------------------------------------------------
// Allows you to edit registered types

namespace KRG
{
    class KRG_TOOLS_CORE_API PropertyGrid
    {
    public:

        PropertyGrid( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataPath );

        // Set the type instance to edit, will reset dirty status
        inline void SetTypeToEdit( IRegisteredType* pTypeInstance )
        {
            if ( pTypeInstance == m_pTypeInstance )
            {
                return;
            }

            KRG_ASSERT( pTypeInstance != nullptr );
            m_pTypeInfo = pTypeInstance->GetTypeInfo();
            m_pTypeInstance = pTypeInstance;
            m_isDirty = false;
        }

        // Set the type instance to edit, will reset dirty status
        inline void SetTypeToEdit( nullptr_t )
        {
            m_pTypeInfo = nullptr;
            m_pTypeInstance = nullptr;
            m_isDirty = false;
        }

        // Display the grid
        void DrawGrid();

        // Has the type been modified
        inline bool IsDirty() const { return m_isDirty; }

        // Clear the modified flag i.e. when changes have been committed/saved
        void ClearDirty() { m_isDirty = false; }

    private:

        void DrawPropertyRow( TypeSystem::TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance );
        void DrawValuePropertyRow( TypeSystem::TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance, int32 arrayIdx = InvalidIndex );
        void DrawArrayPropertyRow( TypeSystem::TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance );

        void PreChange();
        void PostChange();

    private:

        TypeSystem::TypeRegistry const&             m_typeRegistry;
        FileSystem::Path const                      m_sourceDataPath;
        PG::Context                                 m_context;
        TypeSystem::TypeInfo const*                 m_pTypeInfo = nullptr;
        IRegisteredType*                            m_pTypeInstance = nullptr;
        bool                                        m_isDirty = false;
    };
}