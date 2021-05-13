#pragma once

#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "System/DevTools/ImguiX.h"
#include "PropertyEditors.h"

//-------------------------------------------------------------------------

namespace KRG { class EditorModel; }

//-------------------------------------------------------------------------
// Property grid
//-------------------------------------------------------------------------
// Allows you to edit registered types

namespace KRG::TypeSystem
{
    class PropertyGrid
    {
    public:

        PropertyGrid( EditorModel* pModel );

        // Set the type instance to edit, will reset dirty status
        template<typename T>
        inline void SetTypeToEdit( T* pTypeInstance )
        {
            KRG_ASSERT( pTypeInstance != nullptr );
            m_pTypeInfo = T::s_pTypeInfo;
            m_pTypeInstance = reinterpret_cast<Byte*>( pTypeInstance );
            m_isDirty = false;
        }

        // Set the type instance to edit, will reset dirty status
        inline void SetTypeToEdit( TypeInfo const* pTypeInfo, void* pTypeInstance )
        {
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

        void DrawPropertyRow( TypeInfo const& typeInfo, Byte* pTypeInstance, PropertyInfo const& propertyInfo, Byte* propertyInstance );
        void DrawValuePropertyRow( TypeInfo const& typeInfo, Byte* pTypeInstance, PropertyInfo const& propertyInfo, Byte* propertyInstance, int32 arrayIdx = InvalidIndex );
        void DrawArrayPropertyRow( TypeInfo const& typeInfo, Byte* pTypeInstance, PropertyInfo const& propertyInfo, Byte* propertyInstance );

        void PreChange();
        void PostChange();

    private:

        EditorModel*        m_pModel = nullptr;
        PG::Context         m_context;
        TypeInfo const*     m_pTypeInfo = nullptr;
        Byte*               m_pTypeInstance = nullptr;
        bool                m_isDirty = false;
    };
}