#include "MeshEditor_Model.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    void Model::Initialize( UpdateContext const& context )
    {
        EditorModel::Initialize( context );

        m_pDataBrowserModel = KRG::New<DataBrowserModel>( m_sourceDataDirectory );
        static char const* const allowedExtensions[] = { ".msh", ".smsh", ".fbx", ".gltf", 0 };
        m_pDataBrowserModel->Initialize( allowedExtensions );
    }

    void Model::Shutdown( UpdateContext const& context )
    {
        m_pDataBrowserModel->Shutdown();
        KRG::Delete( m_pDataBrowserModel );

        EditorModel::Shutdown( context );
    }
}