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
    };
}