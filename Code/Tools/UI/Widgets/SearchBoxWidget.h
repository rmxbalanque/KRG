#pragma once
#include "../_Module/API.h"
#include "QWidget"
#include "QLineEdit"
#include "QPushButton"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_TOOLS_UI_API KSearchBoxWidget : public QWidget
    {
        Q_OBJECT
    
    signals:

        void SearchInputChanged( QString const& text );

    public:

        explicit KSearchBoxWidget( QWidget* pParent = nullptr );

        void SetPlaceholderText( QString const& text );
        void Clear();

    private:

        void OnTextChanged( QString const& filterText );

    private:

        QLineEdit*                   m_pFilterBox = nullptr;
        QPushButton*                 m_pClearButton = nullptr;
    };
}
