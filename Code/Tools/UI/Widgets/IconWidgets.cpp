#include "IconWidgets.h"
#include <QIconEngine>
#include <QFontDatabase>
#include <QApplication>
#include <QPainter>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace IconFonts
    {
        static int32          g_loadedFontIDs[2];

        //-------------------------------------------------------------------------

        void Initialize()
        {
            g_loadedFontIDs[0] = QFontDatabase::addApplicationFont( ":/KRG/Fonts/forkawesome-webfont.ttf" );
            KRG_ASSERT( g_loadedFontIDs[0] != InvalidIndex );

            g_loadedFontIDs[1] = QFontDatabase::addApplicationFont( ":/KRG/Fonts/materialdesignicons-webfont.ttf" );
            KRG_ASSERT( g_loadedFontIDs[1] != InvalidIndex );
        }

        void Shutdown()
        {
            QFontDatabase::removeApplicationFont( g_loadedFontIDs[0] );
            g_loadedFontIDs[0] = InvalidIndex;

            QFontDatabase::removeApplicationFont( g_loadedFontIDs[1] );
            g_loadedFontIDs[1] = InvalidIndex;
        }

        //-------------------------------------------------------------------------

        class QFontIconEngine final : public QIconEngine
        {
        public:

            QFontIconEngine( QChar const& code, QColor const& baseColor, QString const& fontFamily, bool drawShadowed = true )
                : QIconEngine()
                , m_code( code )
                , m_baseColor( baseColor )
                , m_fontFamily( fontFamily )
                , m_drawShadow( drawShadowed )
            {
                KRG_ASSERT( g_loadedFontIDs[0] != InvalidIndex && g_loadedFontIDs[1] != InvalidIndex );
            }

            virtual void paint( QPainter* painter, QRect const& rect, QIcon::Mode mode, QIcon::State state ) override
            {
                Q_UNUSED( state );

                // Color
                //-------------------------------------------------------------------------

                QColor penColor;
                if ( !m_baseColor.isValid() )
                {
                    penColor = QApplication::palette( "QWidget" ).color( QPalette::Normal, QPalette::ButtonText );
                }
                else
                {
                    penColor = m_baseColor;
                }

                if ( mode == QIcon::Disabled )
                {
                    penColor = QApplication::palette( "QWidget" ).color( QPalette::Disabled, QPalette::ButtonText );
                }

                if ( mode == QIcon::Selected )
                {
                    penColor = QApplication::palette( "QWidget" ).color( QPalette::Active, QPalette::ButtonText );
                }

                // Sizes
                //-------------------------------------------------------------------------

                int32 const drawSize = rect.height();
                int32 const shadowOffset = drawSize > 16 ? 2 : 1;

                //-------------------------------------------------------------------------

                painter->save();

                QFont font = QFont( m_fontFamily );
                font.setPixelSize( drawSize );

                // Draw shadow
                if ( m_drawShadow )
                {
                    QRect shadowRect = rect;
                    shadowRect.adjust( shadowOffset, shadowOffset, shadowOffset * 2, shadowOffset * 2 );
                    painter->setFont( font );
                    painter->setPen( QPen( QColor( 33, 33, 33 ) ) );
                    painter->drawText( shadowRect, Qt::AlignCenter | Qt::AlignVCenter, m_code );
                }

                // Draw icon
                painter->setFont( font );
                painter->setPen( QPen( penColor ) );
                painter->drawText( rect, Qt::AlignCenter | Qt::AlignVCenter, m_code );
                painter->restore();
            }

            virtual QPixmap pixmap( QSize const& size, QIcon::Mode mode, QIcon::State state ) override
            {
                QPixmap pix( size );
                pix.fill( Qt::transparent );

                QPainter painter( &pix );
                paint( &painter, QRect( QPoint( 0, 0 ), size ), mode, state );
                return pix;
            }

            virtual QIconEngine* clone() const override
            {
                QFontIconEngine* engine = new QFontIconEngine( m_code, m_baseColor, m_fontFamily, m_drawShadow );
                return engine;
            }

        private:

            QString     m_fontFamily;
            QColor      m_baseColor;
            QChar       m_code;
            bool        m_drawShadow = false;
        };
    }

    //-------------------------------------------------------------------------

    KIcon::KIcon( ForkAwesome codePoint, QColor const& baseColor )
        : QIcon( new IconFonts::QFontIconEngine( (int32) codePoint, baseColor, QFontDatabase::applicationFontFamilies( IconFonts::g_loadedFontIDs[0] ).first() ) )
    {}

    KIcon::KIcon( MaterialDesign codePoint, QColor const& baseColor )
        : QIcon( new IconFonts::QFontIconEngine( (int32) codePoint, baseColor, QFontDatabase::applicationFontFamilies( IconFonts::g_loadedFontIDs[1] ).first() ) )
    {}

    //-------------------------------------------------------------------------

    KIconLabel::KIconLabel( ForkAwesome codePoint, int32 w, int32 h, QColor const& baseColor )
        : QLabel()
    {
        KRG_ASSERT( w > 0 && h > 0 );
        setPixmap( KIcon( codePoint, baseColor ).pixmap( QSize( w, h ) ) );
    }

    KIconLabel::KIconLabel( MaterialDesign codePoint, int32 w, int32 h, QColor const& baseColor )
        : QLabel()
    {
        KRG_ASSERT( w > 0 && h > 0 );
        setPixmap( KIcon( codePoint, baseColor ).pixmap( QSize( w, h ) ) );
    }

    KIconLabel::KIconLabel( QIcon const& icon, int32 w, int32 h )
        : QLabel()
    {
        KRG_ASSERT( w > 0 && h > 0 );
        setPixmap( icon.pixmap( QSize( w, h ) ) );
    }
}