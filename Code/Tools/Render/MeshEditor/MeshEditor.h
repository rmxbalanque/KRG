#pragma once

#include "MeshEditor_DataBrowser.h"
#include "Tools/Core/Editor/Editor.h"
#include "Tools/Core/Editor/EditorTabWell.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class MeshEditor final : public TEditor<EditorModel>
    {
    public:

        ~MeshEditor();

    private:

        virtual char const* GetName() const override final { return "Kruger Mesh Editor"; }

        virtual void SetUserFlags( uint64 flags ) override;
        virtual uint64 GetUserFlags() const override;

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

    private:

        DataBrowser*                        m_pDataBrowser = nullptr;
        EditorTabWell*                      m_pTabWell = nullptr;
    };
}