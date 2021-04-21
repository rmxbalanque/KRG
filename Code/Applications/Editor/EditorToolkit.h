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
    // Editor Toolkit Base
    //-------------------------------------------------------------------------
    // Defines the set of tools created and available when instantiating the editor.
    // DO NOT DERIVED FROM THIS CLASS - derive from 'TEditor' instead

    class EditorToolkit : public ImGuiX::DevelopmentTools
    {
        template<typename ModelType> friend class TEditorToolkit;

        struct ModalPopupMessage
        {
            String                                      m_ID;
            String                                      m_channel;
            String                                      m_message;
            Log::Severity                               m_severity;
            bool                                        m_isOpen = true;
        };

    public:

        virtual ~EditorToolkit();

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
            
            EditorTool* pAddedTool = m_editorTools.emplace_back( pTool );
            pAddedTool->Initialize( context );

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
    class TEditorToolkit : public EditorToolkit
    {
        static_assert( std::is_base_of<EditorModel, ModelType>::value, "ModelType must derive from editor model" );

    public:

        TEditorToolkit()
        {
            m_pModel = KRG::New<ModelType>();
        }

        virtual ~TEditorToolkit()
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

    class EditorToolkitFactory
    {
        friend class EditorToolkitRegistry;

        // Static linked list to iterate over all editor factories
        static EditorToolkitFactory*   s_pHead;
        static EditorToolkitFactory*   s_pTail;

    public:

        EditorToolkitFactory();

        // Main interface - every mode needs to declare a static factory
        virtual StringID GetID() const = 0;
        virtual EditorToolkit* CreateEditor() const = 0;

    private:

        EditorToolkitFactory*          m_pNext = nullptr;
    };

    //-------------------------------------------------------------------------
    // Editor Mode Registry
    //-------------------------------------------------------------------------
    // Keeps track of all registered editor modes

    class EditorToolkitRegistry
    {
    public:

        static EditorToolkit* TryCreateEditor( StringID const& editorModeID );
    };
}