#include "CollapsibleWidget.h"
#include "QPainter"
#include "QBoxLayout"

//-------------------------------------------------------------------------

namespace KRG
{
    KCollapsibleArrowWidget::KCollapsibleArrowWidget()
        : QFrame()
    {
        m_expandedArrow << QPoint( 3, 8 ) << QPoint( 13, 8 ) << QPoint( 8, 13 );
        m_collapsedArrow << QPoint( 4, 7 ) << QPoint( 9, 12 ) << QPoint( 4, 17 );
        setFixedSize( 16, 24 );
        setMaximumSize( 16, 24 );
        setMinimumSize( 16, 24 );
    }

    void KCollapsibleArrowWidget::paintEvent( QPaintEvent* pPaintEvent )
    {
        QPainter painter( this );
        painter.setBrush( QColor( 192, 192, 192 ) );
        painter.setPen( QColor( 64, 64, 64 ) );

        if ( m_isCollapsed )
        {
            painter.drawPolygon( m_collapsedArrow );
        }
        else
        {
            painter.drawPolygon( m_expandedArrow );
        }
    }

    QSize KCollapsibleArrowWidget::sizeHint() const
    {
        return QSize( 16, 24 );
    }

    //-------------------------------------------------------------------------

    CollapsibleWidgetHeader::CollapsibleWidgetHeader( QWidget* pHeaderWidget, int32 headerIndent )
    {
        auto pMainLayout = CreateMainLayout( headerIndent );
        pMainLayout->addWidget( pHeaderWidget, 1.0f );
    }

    CollapsibleWidgetHeader::CollapsibleWidgetHeader( QLayout* pHeaderLayout, int32 headerIndent )
    {
        auto pMainLayout = CreateMainLayout( headerIndent );
        pMainLayout->addLayout( pHeaderLayout, 1.0f );
    }

    void CollapsibleWidgetHeader::mousePressEvent( QMouseEvent *event )
    {
        m_pArrow->SetArrowState( !m_pArrow->IsCollapsed() );
        emit Clicked();
    }

    QHBoxLayout* CollapsibleWidgetHeader::CreateMainLayout( int32 headerIndent )
    {
        setFixedHeight( 24 );

        m_pArrow = new KCollapsibleArrowWidget();

        auto pMainLayout = new QHBoxLayout( this );
        pMainLayout->setContentsMargins( headerIndent, 0, 0, 0 );
        pMainLayout->setSpacing( 0 );
        pMainLayout->addWidget( m_pArrow, 0.0f );

        return pMainLayout;
    }

    //-------------------------------------------------------------------------

    KCollapsibleWidget::KCollapsibleWidget( QLayout* pHeaderLayout, QLayout* pContentLayout, int32 headerIndent, int32 contentIndent )
    {
        KRG_ASSERT( pHeaderLayout != nullptr && pContentLayout != nullptr );

        //-------------------------------------------------------------------------

        auto pMainLayout = new QVBoxLayout( this );
        pMainLayout->setAlignment( Qt::AlignTop );
        pMainLayout->setMargin( 0 );
        pMainLayout->setSpacing( 0 );

        // Header
        //-------------------------------------------------------------------------

        m_pHeader = new CollapsibleWidgetHeader( pHeaderLayout, headerIndent );
        pMainLayout->addWidget( m_pHeader, 0.0f );

        // Header
        //-------------------------------------------------------------------------

        m_pContentFrame = new QWidget();
        m_pContentFrame->setVisible( !m_pHeader->IsCollapsed() );
        m_pContentFrame->setLayout( pContentLayout );

        pMainLayout->addWidget( m_pContentFrame, 1.0f );

        //-------------------------------------------------------------------------

        auto onHeaderClicked = [this] ()
        {
            m_pContentFrame->setVisible( !m_pHeader->IsCollapsed() );
        };

        connect( m_pHeader, &CollapsibleWidgetHeader::Clicked, this, onHeaderClicked );
    }
}