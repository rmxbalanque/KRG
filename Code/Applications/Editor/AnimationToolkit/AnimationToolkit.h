#pragma once

#include "Applications/Editor/EditorToolkit.h"
#include "AnimationToolkit_Model.h"
#include "AnimationToolkit_DataBrowserTool.h"
#include "AnimationToolkit_AnimationEditorTool.h"
#include "AnimationToolkit_SkeletonTreeTool.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::AnimationTools
{
    class AnimationToolkit final : public TEditorToolkit<Model>
    {
    public:

        ~AnimationToolkit();

    private:

        virtual char const* GetName() const override final { return "Kruger Animation Toolkit"; }

        virtual void SetUserFlags( uint64 flags ) override;
        virtual uint64 GetUserFlags() const override;

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

    private:

        DataBrowser*                        m_pDataBrowser = nullptr;
        AnimationEditor*                  m_pAnimationTimeline = nullptr;
        SkeletonTree*                       m_pSkeletonTree = nullptr;
    };
}