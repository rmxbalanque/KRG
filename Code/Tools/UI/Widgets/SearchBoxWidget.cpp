#include "SearchBoxWidget.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "QBoxLayout"

//-------------------------------------------------------------------------

namespace KRG
{
    KSearchBoxWidget::KSearchBoxWidget( QWidget* pParent )
        : QWidget( pParent )
    {
        m_pFilterBox = new QLineEdit();
        m_pFilterBox->setPlaceholderText( "Search..." );
        connect( m_pFilterBox, &QLineEdit::textChanged, this, &KSearchBoxWidget::OnTextChanged );

        //-------------------------------------------------------------------------

        m_pClearButton = new QPushButton( KIcon( ForkAwesome::Window_Close, QColor( 255, 255, 255 ) ), "" );
        m_pClearButton->setFlat( true );
        m_pClearButton->hide();

        connect( m_pClearButton, &QPushButton::clicked, this, &KSearchBoxWidget::Clear );

        //-------------------------------------------------------------------------

        auto pFilterLayout = new QHBoxLayout();
        pFilterLayout->setMargin( 0 );
        pFilterLayout->setSpacing( 0 );
        pFilterLayout->addWidget( m_pFilterBox, 1 );
        pFilterLayout->addWidget( m_pClearButton, 0 );

        setLayout( pFilterLayout );
    }

    void KSearchBoxWidget::SetPlaceholderText( QString const& text )
    {
        m_pFilterBox->setPlaceholderText( text );
    }

    void KSearchBoxWidget::OnTextChanged( QString const& filterText )
    {
        if ( filterText.isEmpty() )
        {
            m_pClearButton->hide();
        }
        else
        {
            m_pClearButton->show();
        }

        emit SearchInputChanged( filterText );
    }

    void KSearchBoxWidget::Clear()
    {
        m_pFilterBox->clear();
    }
}