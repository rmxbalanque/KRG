#include "DarkStyle.h"
#include "Tools/UI/Widgets/IconWidgets.h"

#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include <QStyleFactory>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Tools
    {
        namespace Style
        {
            int DarkStyle::styleHint( StyleHint hint, const QStyleOption *option, const QWidget *widget, QStyleHintReturn *returnData ) const
            {
                if ( hint == SH_ToolTip_WakeUpDelay )
                {
                    return 450;
                }
                else if ( hint == SH_ToolTip_FallAsleepDelay )
                {
                    return 200;
                }
                else
                {
                    return baseStyle()->styleHint( hint, option, widget, returnData );
                }
            }

            //-------------------------------------------------------------------------

            void InitializeDarkStyle( bool forceReload )
            {
                KRG_ASSERT( qApp != nullptr );

                QFontDatabase::removeAllApplicationFonts();
                IconFonts::Initialize();

                //-------------------------------------------------------------------------

                // Hard coded path to style for development use only
                QFile File( forceReload ? "D:\\Kruger\\Code\\Tools\\UI\\Style\\DarkStyle.qss" : ":/KRG/Style/DarkStyle.qss" );
                File.open( QFile::ReadOnly );
                qApp->setStyleSheet( File.readAll() );

                //-------------------------------------------------------------------------

                auto FusionStyle = QStyleFactory::create( "Fusion" );
                qApp->setStyle( new DarkStyle( FusionStyle ) );

                QPalette darkPalette;
                darkPalette.setColor( QPalette::Window, QColor( 53, 53, 53 ) );
                darkPalette.setColor( QPalette::WindowText, Qt::white );
                darkPalette.setColor( QPalette::Disabled, QPalette::WindowText, QColor( 127, 127, 127 ) );
                darkPalette.setColor( QPalette::Base, QColor( 50, 50, 50 ) );
                darkPalette.setColor( QPalette::AlternateBase, QColor( 66, 66, 66 ) );
                darkPalette.setColor( QPalette::ToolTipBase, Qt::white );
                darkPalette.setColor( QPalette::ToolTipText, Qt::white );
                darkPalette.setColor( QPalette::Text, Qt::white );
                darkPalette.setColor( QPalette::Disabled, QPalette::Text, QColor( 127, 127, 127 ) );
                darkPalette.setColor( QPalette::Dark, QColor( 35, 35, 35 ) );
                darkPalette.setColor( QPalette::Shadow, QColor( 20, 20, 20 ) );
                darkPalette.setColor( QPalette::Button, QColor( 53, 53, 53 ) );
                darkPalette.setColor( QPalette::ButtonText, Qt::white );
                darkPalette.setColor( QPalette::Disabled, QPalette::ButtonText, QColor( 127, 127, 127 ) );
                darkPalette.setColor( QPalette::BrightText, Qt::red );
                darkPalette.setColor( QPalette::Link, QColor( "#CCCCCC" ) );
                darkPalette.setColor( QPalette::Highlight, QColor( "#CCCCCC" ) );
                darkPalette.setColor( QPalette::HighlightedText, Qt::white );
                darkPalette.setColor( QPalette::Disabled, QPalette::Highlight, QColor( 80, 80, 80 ) );
                darkPalette.setColor( QPalette::Disabled, QPalette::HighlightedText, QColor( 127, 127, 127 ) );

                qApp->setPalette( darkPalette );
            }

            void ShutdownDarkStyle()
            {
                IconFonts::Shutdown();
                QFontDatabase::removeAllApplicationFonts();
            }
        }
    }
}