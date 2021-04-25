#pragma once

#include "Applications/Editor/EditorToolkit.h"
#include "MeshToolkit_Model.h"
#include "MeshToolkit_DataBrowserTool.h"
#include "MeshToolkit_MeshInfoTool.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
{
    class MeshToolkit final : public TEditorToolkit<Model>
    {
    public:

        ~MeshToolkit();

    private:

        virtual char const* GetName() const override final { return "Kruger Mesh Toolkit"; }

        virtual void SetUserFlags( uint64 flags ) override;
        virtual uint64 GetUserFlags() const override;

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

    private:

        DataBrowser*                        m_pDataBrowser = nullptr;
        MeshInfo*                           m_pMeshInfo = nullptr;
    };
}