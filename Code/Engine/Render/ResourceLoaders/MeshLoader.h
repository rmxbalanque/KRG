#pragma once

#include "../_Module/API.h"
#include "System/Resource/ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        class RenderDevice;
        class MeshGeometry;

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

            virtual void CreateRenderBuffers( MeshGeometry* pMeshGeometry ) const;

        private:

            RenderDevice* m_pRenderDevice = nullptr;
        };
    }
}