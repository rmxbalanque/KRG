#include "ResourceLoader_PhysicsMaterialDatabase.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Physics/PhysicsMaterial.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------
namespace KRG::Physics
{
    PhysicsMaterialDatabaseLoader::PhysicsMaterialDatabaseLoader()
    {
        m_loadableTypes.push_back( PhysicsMaterialDatabase::GetStaticResourceTypeID() );
    }

    void PhysicsMaterialDatabaseLoader::SetPhysics( PhysicsSystem* pPhysicsSystem )
    {
        KRG_ASSERT( pPhysicsSystem != nullptr && m_pPhysicsSystem == nullptr );
        m_pPhysicsSystem = pPhysicsSystem;
    }

    bool PhysicsMaterialDatabaseLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() );

        TVector<PhysicsMaterialSettings> serializedMaterials;
        archive >> serializedMaterials;

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        for ( auto const& materialSettings : serializedMaterials )
        {
            KRG_ASSERT( materialSettings.IsValid() );
        }
        #endif

        //-------------------------------------------------------------------------

        m_pPhysicsSystem->FillMaterialDatabase( serializedMaterials );
        
        //-------------------------------------------------------------------------

        PhysicsMaterialDatabase* pPhysicsMaterialDB = KRG::New<PhysicsMaterialDatabase>();
        pResourceRecord->SetResourceData( pPhysicsMaterialDB );
        return true;
    }

    void PhysicsMaterialDatabaseLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
    {
        PhysicsMaterialDatabase* pPhysicsMaterialDB = pResourceRecord->GetResourceData<PhysicsMaterialDatabase>();
        if ( pPhysicsMaterialDB != nullptr )
        {
            m_pPhysicsSystem->ClearMaterialDatabase();
        }

        ResourceLoader::UnloadInternal( resID, pResourceRecord );
    }
}