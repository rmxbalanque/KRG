#pragma once

#include "Applications/Editor/EditorModel.h"
#include "Tools/Resource/DataBrowser/DataBrowserModel.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    class Model final : public EditorModel
    {

    public:

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

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

        DataPath    m_previewedMeshPath;
    };
}