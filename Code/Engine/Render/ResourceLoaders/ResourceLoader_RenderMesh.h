#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class RenderDevice;
    class Mesh;

    //-------------------------------------------------------------------------

    class MeshLoader final : public Resource::ResourceLoader
    {
    public:

        MeshLoader();

        inline void SetRenderDevice( RenderDevice* pRenderDevice )
        {
            KRG_ASSERT( pRenderDevice != nullptr );
            m_pRenderDevice = pRenderDevice;
        }

        inline void ClearRenderDevice() { m_pRenderDevice = nullptr; }

    private:

        virtual bool LoadInternal( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const override;
        virtual Resource::InstallResult Install( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const override;
        virtual Resource::InstallResult UpdateInstall( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord ) const override;
        virtual void Uninstall( ResourceID const& resourceID, Resource::ResourceRecord* pResourceRecord ) const override;

    private:

        RenderDevice* m_pRenderDevice = nullptr;
    };
}