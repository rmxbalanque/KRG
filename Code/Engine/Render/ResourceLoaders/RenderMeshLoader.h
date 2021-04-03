#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class RenderDevice;
    class Mesh;

    //-------------------------------------------------------------------------

    class MeshLoader : public Resource::ResourceLoader
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

        virtual bool LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const final;
        virtual void UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const final;
        virtual bool Install( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Resource::InstallDependencyList const& installDependencies ) const final;

        virtual void CreateRenderBuffers( Mesh* pMesh ) const;

    private:

        RenderDevice* m_pRenderDevice = nullptr;
    };
}