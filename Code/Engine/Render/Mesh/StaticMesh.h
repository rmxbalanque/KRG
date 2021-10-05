#pragma once
#include "RenderMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Render
{
    class KRG_ENGINE_RENDER_API StaticMesh : public Mesh
    {
        KRG_REGISTER_RESOURCE( 'MSH', "Static Mesh" );
        KRG_SERIALIZE_MEMBERS( KRG_SERIALIZE_BASE( Mesh ) );
    };
}