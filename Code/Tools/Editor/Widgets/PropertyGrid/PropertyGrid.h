#pragma once

#include "Tools/Editor/_Module/API.h"
#include "PropertyGridEditors.h"
#include "System/DevTools/ImguiX.h"
#include "System/TypeSystem/TypeInfo.h"

//-------------------------------------------------------------------------

namespace KRG { class EditorModel; }

//-------------------------------------------------------------------------
// Property grid
//-------------------------------------------------------------------------
// Allows you to edit registered types

namespace KRG::Editor
{
    class KRG_TOOLS_EDITOR_API PropertyGrid
    {
    public:

        PropertyGrid( EditorModel* pModel );

        // Set the type instance to edit, will reset dirty status
        template<typename T>
        inline void SetTypeToEdit( T* pTypeInstance )
        {
            if ( (Byte*) pTypeInstance == m_pTypeInstance )
            {
                return;
            }

            KRG_ASSERT( pTypeInstance != nullptr );
            m_pTypeInfo = pTypeInstance->GetTypeInfo();
            m_pTypeInstance = reinterpret_cast<Byte*>( pTypeInstance );
            m_isDirty = false;
        }

        // Set the type instance to edit, will reset dirty status
        inline void SetTypeToEdit( TypeSystem::TypeInfo const* pTypeInfo, void* pTypeInstance )
        {
            if ( pTypeInstance == m_pTypeInstance )
            {
                return;
            }

            KRG_ASSERT( pTypeInfo != nullptr && pTypeInstance != nullptr );
            m_pTypeInfo = pTypeInfo;
            m_pTypeInstance = reinterpret_cast<Byte*>( pTypeInstance );
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

        void DrawPropertyRow( TypeSystem::TypeInfo const& typeInfo, Byte* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance );
        void DrawValuePropertyRow( TypeSystem::TypeInfo const& typeInfo, Byte* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance, int32 arrayIdx = InvalidIndex );
        void DrawArrayPropertyRow( TypeSystem::TypeInfo const& typeInfo, Byte* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance );

        void PreChange();
        void PostChange();

    private:

        EditorModel*                    m_pModel = nullptr;
        PG::Context                     m_context;
        TypeSystem::TypeInfo const*     m_pTypeInfo = nullptr;
        Byte*                           m_pTypeInstance = nullptr;
        bool                            m_isDirty = false;
    };
}