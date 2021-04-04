#pragma once
#include "Applications/Editor/EditorTool.h"
#include "MeshEditor_Model.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    class MeshInfo : public EditorTool
    {
    public:

        MeshInfo( Model& model ) : m_model( model ) {}

        virtual char const * const GetName() { return "Mesh Info"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        Model&                              m_model;
    };
}