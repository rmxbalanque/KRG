#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/TypeSystem/TypeInfo.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem 
{
    class TypeRegistry; 
    struct PropertyInfo;
}

namespace KRG::Resource { class ResourceDatabase; }

//-------------------------------------------------------------------------
// Property grid
//-------------------------------------------------------------------------
// Allows you to edit registered types

namespace KRG
{
    namespace TypeSystem{ class PropertyEditor; }

    //-------------------------------------------------------------------------

    struct PropertyEditInfo
    {
        enum class Action
        {
            Invalid = -1,
            Edit,
            AddArrayElement,
            RemoveArrayElement,
        };

    public:

        IRegisteredType*                        m_pEditedTypeInstance = nullptr;
        TypeSystem::PropertyInfo const*         m_pPropertyInfo = nullptr;
        Action                                  m_action = Action::Invalid;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API PropertyGrid
    {
        friend struct ScopedChangeNotifier;

    public:

        PropertyGrid( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDatabase );
        ~PropertyGrid();

        // Set the type instance to edit, will reset dirty status
        void SetTypeToEdit( IRegisteredType* pTypeInstance );

        // Set the type instance to edit, will reset dirty status
        void SetTypeToEdit( nullptr_t );

        // Display the grid
        void DrawGrid();

        // Has the type been modified
        inline bool IsDirty() const { return m_isDirty; }

        // Clear the modified flag i.e. when changes have been committed/saved
        void ClearDirty() { m_isDirty = false; }

        //-------------------------------------------------------------------------

        // Event fired just before a property is modified
        inline TSingleUserEvent<void( PropertyEditInfo const& )> OnPreEdit() { return m_preEditEvent; }

        // Event fired just after a property was modified
        inline TSingleUserEvent<void( PropertyEditInfo const& )> OnPostEdit() { return m_postEditEvent; }

    private:

        void DrawPropertyRow( TypeSystem::TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance );
        void DrawValuePropertyRow( TypeSystem::TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance, int32 arrayIdx = InvalidIndex );
        void DrawArrayPropertyRow( TypeSystem::TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance );

        TypeSystem::PropertyEditor* GetPropertyEditor( TypeSystem::PropertyInfo const& propertyInfo, Byte* pActualPropertyInstance );
        void DestroyPropertyEditors();

    private:

        TypeSystem::TypeRegistry const&                             m_typeRegistry;
        Resource::ResourceDatabase const&                           m_resourceDB;
        TypeSystem::TypeInfo const*                                 m_pTypeInfo = nullptr;
        IRegisteredType*                                            m_pTypeInstance = nullptr;
        bool                                                        m_isDirty = false;

        TSingleUserEventInternal<void( PropertyEditInfo const& )>   m_preEditEvent; // Fired just before we change a property value
        TSingleUserEventInternal<void( PropertyEditInfo const& )>   m_postEditEvent; // Fired just after we change a property value
        THashMap<void*, TypeSystem::PropertyEditor*>                m_propertyEditors;
    };
}