#pragma once

#include "_Module/API.h"
#include "System/Resource/IResource.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class KRG_SYSTEM_NAVMESH_API NavmeshData : public Resource::IResource
    {
        KRG_REGISTER_VIRTUAL_RESOURCE( 'NAV' );
        friend class NavmeshBuilder;
        friend class NavmeshLoader;

        KRG_SERIALIZE_MEMBERS( KRG_NVP( m_graphImage ) );

    public:

        virtual bool IsValid() const override;

        inline bool HasNavmeshData() const { return !m_graphImage.empty(); }
        inline TVector<Byte> const& GetGraphImage() const { return m_graphImage; }

    private:

        TVector<Byte>   m_graphImage;
    };
}