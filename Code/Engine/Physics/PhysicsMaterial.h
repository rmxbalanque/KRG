#pragma once

#include "_Module/API.h"
#include "System/Resource/IResource.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "PxMaterial.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    enum class PhysicMaterialCombineMode
    {
        KRG_REGISTER_ENUM

        Average = physx::PxCombineMode::eAVERAGE,
        Min = physx::PxCombineMode::eMIN,
        Multiply = physx::PxCombineMode::eMULTIPLY,
        Max = physx::PxCombineMode::eMAX,
    };

    //-------------------------------------------------------------------------

    struct KRG_ENGINE_PHYSICS_API PhysicsMaterialSettings
    {
        KRG_REGISTER_TYPE;
        KRG_SERIALIZE_MEMBERS( m_name, m_dynamicFriction, m_staticFriction, m_restitution, m_frictionCombineMode, m_restitutionCombineMode );

        bool IsValid() const;

        EXPOSE StringID                    m_name;

        // The friction coefficients - [0, FloatMax]
        EXPOSE float                         m_dynamicFriction = 0.5f;
        EXPOSE float                         m_staticFriction = 0.5f;

        // The amount of restitution (bounciness) - [0,1]
        EXPOSE float                         m_restitution = 0.0f;

        // How material properties will be combined on collision
        EXPOSE PhysicMaterialCombineMode   m_frictionCombineMode = PhysicMaterialCombineMode::Average;
        EXPOSE PhysicMaterialCombineMode   m_restitutionCombineMode = PhysicMaterialCombineMode::Average;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsMaterial : public Resource::IResource
    {
        KRG_REGISTER_RESOURCE( 'PMAT' );
        friend class PhysicsMaterialCompiler;
        friend class PhysicsMaterialLoader;

        KRG_SERIALIZE_NONE();

    public:

        PhysicsMaterial( StringID name ) : m_name( name ) { KRG_ASSERT( name.IsValid() ); }

        virtual bool IsValid() const override;

        // KRG material properties
        inline StringID GetMaterialName() const { return m_name; }

        // PhysX material properties
        physx::PxMaterial* GetMaterial() const { return m_pMaterial; }

    private:

        StringID                    m_name;
        physx::PxMaterial*          m_pMaterial = nullptr;
    };
}