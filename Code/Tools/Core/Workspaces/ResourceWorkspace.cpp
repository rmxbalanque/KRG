#include "ResourceWorkspace.h"
#include "Tools/Core/TypeSystem/Serialization/TypeReader.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"

//-------------------------------------------------------------------------

namespace KRG
{
    KRG_DEFINE_GLOBAL_REGISTRY( ResourceWorkspaceFactory );

    //-------------------------------------------------------------------------

    GenericResourceWorkspace::GenericResourceWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
        : EditorWorkspace( context, pWorld, resourceID.GetResourcePath().ToFileSystemPath( context.m_sourceResourceDirectory ).GetFileNameWithoutExtension() )
        , m_descriptorID( resourceID )
        , m_descriptorPath( resourceID.GetResourcePath().ToFileSystemPath( context.m_sourceResourceDirectory ) )
        , m_propertyGrid( *context.m_pTypeRegistry, context.m_sourceResourceDirectory )
    {
        KRG_ASSERT( resourceID.IsValid() );
    }

    GenericResourceWorkspace::~GenericResourceWorkspace()
    {
        KRG_ASSERT( m_pDescriptor == nullptr );
    }

    void GenericResourceWorkspace::Initialize()
    {
        EditorWorkspace::Initialize();

        m_descriptorWindowName.sprintf( "Descriptor##%u", GetID() );

        TypeSystem::Serialization::TypeReader typeReader( *m_editorContext.m_pTypeRegistry );
        if ( typeReader.ReadFromFile( m_descriptorPath ) )
        {
            TypeSystem::TypeDescriptor typeDesc;
            if ( typeReader.ReadType( typeDesc ) )
            {
                m_pDescriptor = TypeSystem::TypeCreator::CreateTypeFromDescriptor<Resource::ResourceDescriptor>( *m_editorContext.m_pTypeRegistry, typeDesc );
                m_propertyGrid.SetTypeToEdit( m_pDescriptor );
            }
        }
    }

    void GenericResourceWorkspace::Shutdown()
    {
        KRG::Delete( m_pDescriptor );
        EditorWorkspace::Shutdown();
    }

    void GenericResourceWorkspace::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGui::DockBuilderDockWindow( m_descriptorWindowName.c_str(), dockspaceID );
    }

    void GenericResourceWorkspace::UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
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

                ImGui::BeginDisabled( !m_propertyGrid.IsDirty() );
                if ( ImGuiX::ButtonColored( Colors::LimeGreen.ToFloat4(), KRG_ICON_FLOPPY_O " Save", ImVec2( -1, 0 ) ) )
                {
                    Save();
                    m_propertyGrid.ClearDirty();
                }
                ImGui::EndDisabled();

                m_propertyGrid.DrawGrid();
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
            m_propertyGrid.ClearDirty();
            return true;
        }

        return false;
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