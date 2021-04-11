#pragma once

#include "Tools/Core/_Module/API.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class PropertyGrid
    {

    public:

        PropertyGrid( TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataPath );

        inline bool IsDirty() const { return m_isDirty; }
        void ClearDirty() { m_isDirty = false; }

        void SetTypeToEdit( TypeInstanceModel* pTypeInstanceModel = nullptr );
        void DrawGrid();

    private:

        void DrawPropertyRow( PropertyInstanceModel* pPropertyModel, PropertyInstanceModel* pParentPropertyModel );
        void DrawArrayPropertyRow( PropertyInstanceModel* pPropertyModel, PropertyInstanceModel* pParentPropertyModel );
        void DrawStructPropertyRow( PropertyInstanceModel* pPropertyModel, PropertyInstanceModel* pParentPropertyModel );
        void DrawCoreTypePropertyRow( PropertyInstanceModel* pPropertyModel, PropertyInstanceModel* pParentPropertyModel );

        void DrawRowControls( PropertyInstanceModel* pPropertyModel, PropertyInstanceModel* pParentPropertyModel );

    private:

        TypeRegistry const&                 m_typeRegistry;
        FileSystem::Path const              m_sourceDataPath;
        TypeInstanceModel*                  m_pTypeInstanceModel = nullptr;
        bool                                m_isDirty;
    };
}