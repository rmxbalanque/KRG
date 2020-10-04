#pragma once

#include "Tools/UI/_Module/API.h"
#include "Tools/UI/Fonts/ForkAwesome.h"
#include "Tools/UI/Fonts/MaterialDesign.h"
#include "System/Core/Core.h"

#include <QColor>
#include <QLabel>
#include <QIcon>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace IconFonts
    {
        KRG_TOOLS_UI_API void Initialize();
        KRG_TOOLS_UI_API void Shutdown();
    }

    //-------------------------------------------------------------------------

    class KRG_TOOLS_UI_API KIcon : public QIcon
    {
    public:

        KIcon( ForkAwesome codePoint, QColor const& baseColor = QColor() );
        KIcon( MaterialDesign codePoint, QColor const& baseColor = QColor() );
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_UI_API KIconLabel : public QLabel
    {
    public:

        KIconLabel( QIcon const& icon, S32 w = 16, S32 h = 16 );
        KIconLabel( ForkAwesome codePoint, S32 w = 16, S32 h = 16, QColor const& baseColor = QColor() );
        KIconLabel( MaterialDesign codePoint, S32 w = 16, S32 h = 16, QColor const& baseColor = QColor() );
    };
}