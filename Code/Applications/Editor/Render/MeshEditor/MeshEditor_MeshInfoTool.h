#pragma once
#include "Applications/Editor/EditorTool.h"
#include "MeshEditor_Model.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    class MeshInfo : public TEditorTool<Model>
    {
    public:

        MeshInfo( EditorModel* pModel ) : TEditorTool<Model>( pModel ) {}

        virtual char const * const GetName() { return "Mesh Info"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;
    };
}