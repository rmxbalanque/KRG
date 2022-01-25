#pragma once
#include "System/Core/Math/Vector.h"
//-------------------------------------------------------------------------

namespace KRG::Weapon
{
	enum class DamageType : uint8
	{
		Normal,
		Silver
	};

	struct DamageDealtInfo
	{
		Vector m_hitLocation	= Vector::Zero;
		float m_damageValue		= 1.f;
		DamageType m_damageType = DamageType::Normal;
	};

	struct RangedWeaponInfo
	{
		float m_baseAccuracy	= 1.f;
		float m_damageValue		= 1.f;
	};

}