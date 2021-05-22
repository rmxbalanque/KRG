#pragma once

#include "Tools/Resource/Compilers/ResourceCompiler.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Math/Transform.h"

#include <bfxBuilder.h>

//-------------------------------------------------------------------------

namespace KRG::Resource { struct CompileContext; } 
namespace KRG::EntityModel { class EntityCollectionDescriptor; }

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class NavmeshData;

    //-------------------------------------------------------------------------

    class NavmeshBuilder
    {
        static const int32 s_version = 1;

    public:

        bool Build( Resource::CompileContext const& ctx, EntityModel::EntityCollectionDescriptor const& entityCollectionDesc, FileSystem::Path const& navmeshResourcePath );

    private:

        bool CollectCollisionPrimitives( Resource::CompileContext const& ctx, EntityModel::EntityCollectionDescriptor const& entityCollectionDesc, THashMap<DataPath, TVector<Transform>>& collisionPrimitives );
        bool CollectTriangles( Resource::CompileContext const& ctx, THashMap<DataPath, TVector<Transform>> const& collisionPrimitives );
        bool BuildNavmesh( Resource::CompileContext const& ctx, NavmeshData& navmeshData );

        bool Error( char const* pFormat, ... ) const;
        void Warning( char const* pFormat, ... ) const;
        void Message( char const* pFormat, ... ) const;

    private:

        TVector<bfx::BuildFace>         m_buildFaces;
        TVector<bfx::BuildParams>       m_buildParams;
    };
}