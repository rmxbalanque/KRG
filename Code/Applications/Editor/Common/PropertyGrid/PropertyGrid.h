#pragma once

#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------
// Property grid
//-------------------------------------------------------------------------
// Allows you to edit registered types

namespace KRG::TypeSystem
{
    class PropertyGrid
    {

    public:

        PropertyGrid( TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataPath );

        // Set the type instance to edit, will reset dirty status
        void SetTypeToEdit( TypeInstanceModel* pTypeInstanceModel = nullptr );

        // Has the type been modified
        inline bool IsDirty() const { return m_isDirty; }

        // Clear the modified flag i.e. when changes have been committed/saved
        void ClearDirty() { m_isDirty = false; }

        // Display the grid
        void DrawGrid();

    private:

        // Draw the rows in the property grid
        void DrawPropertyRow( PropertyInstanceModel* pPropertyModel, PropertyInstanceModel* pParentPropertyModel );
        void DrawArrayPropertyRow( PropertyInstanceModel* pPropertyModel, PropertyInstanceModel* pParentPropertyModel );
        void DrawStructPropertyRow( PropertyInstanceModel* pPropertyModel, PropertyInstanceModel* pParentPropertyModel );
        void DrawCoreTypePropertyRow( PropertyInstanceModel* pPropertyModel, PropertyInstanceModel* pParentPropertyModel );
        void DrawRowControls( PropertyInstanceModel* pPropertyModel, PropertyInstanceModel* pParentPropertyModel );

        // Property value and change handlers
        void ResetToDefaultValue( PropertyInstanceModel* pPropertyModel );
        void PreChange( PropertyInstanceModel& propertyModel );
        void PostChange( PropertyInstanceModel& propertyModel );

    private:

        TypeRegistry const&                 m_typeRegistry;
        FileSystem::Path const              m_sourceDataPath;
        TypeInstanceModel*                  m_pTypeInstanceModel = nullptr;
        bool                                m_isDirty;
    };
}