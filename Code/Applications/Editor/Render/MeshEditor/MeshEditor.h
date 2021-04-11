#pragma once

#include "Applications/Editor/Editor.h"
#include "MeshEditor_Model.h"
#include "MeshEditor_DataBrowserTool.h"
#include "MeshEditor_MeshInfoTool.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    class MainEditor final : public TEditor<Model>
    {
    public:

        ~MainEditor();

    private:

        virtual char const* GetName() const override final { return "Kruger Mesh Editor"; }

        virtual void SetUserFlags( uint64 flags ) override;
        virtual uint64 GetUserFlags() const override;

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

    private:

        DataBrowser*                        m_pDataBrowser = nullptr;
        MeshInfo*                           m_pMeshInfo = nullptr;
    };
}