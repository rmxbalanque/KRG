#include "AnimationEditor.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class AnimationEditorFactory : public EditorFactory
    {
    public:

        virtual StringID GetID() const override  { return StringID( "AnimationEditor" ); }
        virtual Editor* CreateEditor() const override { return KRG::New<AnimationEditor>(); }
    };

    static const AnimationEditorFactory g_AnimationEditorFactory;
}

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    void AnimationEditor::Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry )
    {
    }

    void AnimationEditor::Shutdown()
    {
    }

    void AnimationEditor::DrawMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        ImGui::TextColored( (Float4) Colors::LimeGreen, KRG_ICON_FILM );
    }

    void AnimationEditor::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( ImGui::Begin( "AnimationEditor" ) )
        {
            ImGui::Text( "DASdas" );
        }
        ImGui::End();
    }
}