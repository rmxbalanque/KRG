#include "ResourceWorkspace.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "System/TypeSystem/TypeSerialization.h"

//-------------------------------------------------------------------------

namespace KRG
{
    KRG_DEFINE_GLOBAL_REGISTRY( ResourceWorkspaceFactory );

    //-------------------------------------------------------------------------

    class ResourceDescriptorUndoableAction final : public IUndoableAction
    {
    public:

        ResourceDescriptorUndoableAction( TypeSystem::TypeRegistry const& typeRegistry, IRegisteredType* pTypeInstance )
            : m_typeRegistry( typeRegistry )
            , m_pEditedType( pTypeInstance )
        {
            KRG_ASSERT( m_pEditedType != nullptr );
        }

        virtual void Undo() override
        {
            TypeSystem::Serialization::ReadNativeTypeFromString( m_typeRegistry, m_valueBefore, m_pEditedType );
        }

        virtual void Redo() override
        {
            TypeSystem::Serialization::ReadNativeTypeFromString( m_typeRegistry, m_valueAfter, m_pEditedType );
        }

        void SerializeBeforeState()
        {
            TypeSystem::Serialization::WriteNativeTypeToString( m_typeRegistry, m_pEditedType, m_valueBefore );
        }

        void SerializeAfterState()
        {
            TypeSystem::Serialization::WriteNativeTypeToString( m_typeRegistry, m_pEditedType, m_valueAfter );
        }

    private:

        TypeSystem::TypeRegistry const&     m_typeRegistry;
        IRegisteredType*                    m_pEditedType = nullptr;
        String                              m_valueBefore;
        String                              m_valueAfter;
    };

    //-------------------------------------------------------------------------

    GenericResourceWorkspace::GenericResourceWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
        : EditorWorkspace( context, pWorld, context.ToFileSystemPath( resourceID.GetResourcePath() ).GetFileNameWithoutExtension() )
        , m_descriptorID( resourceID )
        , m_descriptorPath( resourceID.GetResourcePath().ToFileSystemPath( context.GetRawResourceDirectoryPath() ) )
        , m_descriptorPropertyGrid( *context.m_pTypeRegistry, *context.m_pResourceDatabase )
    {
        KRG_ASSERT( resourceID.IsValid() );

        m_preEditEventBindingID = m_descriptorPropertyGrid.OnPreEdit().Bind( [this] ( PropertyEditInfo const& info ) { PreEdit( info ); } );
        m_postEditEventBindingID = m_descriptorPropertyGrid.OnPostEdit().Bind( [this] ( PropertyEditInfo const& info ) { PostEdit( info ); } );
    }

    GenericResourceWorkspace::~GenericResourceWorkspace()
    {
        KRG_ASSERT( m_pDescriptor == nullptr );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );

