#pragma once

#include "Game/Core/_Module/API.h"
#include "Engine/Core/Components/Component_Volumes.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Drawing{ class DrawContext; }

    //-------------------------------------------------------------------------

    enum class CoverType
    {
        KRG_REGISTER_ENUM

        Low = 0,
        HighShootLeft,
        HighShootRight,
        HighHidden,
    };

    //-------------------------------------------------------------------------

    class KRG_GAME_CORE_API CoverVolumeComponent : public BoxVolumeComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( CoverVolumeComponent );

    public:

        inline CoverVolumeComponent() = default;
        inline CoverVolumeComponent( StringID name ) : BoxVolumeComponent( name ) {}

        #if KRG_DEVELOPMENT_TOOLS
        virtual Color GetVolumeColor() const override { return Colors::GreenYellow; }
        virtual void Draw( Drawing::DrawContext& drawingCtx ) const override;
        #endif

    private:

        KRG_EXPOSE CoverType    m_coverType = CoverType::HighHidden;
    };
}