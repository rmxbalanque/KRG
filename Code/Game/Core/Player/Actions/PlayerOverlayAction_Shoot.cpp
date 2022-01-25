#include "PlayerOverlayAction_Shoot.h"
//-------------------------------------------------------------------------
#include "Engine/Physics/Components/Component_PhysicsCharacter.h"
#include "Game/Core/Player/PlayerCameraController.h"
#include "Game/Core/Weapon/Components/Ammo.h"
#include "Game/Core/Weapon/Components/BaseWeapon.h"
#include "System/Input/InputSystem.h"
//-------------------------------------------------------------------------

namespace KRG::Player
{
    bool ShootOverlayAction::TryStartInternal( ActionContext const& ctx )
    {
        if ( ctx.m_pInputState->GetControllerState()->GetRightTriggerValue() >= 0.2f )
        {
            auto const& camFwd  = ctx.m_pCameraController->GetCameraRelativeForwardVector();
            Vector const origin = ctx.m_pCharacterComponent->GetCapsulePosition();
            Vector const target = origin + camFwd*500.f;
            // hard code a weapon here?
            KRG::Weapon::BaseWeapon* weaponTest = KRG::New<KRG::Weapon::BaseWeapon>();
            weaponTest->m_pCurrentAmmo = KRG::New<KRG::Weapon::Ammo>();

            KRG::Weapon::AmmoContext ammoCtx;
            ammoCtx.m_pEntityWorldUpdateContext = ctx.m_pEntityWorldUpdateContext;
            ammoCtx.m_pPhysicsScene = ctx.m_pPhysicsScene;
            weaponTest->Shoot(ammoCtx, origin, target);

            KRG::Delete(weaponTest->m_pCurrentAmmo);
            KRG::Delete(weaponTest);
            return true;
        }

        return false;
    }

    Action::Status ShootOverlayAction::UpdateInternal( ActionContext const& ctx )
    {
        if ( ctx.m_pInputState->GetControllerState()->GetRightTriggerValue() < 0.2f )
        {
            return Status::Completed;
        }

        return Status::Interruptible;
    }

    void ShootOverlayAction::StopInternal( ActionContext const& ctx, StopReason reason )
    {

    }
}