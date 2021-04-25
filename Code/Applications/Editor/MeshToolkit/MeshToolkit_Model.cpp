#include "MeshToolkit_Model.h"
#include "System/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshTools
{
    void Model::Initialize( UpdateContext const& context )
    {
        EditorModel::Initialize( context );

        // Create preview entity
        //-------------------------------------------------------------------------

        // We dont own the entity as soon as we add it to the map
        auto pWorld = context.GetSystem<EntityWorld>();
        auto pPersistentMap = pWorld->GetPersistentMap();

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        pPersistentMap->AddEntity( m_pPreviewEntity );
    }

    void Model::Shutdown( UpdateContext const& context )
    {
        EditorModel::Shutdown( context );

        // These need to be cleared after the base shutdown since the "OnClearActiveResource" uses them
        m_pPreviewEntity = nullptr;
    }

    void Model::OnSetActiveResource( ResourceID const& resourceID )
    {
        if ( IsPreviewingStaticMesh() )
        {
            m_pStaticMeshComponent = KRG::New<StaticMeshComponent>( StringID( "Static Mesh Component" ) );
            m_pStaticMeshComponent->SetMesh( m_activeResource );
            m_pPreviewEntity->AddComponent( m_pStaticMeshComponent );
        }
        else // Skeletal Mesh
        {
            m_pSkeletalMeshComponent = KRG::New<SkeletalMeshComponent>( StringID( "Skeletal Mesh Component" ) );
            m_pSkeletalMeshComponent->SetMesh( m_activeResource );
            m_pPreviewEntity->AddComponent( m_pSkeletalMeshComponent );
        }
    }

    void Model::OnClearActiveResource()
    {
        if ( m_pStaticMeshComponent != nullptr )
        {
            m_pPreviewEntity->DestroyComponent( m_pStaticMeshComponent->GetID() );
            m_pStaticMeshComponent = nullptr;
        }

        if ( m_pSkeletalMeshComponent != nullptr )
        {
            m_pPreviewEntity->DestroyComponent( m_pSkeletalMeshComponent->GetID() );
            m_pSkeletalMeshComponent = nullptr;
        }
    }

    bool Model::IsPreviewingStaticMesh() const
    {
        return m_activeResource.IsValid() && m_activeResource.GetResourceTypeID() == StaticMesh::GetStaticResourceTypeID();
    }

    bool Model::IsPreviewingSkeletalMesh() const
    {
        return m_activeResource.IsValid() && m_activeResource.GetResourceTypeID() == SkeletalMesh::GetStaticResourceTypeID();
    }
}