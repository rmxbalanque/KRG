#include "PhysicsMaterialLoader.h"
#include "Engine/Physics/PhysX.h"
#include "Engine/Physics/PhysicsMaterial.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------
namespace KRG::Physics
{
    PhysicsMaterialLoader::PhysicsMaterialLoader()
    {
        m_loadableTypes.push_back( PhysicsMaterial::GetStaticResourceTypeID() );
    }

    void PhysicsMaterialLoader::SetPhysics( PxPhysics* pPhysics )
    {
        KRG_ASSERT( pPhysics != nullptr && m_pPhysics == nullptr );
        m_pPhysics = pPhysics;
    }

    bool PhysicsMaterialLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() );

        PhysicsMaterialSettings materialSettings;
        archive >> materialSettings;
        if ( !materialSettings.IsValid() )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        // Create material
        PhysicsMaterial* pPhysicsMaterial = KRG::New<PhysicsMaterial>();
        pPhysicsMaterial->m_name = materialSettings.m_name;

        // Create physX material
        pPhysicsMaterial->m_pMaterial = m_pPhysics->createMaterial( materialSettings.m_staticFriction, materialSettings.m_dynamicFriction, materialSettings.m_restitution );
        pPhysicsMaterial->m_pMaterial->setFrictionCombineMode( (PxCombineMode::Enum) materialSettings.m_frictionCombineMode );
        pPhysicsMaterial->m_pMaterial->setRestitutionCombineMode( (PxCombineMode::Enum) materialSettings.m_restitutionCombineMode );
        pPhysicsMaterial->m_pMaterial->userData = pPhysicsMaterial;

        pResourceRecord->SetResourceData( pPhysicsMaterial );
        return true;
    }

    void PhysicsMaterialLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
    {
        PhysicsMaterial* pPhysicsMaterial = pResourceRecord->GetResourceData<PhysicsMaterial>();
        if ( pPhysicsMaterial != nullptr )
        {
            if ( pPhysicsMaterial->m_pMaterial != nullptr )
            {
                pPhysicsMaterial->m_pMaterial->release();
                pPhysicsMaterial->m_pMaterial = nullptr;
            }
        }

        ResourceLoader::UnloadInternal( resID, pResourceRecord );
    }
}