#pragma once

#include "System/Core/Core.h"
#include "QFrame"
#include "QWidget"
#include "QPolygon"

//-------------------------------------------------------------------------

class QHBoxLayout;

//-------------------------------------------------------------------------

namespace KRG
{
    class KCollapsibleArrowWidget final : public QFrame
    {
        Q_OBJECT

    public:

        KCollapsibleArrowWidget();

        inline void SetArrowState( bool isCollapsed ) { m_isCollapsed = isCollapsed; }
        inline bool IsCollapsed() const { return m_isCollapsed; }

    private:

        virtual QSize sizeHint() const override;
        virtual void paintEvent( QPaintEvent* pPaintEvent ) override;

    private:

        QPolygon    m_collapsedArrow;
        QPolygon    m_expandedArrow;
        bool        m_isCollapsed = false;
    };

    //-------------------------------------------------------------------------

    class CollapsibleWidgetHeader : public QWidget
    {
        Q_OBJECT

    signals:

        void Clicked();

    public:

        CollapsibleWidgetHeader( QWidget* pHeaderWidget, S32 headerIndent = 0 );
        CollapsibleWidgetHeader( QLayout* pHeaderLayout, S32 headerIndent = 0 );

        bool IsCollapsed() const { return m_pArrow->IsCollapsed(); }

    protected:

        virtual void mousePressEvent( QMouseEvent *event ) override;

    private:

        QHBoxLayout* CreateMainLayout( S32 headerIndent );

    private:

        KCollapsibleArrowWidget*     m_pArrow;
    };

    //-------------------------------------------------------------------------

    class KCollapsibleWidget : public QWidget
    {
        Q_OBJECT

    public:

        KCollapsibleWidget( QLayout* pHeaderLayout, QLayout* pContentLayout, S32 headerIndent = 0, S32 contentIndent = 0 );

    private:

        CollapsibleWidgetHeader*                        m_pHeader = nullptr;
        QWidget*                                        m_pContentFrame = nullptr;
    };
}