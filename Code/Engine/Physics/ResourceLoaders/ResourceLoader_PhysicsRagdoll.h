#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsSystem;

    //-------------------------------------------------------------------------

    class RagdollLoader final : public Resource::ResourceLoader
    {
    public:

        RagdollLoader();
        ~RagdollLoader() { KRG_ASSERT( m_pPhysicsSystem == nullptr ); }

        void SetPhysics( PhysicsSystem* pPhysicsSystem );
        void ClearPhysics() { m_pPhysicsSystem = nullptr; }

    private:

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const override;
        Resource::InstallResult Install( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const override;

    private:

        PhysicsSystem* m_pPhysicsSystem = nullptr;
    };
}