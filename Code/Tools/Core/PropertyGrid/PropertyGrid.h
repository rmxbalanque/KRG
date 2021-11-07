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
    struct TypeSystem::PropertyInfo;
}

//-------------------------------------------------------------------------
// Property grid
//-------------------------------------------------------------------------
// Allows you to edit registered types

namespace KRG
{
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

        PropertyGrid( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& rawResourceDirectoryPath );

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

        //-------------------------------------------------------------------------

        // Event fired just before a property is modified
        inline TSingleUserEvent<void( PropertyEditInfo const& )> OnPreEdit() { return m_preEditEvent; }

        // Event fired just after a property was modified
        inline TSingleUserEvent<void( PropertyEditInfo const& )> OnPostEdit() { return m_postEditEvent; }

    private:

        void DrawPropertyRow( TypeSystem::TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance );
        void DrawValuePropertyRow( TypeSystem::TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance, int32 arrayIdx = InvalidIndex );
        void DrawArrayPropertyRow( TypeSystem::TypeInfo const& typeInfo, IRegisteredType* pTypeInstance, TypeSystem::PropertyInfo const& propertyInfo, Byte* propertyInstance );

        void CreatePropertyEditor( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );

        // Editors
        //-------------------------------------------------------------------------

        void CreateEditorBool( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorInt8( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorInt16( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorInt32( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorInt64( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorUInt8( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorUInt16( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorUInt32( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorUInt64( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorFloat( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorDouble( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorColor( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorMicroseconds( TypeSystem::PropertyInfo const & propertyInfo, Byte * pPropertyInstance );
        void CreateEditorMilliseconds( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorSeconds( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorPercentage( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorDegrees( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorRadians( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorFloat2( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorFloat3( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorFloat4( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorVector( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorEulerAngles( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorQuaternion( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorMatrix( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorTransform( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorIntRange( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorFloatRange( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorString( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorStringID( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorUUID( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorResourcePath( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorResourceTypeID( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorResourceID( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorResourcePtr( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorEnum( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorGenericFlags( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );
        void CreateEditorSpecificFlags( TypeSystem::PropertyInfo const& propertyInfo, Byte* pPropertyInstance );

    private:

        TypeSystem::TypeRegistry const&                             m_typeRegistry;
        FileSystem::Path const                                      m_rawResourceDirectoryPath;
        TypeSystem::TypeInfo const*                                 m_pTypeInfo = nullptr;
        IRegisteredType*                                            m_pTypeInstance = nullptr;
        bool                                                        m_isDirty = false;

        TSingleUserEventInternal<void( PropertyEditInfo const& )>   m_preEditEvent; // Fired just before we change a property value
        TSingleUserEventInternal<void( PropertyEditInfo const& )>   m_postEditEvent; // Fired just after we change a property value
    };
}