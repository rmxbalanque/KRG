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

        ResourceDescriptorUndoableAction( TypeSystem::TypeRegistry const& typeRegistry, GenericResourceWorkspace* pWorkspace )
            : m_typeRegistry( typeRegistry )
            , m_pWorkspace( pWorkspace )
        {
            KRG_ASSERT( m_pWorkspace != nullptr );
            KRG_ASSERT( m_pWorkspace->m_pDescriptor != nullptr );
        }

        virtual void Undo() override
        {
            TypeSystem::Serialization::ReadNativeTypeFromString( m_typeRegistry, m_valueBefore, m_pWorkspace->m_pDescriptor );
            m_pWorkspace->m_isDirty = true;
        }

        virtual void Redo() override
        {
            TypeSystem::Serialization::ReadNativeTypeFromString( m_typeRegistry, m_valueAfter, m_pWorkspace->m_pDescriptor );
            m_pWorkspace->m_isDirty = true;
        }

        void SerializeBeforeState()
        {
            TypeSystem::Serialization::WriteNativeTypeToString( m_typeRegistry, m_pWorkspace->m_pDescriptor, m_valueBefore );
        }

        void SerializeAfterState()
        {
            TypeSystem::Serialization::WriteNativeTypeToString( m_typeRegistry, m_pWorkspace->m_pDescriptor, m_valueAfter );
            m_pWorkspace->m_isDirty = true;
        }

    private:

        TypeSystem::TypeRegistry const&     m_typeRegistry;
        GenericResourceWorkspace*           m_pWorkspace = nullptr;
        String                              m_valueBefore;
        String                              m_valueAfter;
    };

    //-------------------------------------------------------------------------

    GenericResourceWorkspace::GenericResourceWorkspace( WorkspaceInitializationContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
        : EditorWorkspace( context, pWorld, resourceID.GetFileNameWithoutExtension() )
        , m_descriptorID( resourceID )
        , m_descriptorPath( GetFileSystemPath( resourceID ) )
        , m_descriptorPropertyGrid( *context.m_pTypeRegistry, *context.m_pResourceDatabase )
    {
        KRG_ASSERT( resourceID.IsValid() );
        m_preEditEventBindingID = m_descriptorPropertyGrid.OnPreEdit().Bind( [this] ( PropertyEditInfo const& info ) { PreEdit( info ); } );
        m_postEditEventBindingID = m_descriptorPropertyGrid.OnPostEdit().Bind( [this] ( PropertyEditInfo const& info ) { PostEdit( info ); } );

        m_gizmoStartManipulationEventBindingID = m_gizmo.OnManipulationStarted().Bind( [this] () { BeginModification(); } );
        m_gizmoEndManipulationEventBindingID = m_gizmo.OnManipulationEnded().Bind( [this] () { EndModification(); } );
    }

    GenericResourceWorkspace::~GenericResourceWorkspace()
    {
        KRG_ASSERT( m_pDescriptor == nullptr );
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );

        m_descriptorPropertyGrid.OnPreEdit().Unbind( m_preEditEventBindingID );
        m_descriptorPropertyGrid.OnPostEdit().Unbind( m_postEditEventBindingID );

        m_gizmo.OnManipulationStarted().Unbind( m_gizmoStartManipulationEventBindingID );
        m_gizmo.OnManipulationEnded().Unbind( m_gizmoEndManipulationEventBindingID );
    }

    void GenericResourceWorkspace::Initialize( UpdateContext const& context )
    {
        EditorWorkspace::Initialize( context );
        m_descriptorWindowName.sprintf( "Descriptor##%u", GetID() );
        LoadDescriptor();
    }

    void GenericResourceWorkspace::Shutdown( UpdateContext const& context )
    {
        KRG::Delete( m_pDescriptor );
        EditorWorkspace::Shutdown( context );
    }

    void GenericResourceWorkspace::BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToBeReloaded, TVector<ResourceID> const& resourcesToBeReloaded )
    {
        // Destroy descriptor if the resource we are operating on was modified
        if ( VectorContains( resourcesToBeReloaded, m_descriptorID ) )
        {
            KRG::Delete( m_pDescriptor );
        }

        EditorWorkspace::BeginHotReload( usersToBeReloaded, resourcesToBeReloaded );
    }

    void GenericResourceWorkspace::EndHotReload()
    {
        EditorWorkspace::EndHotReload();

        // Reload the descriptor if needed
        if ( m_pDescriptor == nullptr )
        {
            LoadDescriptor();
        }
    }

    void GenericResourceWorkspace::LoadDescriptor()
    {
        KRG_ASSERT( m_pDescriptor == nullptr );
        TypeSystem::Serialization::TypeReader typeReader( *m_pTypeRegistry );
        if ( typeReader.ReadFromFile( m_descriptorPath ) )
        {
            TypeSystem::TypeDescriptor typeDesc;
            if ( typeReader.ReadType( typeDesc ) )
            {
                m_pDescriptor = typeDesc.CreateTypeInstance<Resource::ResourceDescriptor>( *m_pTypeRegistry );
                m_descriptorPropertyGrid.SetTypeToEdit( m_pDescriptor );
            }
        }
    }

    void GenericResourceWorkspace::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGui::DockBuilderDockWindow( m_descriptorWindowName.c_str(), dockspaceID );
    }

    void GenericResourceWorkspace::DrawUI( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
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
        
        if ( WriteResourceDescriptorToFile( *m_pTypeRegistry, m_descriptorPath, m_pDescriptor ) )
        {
            m_descriptorPropertyGrid.ClearDirty();
            m_isDirty = false;
            return true;
        }

        return false;
    }

    void GenericResourceWorkspace::PreEdit( PropertyEditInfo const& info )
    {
        KRG_ASSERT( m_pActiveUndoableAction == nullptr );
        BeginModification();
    }

    void GenericResourceWorkspace::PostEdit( PropertyEditInfo const& info )
    {
        KRG_ASSERT( m_pActiveUndoableAction != nullptr );
        EndModification();
    }

    void GenericResourceWorkspace::BeginModification()
    {
        if ( m_beginModificationCallCount == 0 )
        {
            auto pUndoableAction = KRG::New<ResourceDescriptorUndoableAction>( *m_pTypeRegistry, this );
            pUndoableAction->SerializeBeforeState();
            m_pActiveUndoableAction = pUndoableAction;
        }
        m_beginModificationCallCount++;
    }

    void GenericResourceWorkspace::EndModification()
    {
        KRG_ASSERT( m_beginModificationCallCount > 0 );
        KRG_ASSERT( m_pActiveUndoableAction != nullptr );

        m_beginModificationCallCount--;

        if ( m_beginModificationCallCount == 0 )
        {
            auto pUndoableAction = static_cast<ResourceDescriptorUndoableAction*>( m_pActiveUndoableAction );
            pUndoableAction->SerializeAfterState();
            m_undoStack.RegisterAction( pUndoableAction );
            m_pActiveUndoableAction = nullptr;
            m_isDirty = true;
        }
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

    EditorWorkspace* ResourceWorkspaceFactory::TryCreateWorkspace( WorkspaceInitializationContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
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