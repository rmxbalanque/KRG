#pragma once

#include "_Module/API.h"
#include "PhysX.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    //-------------------------------------------------------------------------
    // Physics Material
    //-------------------------------------------------------------------------
    // Physics material instance, created from the serialized settings

    struct KRG_ENGINE_PHYSICS_API PhysicsMaterial
    {
        constexpr static char const* const DefaultID = "Default";
        constexpr static float const DefaultStaticFriction = 0.5f;
        constexpr static float const DefaultDynamicFriction = 0.5f;
        constexpr static float const DefaultRestitution = 0.5f;

    public:

        PhysicsMaterial( StringID ID, physx::PxMaterial* pMaterial )
            : m_ID( ID )
            , m_pMaterial( pMaterial )
        {
            KRG_ASSERT( ID.IsValid() && pMaterial != nullptr );
        }

    public:

        StringID                                m_ID;
        physx::PxMaterial*                      m_pMaterial = nullptr;
    };

    enum class PhysicsCombineMode
    {
        KRG_REGISTER_ENUM

        Average = physx::PxCombineMode::eAVERAGE,
        Min = physx::PxCombineMode::eMIN,
        Multiply = physx::PxCombineMode::eMULTIPLY,
        Max = physx::PxCombineMode::eMAX,
    };

    //-------------------------------------------------------------------------
    // Material Settings
    //-------------------------------------------------------------------------
    // Serialized physical material settings

    struct KRG_ENGINE_PHYSICS_API PhysicsMaterialSettings : public IRegisteredType
    {
        KRG_REGISTER_TYPE( PhysicsMaterialSettings );
        KRG_SERIALIZE_MEMBERS( m_ID, m_dynamicFriction, m_staticFriction, m_restitution, m_frictionCombineMode, m_restitutionCombineMode );

        bool IsValid() const;

        EXPOSE StringID                         m_ID;

        // The friction coefficients - [0, FloatMax]
        EXPOSE float                            m_staticFriction = PhysicsMaterial::DefaultStaticFriction;
        EXPOSE float                            m_dynamicFriction = PhysicsMaterial::DefaultDynamicFriction;

        // The amount of restitution (bounciness) - [0,1]
        EXPOSE float                            m_restitution = PhysicsMaterial::DefaultRestitution;

        // How material properties will be combined on collision
        EXPOSE PhysicsCombineMode               m_frictionCombineMode = PhysicsCombineMode::Average;
        EXPOSE PhysicsCombineMode               m_restitutionCombineMode = PhysicsCombineMode::Average;
    };
}