#pragma once

#include "EditorModel.h"
#include "Tools/Core/Helpers/GlobalRegistryBase.h"
#include "System/Core/Types/String.h"
#include "System/Core/Types/StringID.h"
#include "System/DevTools/DevelopmentToolset.h"
#include "System/DevTools/ImguiX.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Editor Base
    //-------------------------------------------------------------------------
    // Defines the set of tools created and available when instantiating the editor.
    // DO NOT DERIVED FROM THIS CLASS - derive from 'TEditor' instead

    class KRG_TOOLS_CORE_API StandaloneEditor : public ImGuiX::DevelopmentToolset
    {
        template<typename ModelType> friend class TStandaloneEditor;

        struct ModalPopupMessage
        {
            String                                      m_ID;
            String                                      m_channel;
            String                                      m_message;
            Log::Severity                               m_severity;
            bool                                        m_isOpen = true;
        };

    public:

        virtual ~StandaloneEditor();

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
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) = 0;
        virtual void FrameEndUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) {}

        // Draw 3D viewport
        void DrawViewportWindow( UpdateContext const& context, Render::ViewportManager& viewportManager, char const* const pEditorViewportName = "Editor Viewport" );

    private:

        virtual void Update( UpdateContext const& context, Render::ViewportManager& viewportManager ) override final;

        void DrawPopups( UpdateContext const& context );

    private:

        ImGuiID                             m_mainDockspaceID;
        bool                                m_mouseWithinEditorViewport = false;
        EditorModel*                        m_pModel = nullptr;
        TVector<ModalPopupMessage>          m_modalPopups;
    };

    //-------------------------------------------------------------------------

    template<typename ModelType>
    class TStandaloneEditor : public StandaloneEditor
    {
        static_assert( std::is_base_of<EditorModel, ModelType>::value, "ModelType must derive from editor model" );

    public:

        TStandaloneEditor()
        {
            m_pModel = KRG::New<ModelType>();
        }

        virtual ~TStandaloneEditor()
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

    class KRG_TOOLS_CORE_API StandaloneEditorFactory : public TGlobalRegistryBase<StandaloneEditorFactory>
    {
        KRG_DECLARE_GLOBAL_REGISTRY( StandaloneEditorFactory );

    public:

        static StandaloneEditor* TryCreateEditor( StringID const& editorModeID );

    protected:

        virtual const char* GetID() const = 0;
        virtual StandaloneEditor* CreateEditor() const = 0;
    };
}

//-------------------------------------------------------------------------
//  Macro to create a resource workspace factory
//-------------------------------------------------------------------------
// Use in a CPP to define a factory e.g., KRG_RESOURCE_WORKSPACE_FACTORY( SkeletonWorkspaceFactory, Skeleton, SkeletonResourceEditor );

#define KRG_STANDALONE_EDITOR_FACTORY( FactoryName, FactoryID, StandaloneEditorClass )\
class FactoryName : public StandaloneEditorFactory\
{\
    virtual const char* GetID() const override { return #FactoryID; }\
    virtual StandaloneEditor* CreateEditor() const override { return KRG::New<StandaloneEditorClass>(); }\
};\
static FactoryName g_##FactoryName;