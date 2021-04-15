#pragma once

#include "EditorTool.h"
#include "System/Core/Types/String.h"
#include "System/Core/Types/StringID.h"
#include "System/DevTools/DevelopmentTools.h"
#include "System/DevTools/ImguiX.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Base Editor Mode
    //-------------------------------------------------------------------------
    // Defines the set of tools created and available when instantiating the editor.
    // DO NOT DERIVED FROM THIS CLASS - derive from 'TEditor' instead

    class Editor : public ImGuiX::DevelopmentTools
    {
        template<typename ModelType> friend class TEditor;

        struct ModalPopupMessage
        {
            String                                      m_ID;
            String                                      m_channel;
            String                                      m_message;
            Log::Severity                               m_severity;
            bool                                        m_isOpen = true;
        };

    public:

        virtual ~Editor();

        virtual char const* GetName() const = 0;

        // Optional: User Flags
        virtual void SetUserFlags( uint64 flags ) {}
        virtual uint64 GetUserFlags() const { return 0; }

    protected:

        // Init/shutdown
        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

        // Menus and Toolbars
        virtual void DrawMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager );
        virtual bool HasViewportToolbar() const { return false; }
        virtual void DrawViewportToolbar( UpdateContext const& context, Render::ViewportManager& viewportManager ) {};

        // Main UI
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) {};
        virtual void FrameEndUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) {}

        // Tools
        template<typename T, typename ... ConstructorParams> T* CreateTool( UpdateContext const& context, ConstructorParams&&... params )
        {
            static_assert( std::is_base_of<EditorTool, T>::value, "Editor tools need to derive from EditorTool" );
            auto pTool = KRG::New<T>( std::forward<ConstructorParams>( params )... );
            m_editorTools.emplace_back( pTool );

            pTool->Initialize( context );
            return pTool;
        }

        template<typename T>
        void DestroyTool( UpdateContext const& context, T*& pTool )
        {
            DestroyTool( context, static_cast<EditorTool*>( pTool ) );
            pTool = nullptr;
        }

    private:

        virtual void Update( UpdateContext const& context, Render::ViewportManager& viewportManager ) override final;

        void DrawPopups( UpdateContext const& context );
        void DrawEditorMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager );
        void DrawEditorDockSpaceAndViewport( UpdateContext const& context, Render::ViewportManager& viewportManager );

        void DestroyTool( UpdateContext const& context, EditorTool* pTool );

    private:

        ImGuiID                             m_mainDockspaceID;
        bool                                m_mouseWithinEditorViewport = false;
        EditorModel*                        m_pModel = nullptr;
        TVector<ModalPopupMessage>          m_modalPopups;
        TVector<EditorTool*>                m_editorTools;
    };

    //-------------------------------------------------------------------------

    template<typename ModelType>
    class TEditor : public Editor
    {
        static_assert( std::is_base_of<EditorModel, ModelType>::value, "ModelType must derive from editor model" );

    public:

        TEditor()
        {
            m_pModel = KRG::New<ModelType>();
        }

        virtual ~TEditor()
        {
            KRG::Delete( m_pModel );
        }

        inline ModelType& GetModel() { return *static_cast<ModelType*>( m_pModel ); }
        inline ModelType const& GetModel() const { return *static_cast<ModelType const*>( m_pModel ); }
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