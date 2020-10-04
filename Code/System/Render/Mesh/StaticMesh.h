#pragma once
#include "MeshGeometry.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        class KRG_SYSTEM_RENDER_API StaticMesh : public MeshGeometry, public Resource::IResource
        {
            KRG_REGISTER_RESOURCE( 'MSH' );
            friend class MeshCompiler;
            friend class MeshLoader;

            KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( MeshGeometry ) );

        public:

            virtual bool IsValid() const override;
        };
    }
}