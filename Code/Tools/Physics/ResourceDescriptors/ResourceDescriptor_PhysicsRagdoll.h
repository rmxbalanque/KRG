#pragma once

#include "Tools/Physics/_Module/API.h"
#include "Tools/Core/Resource/Compilers/ResourceDescriptor.h"
#include "Engine/Physics/PhysicsRagdoll.h"
#include "System/Animation/AnimationSkeleton.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    struct KRG_TOOLS_PHYSICS_API RagdollResourceDescriptor final : public Resource::ResourceDescriptor
    {
        KRG_REGISTER_TYPE( RagdollResourceDescriptor );
    
        virtual bool IsUserCreateableDescriptor() const override { return true; }
        virtual ResourceTypeID GetCompiledResourceTypeID() const override{ return RagdollDefinition::GetStaticResourceTypeID(); }

    public:

        KRG_EXPOSE RagdollDefinition    m_definition;
    };
}