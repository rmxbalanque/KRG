#pragma once
#include "DamageInfoTypes.h"
#include "System/Core/Math/Vector.h"
#include "Engine/Core/Entity/EntityWorldUpdateContext.h"

#if KRG_DEVELOPMENT_TOOLS
#include "imgui.h"
#endif

//-------------------------------------------------------------------------
namespace KRG
{
    class EntityWorldUpdateContext;
    namespace Physics { class Scene; }
}

namespace KRG::Weapon
{
    //-------------------------------------------------------------------------
    // Provides the common set of systems and components needed for ammo

    struct AmmoContext
    {
        bool IsValid() const {};

        // Forwarding helper functions
        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE Seconds GetDeltaTime() const { return m_pEntityWorldUpdateContext->GetDeltaTime(); }
        template<typename T> inline T* GetWorldSystem() const { return m_pEntityWorldUpdateContext->GetWorldSystem<T>(); }
        template<typename T> inline T* GetSystem() const { return m_pEntityWorldUpdateContext->GetSystem<T>(); }
        template<typename T> inline T* GetAnimSubGraphController() const { return m_pAnimationController->GetSubGraphController<T>(); }

        #if KRG_DEVELOPMENT_TOOLS
        KRG_FORCE_INLINE Drawing::DrawContext GetDrawingContext() const { return m_pEntityWorldUpdateContext->GetDrawingContext(); }
        #endif

    public:

        EntityWorldUpdateContext const* m_pEntityWorldUpdateContext = nullptr;
        Physics::Scene* m_pPhysicsScene = nullptr;
    };

    class Ammo
    {
    public:

        struct DmgModifiers
        {
            DamageType m_damageType		= DamageType::Normal;
            float m_damageModifier		= 1.f;
            float m_accuracyModifier	= 1.f;
        };

    public:

        virtual bool Shoot(AmmoContext const& ctx, RangedWeaponInfo weaponDamage, Vector origin, Vector target);

    private:

        DmgModifiers m_dmgModifiers;
        float m_range = 300.f;

    };
}