        m_descriptorPropertyGrid.OnPreEdit().Unbind( m_preEditEventBindingID );
        m_descriptorPropertyGrid.OnPostEdit().Unbind( m_postEditEventBindingID );
    }

    void GenericResourceWorkspace::Initialize( UpdateContext const& context )
    {
        EditorWorkspace::Initialize( context );

        m_descriptorWindowName.sprintf( "Descriptor##%u", GetID() );

        TypeSystem::Serialization::TypeReader typeReader( *m_editorContext.m_pTypeRegistry );
        if ( typeReader.ReadFromFile( m_descriptorPath ) )
        {
            TypeSystem::TypeDescriptor typeDesc;
            if ( typeReader.ReadType( typeDesc ) )
            {
                m_pDescriptor = typeDesc.CreateTypeInstance<Resource::ResourceDescriptor>( *m_editorContext.m_pTypeRegistry );
                m_descriptorPropertyGrid.SetTypeToEdit( m_pDescriptor );
            }
        }
    }

    void GenericResourceWorkspace::Shutdown( UpdateContext const& context )
    {
        KRG::Delete( m_pDescriptor );
        EditorWorkspace::Shutdown( context );
    }

    void GenericResourceWorkspace::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGui::DockBuilderDockWindow( m_descriptorWindowName.c_str(), dockspaceID );
    }

    void GenericResourceWorkspace::UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( m_descriptorWindowName.c_str() ) )
        {
            if ( auto pDockNode = ImGui::GetWindowDockNode() )
            {
                pDockNode->LocalFlags |= ImGuiDockNodeFlags_HiddenTabBar;
            }

            //-------------------------------------------------------------------------

            if ( m_pDescriptor == nullptr )
            {
                ImGui::Text( "Failed to load descriptor!" );
            }
            else
            {
                {
                    ImGuiX::ScopedFont sf( ImGuiX::Font::Medium );
                    ImGui::Text( "Descriptor: %s", m_descriptorID.c_str() );
                }

                ImGui::BeginDisabled( !m_descriptorPropertyGrid.IsDirty() );
                if ( ImGuiX::ColoredButton( ImGuiX::ConvertColor( Colors::ForestGreen ), ImGuiX::ConvertColor( Colors::White ), KRG_ICON_FLOPPY_O " Save", ImVec2( -1, 0 ) ) )
                {
                    Save();
                }
                ImGui::EndDisabled();

                m_descriptorPropertyGrid.DrawGrid();
            }
        }
        ImGui::End();
    }

    void GenericResourceWorkspace::DrawWorkspaceToolbar( UpdateContext const& context )
    {
        EditorWorkspace::DrawWorkspaceToolbar( context );

        if ( ImGui::MenuItem( KRG_ICON_CLONE" Copy Path" ) )
        {
            ImGui::SetClipboardText( m_descriptorID.c_str() );
        }
    }

    void GenericResourceWorkspace::DrawDescriptorWindow( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( m_descriptorWindowName.c_str() ) )
        {
            if ( m_pDescriptor == nullptr )
            {
                ImGui::Text( "Failed to load descriptor!" );
            }
            else
            {
                m_descriptorPropertyGrid.DrawGrid();
            }
        }
        ImGui::End();
    }

    bool GenericResourceWorkspace::Save()
    {
        KRG_ASSERT( m_descriptorID.IsValid() && m_descriptorPath.IsFile() );
        KRG_ASSERT( m_pDescriptor != nullptr );
        
        if ( WriteResourceDescriptorToFile( *m_editorContext.m_pTypeRegistry, m_descriptorPath, m_pDescriptor ) )
        {
            m_descriptorPropertyGrid.ClearDirty();
            return true;
        }

        return false;
    }

    void GenericResourceWorkspace::PreEdit( PropertyEditInfo const& info )
    {
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );
        auto pUndoableAction = KRG::New<ResourceDescriptorUndoableAction>( *m_editorContext.m_pTypeRegistry, m_pDescriptor );
        pUndoableAction->SerializeBeforeState();
        m_pActiveUndoableAction = pUndoableAction;
    }

    void GenericResourceWorkspace::PostEdit( PropertyEditInfo const& info )
    {
        KRG_ASSERT( m_pActiveUndoableAction != nullptr );
        auto pUndoableAction = static_cast<ResourceDescriptorUndoableAction*>( m_pActiveUndoableAction );
        pUndoableAction->SerializeAfterState();
        m_undoStack.RegisterAction( pUndoableAction );
        m_pActiveUndoableAction = nullptr;
    }

    //-------------------------------------------------------------------------

    bool ResourceWorkspaceFactory::HasCustomWorkspace( ResourceTypeID const& resourceTypeID )
    {
        auto pCurrentFactory = s_pHead;
        while ( pCurrentFactory != nullptr )
        {
            if ( resourceTypeID == pCurrentFactory->GetSupportedResourceTypeID() )
            {
                return true;
            }

            pCurrentFactory = pCurrentFactory->GetNextItem();
        }

        return false;
    }

    EditorWorkspace* ResourceWorkspaceFactory::TryCreateWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );
        auto resourceTypeID = resourceID.GetResourceTypeID();
        KRG_ASSERT( resourceTypeID.IsValid() );

        auto pCurrentFactory = s_pHead;
        while ( pCurrentFactory != nullptr )
        {
            if ( resourceTypeID == pCurrentFactory->GetSupportedResourceTypeID() )
            {
                return pCurrentFactory->CreateWorkspace( context, pWorld, resourceID );
            }

            pCurrentFactory = pCurrentFactory->GetNextItem();
        }

        // Create generic descriptor workspace
        //-------------------------------------------------------------------------

        return KRG::New<GenericResourceWorkspace>( context, pWorld, resourceID );
    }
}