#pragma once

#include "Common/DataDirectoryModel.h"
#include "System/Core/Types/String.h"
#include "System/Core/Types/StringID.h"
#include "System/DevTools/DevelopmentTools.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Editor Mode
    //-------------------------------------------------------------------------
    // Defines the set of tools created and available when instantiating the editor.

    class Editor : public ImGuiX::DevelopmentTools
    {

    public:

        virtual char const* GetName() const = 0;

    protected:

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override {}
        virtual void Shutdown() override {}

        // Menus and Toolbars
        virtual void DrawMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager ) {}
        virtual bool HasViewportToolbar() const { return false; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::ViewportManager& viewportManager ) {};

        // Main UI
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) = 0;
        virtual void FrameEndUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) {}

    private:

        virtual void Update( UpdateContext const& context, Render::ViewportManager& viewportManager ) override final;

        void DrawEditorMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager );
        void DrawEditorDockSpaceAndViewport( UpdateContext const& context, Render::ViewportManager& viewportManager );

    private:

        ImGuiID                             m_mainDockspaceID;
        bool                                m_mouseWithinEditorViewport = false;
    };

    //-------------------------------------------------------------------------
    // Editor Mode Factory Base
    //-------------------------------------------------------------------------
    // Registers an edit mode factory method with a specific ID

    class EditorFactory
    {
        friend class EditorRegistry;

        // Static linked list to iterate over all editor factories
        static EditorFactory*   s_pHead;
        static EditorFactory*   s_pTail;

    public:

        EditorFactory();

        // Main interface - every mode needs to declare a static factory
        virtual StringID GetID() const = 0;
        virtual Editor* CreateEditor() const = 0;

    private:

        EditorFactory*          m_pNext = nullptr;
    };

    //-------------------------------------------------------------------------
    // Editor Mode Registry
    //-------------------------------------------------------------------------
    // Keeps track of all registered editor modes

    class EditorRegistry
    {
    public:

        static Editor* TryCreateEditor( StringID const& editorModeID );
    };
}