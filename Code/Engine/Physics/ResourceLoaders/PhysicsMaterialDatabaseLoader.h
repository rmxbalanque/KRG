#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsSystem;

    //-------------------------------------------------------------------------

    class PhysicsMaterialDatabaseLoader : public Resource::ResourceLoader
    {
    public:

        PhysicsMaterialDatabaseLoader();
        ~PhysicsMaterialDatabaseLoader() { KRG_ASSERT( m_pPhysicsSystem == nullptr ); }

        void SetPhysics( PhysicsSystem* pPhysicsSystem );
        void ClearPhysics() { m_pPhysicsSystem = nullptr; }

    private:

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const override final;
        virtual void UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const override final;

    private:

        PhysicsSystem* m_pPhysicsSystem = nullptr;
    };
}