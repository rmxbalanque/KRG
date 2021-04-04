#pragma once

#include "Tools/Core/_Module/API.h"
#include "Tools/Core/TypeSystem/TypeInstanceModel.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class KRG_TOOLS_CORE_API PropertyGrid
    {
    public:
        
        PropertyGrid( TypeRegistry const& typeRegistry );
        ~PropertyGrid();

        inline bool IsDirty() const { return m_isDirty; }

        void SetTypeToEdit( TypeInstanceModel* pTypeInstanceModel = nullptr );
        void DrawGrid();

    private:

        TypeRegistry const&                 m_typeRegistry;
        TypeInstanceModel*                  m_pTypeInstanceModel = nullptr;
        bool                                m_isDirty;
    };
}