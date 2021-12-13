#pragma once
#include "RawResourceInspector.h"

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    class ResourceInspectorImageFormats : public RawResourceInspector
    {
    public:

        ResourceInspectorImageFormats( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDatabase, FileSystem::Path const& filePath );

    private:

        virtual char const* GetInspectorTitle() const override { return  "Image Inspector"; }
        virtual void DrawFileInfoAndContents() override;
        virtual void DrawResourceDescriptorCreator() override;
    };
}