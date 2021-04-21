#pragma once

#include "Applications/Editor/EditorModel.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshToolkit
{
    class Model final : public EditorModel
    {
    public:

        // Mesh preview
        //-------------------------------------------------------------------------

        inline void SetPreviewedMeshPath( DataPath const& previewMeshPath )
        {
            KRG_ASSERT( previewMeshPath.IsValid() );
            m_previewedMeshPath = previewMeshPath;
        }
        
        inline void ClearPreviewedMeshPath() { m_previewedMeshPath = DataPath(); }

        inline DataPath const& GetPreviewedMeshPath() const { return m_previewedMeshPath; }

    private:

        DataPath                            m_previewedMeshPath;
    };
}