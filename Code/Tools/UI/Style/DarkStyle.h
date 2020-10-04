#pragma once
#include "../_Module/API.h"
#include "QProxyStyle"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Tools
    {
        namespace Style
        {
            class DarkStyle final : public QProxyStyle
            {
                Q_OBJECT

            public:

                using QProxyStyle::QProxyStyle;
                virtual int styleHint( StyleHint hint, const QStyleOption *option = Q_NULLPTR, const QWidget *widget = Q_NULLPTR, QStyleHintReturn *returnData = Q_NULLPTR ) const override final;
            };

            //-------------------------------------------------------------------------

            KRG_TOOLS_UI_API void InitializeDarkStyle( bool forceReload = false );
            KRG_TOOLS_UI_API void ShutdownDarkStyle();
        }
    }
}