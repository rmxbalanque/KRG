#pragma once

#include "Tools/Animation/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "Engine/Animation/Graph/Animation_RuntimeGraph_Resources.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class EditorGraphDefinition;
    class EditorGraphCompilationContext;

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_ANIMATION_API GraphResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( GraphResourceDescriptor );

        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override { return GraphDefinition::GetStaticResourceTypeID(); }
    };

    //-------------------------------------------------------------------------

    struct KRG_TOOLS_ANIMATION_API GraphVariationResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( GraphVariationResourceDescriptor );

    public:

        KRG_EXPOSE ResourcePath                 m_graphPath;
        KRG_EXPOSE StringID                     m_variationID; // Optional: if not set, will use the default variation
    };
}