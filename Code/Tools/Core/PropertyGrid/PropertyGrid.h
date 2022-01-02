#pragma once

#include "Tools/Core/_Module/API.h"
#include "Tools/Core/Resource/ResourceFilePicker.h"
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

        // Get the current edited type
        IRegisteredType const* GetEditedType() const { return m_pTypeInstance; }

        // Get the current edited type
        IRegisteredType* GetEditedType() { return m_pTypeInstance; }

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

        // Manually flag the grid as dirty
        void MarkDirty() { m_isDirty = true; }

        //-------------------------------------------------------------------------

        // Event fired just before a property is modified
        inline TEventHandle<PropertyEditInfo const&> OnPreEdit() { return m_preEditEvent; }

        // Event fired just after a property was modified
        inline TEventHandle<PropertyEditInfo const&> OnPostEdit() { return m_postEditEvent; }

    private:

        void DrawPropertyRow( TypeSystem::TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance );
        void DrawValuePropertyRow( TypeSystem::TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance, int32 arrayIdx = InvalidIndex );
        void DrawArrayPropertyRow( TypeSystem::TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance );

        TypeSystem::PropertyEditor* GetPropertyEditor( TypeSystem::PropertyInfo const& propertyInfo, Byte* pActualPropertyInstance );
        void DestroyPropertyEditors();

    private:

        TypeSystem::TypeRegistry const&                             m_typeRegistry;
        TypeSystem::TypeInfo const*                                 m_pTypeInfo = nullptr;
        Resource::ResourceFilePicker                                m_resourcePicker;
        IRegisteredType*                                            m_pTypeInstance = nullptr;
        bool                                                        m_isDirty = false;

        TEvent<PropertyEditInfo const&>                             m_preEditEvent; // Fired just before we change a property value
        TEvent<PropertyEditInfo const&>                             m_postEditEvent; // Fired just after we change a property value
        THashMap<void*, TypeSystem::PropertyEditor*>                m_propertyEditors;
    };
}