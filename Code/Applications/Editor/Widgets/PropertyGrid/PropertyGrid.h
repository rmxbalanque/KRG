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
        ~PropertyGrid();

        inline bool IsDirty() const { return m_isDirty; }

        void SetTypeToEdit( TypeInstanceModel* pTypeInstanceModel = nullptr );
        void DrawGrid();

    private:

        void DrawPropertyRow( PropertyInstanceModel* pPropertyModel );
        void DrawArrayPropertyRow( PropertyInstanceModel* pPropertyModel );
        void DrawStructPropertyRow( PropertyInstanceModel* pPropertyModel );
        void DrawCoreTypePropertyRow( PropertyInstanceModel* pPropertyModel );

        void DrawRowControls( PropertyInstanceModel* pPropertyModel );

    private:

        TypeRegistry const&                 m_typeRegistry;
        FileSystem::Path const              m_sourceDataPath;
        TypeInstanceModel*                  m_pTypeInstanceModel = nullptr;
        bool                                m_isDirty;
    };
}