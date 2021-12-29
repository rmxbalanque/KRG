#pragma once

#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Resource/ResourceID.h"

namespace KRG {}

//-------------------------------------------------------------------------

namespace KRG
{
    class EditorContext;
    namespace Resource { struct ResourceDescriptor; }

    //-------------------------------------------------------------------------

    class ResourceDescriptorCreator
    {
        constexpr static char const* s_title = "Create Descriptor";

    public:

        ResourceDescriptorCreator( EditorContext* pModel, TypeSystem::TypeID const descriptorTypeID, FileSystem::Path const& startingDir );
        virtual ~ResourceDescriptorCreator();

        bool Draw();

    private:

        bool SaveDescriptor();

    protected:

        EditorContext*                                m_pModel = nullptr;
        Resource::ResourceDescriptor*               m_pDescriptor = nullptr;
        PropertyGrid                                m_propertyGrid;
        FileSystem::Path                            m_startingPath;
    };
}