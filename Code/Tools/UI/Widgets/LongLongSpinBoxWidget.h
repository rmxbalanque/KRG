#pragma once
#include "../_Module/API.h"
#include <QtWidgets/QAbstractSpinBox>
#include "qglobal.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class KRG_TOOLS_UI_API KLongLongSpinBox : public QAbstractSpinBox
    {
        Q_OBJECT

    signals:

        void valueChanged( qlonglong i );
        void valueChanged( QString const& text );

    public slots:

        void setValue( qlonglong value );

    public:

        explicit KLongLongSpinBox( QWidget* pParent = nullptr );

        inline QString cleanText() const { return QString::number( m_value ); }

        inline qlonglong value() const { return m_value; }
        inline QString const& prefix() const { return m_prefix; }
        inline QString const& suffix() const { return m_suffix; }
        inline qlonglong singleStep() const { return m_singleStep; }
        inline qlonglong minimum() const { return m_minimum; }
        inline qlonglong maximum() const { return m_maximum; }

        void setPrefix( QString const& prefix );
        void setSuffix( QString const& suffix );
        void setSingleStep( qlonglong newStep ) { m_singleStep = newStep; }
        void setMinimum( qlonglong min );
        void setMaximum( qlonglong max );
        void setRange( qlonglong min, qlonglong max );

    protected:

        virtual void keyPressEvent( QKeyEvent* pEvent );
        virtual void focusOutEvent( QFocusEvent* pEvent );
        virtual void stepBy( int steps );
        virtual StepEnabled stepEnabled() const;
        virtual QValidator::State validate( QString& input, int& pos ) const;

    private:

        void selectCleanText();
        void lineEditEditingFinalize();
        bool validateInternal( QString& inputText, int& position, qlonglong& outValue ) const;

    private:

        QString         m_prefix;
        QString         m_suffix;
        qlonglong       m_singleStep = 1;
        qlonglong       m_minimum = std::numeric_limits<qlonglong>::min();
        qlonglong       m_maximum = std::numeric_limits<qlonglong>::max();
        qlonglong       m_value = 0;
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_UI_API KULongLongSpinBox : public QAbstractSpinBox
    {
        Q_OBJECT

    signals:

        void valueChanged( qulonglong i );
        void valueChanged( QString const& text );

    public slots:

        void setValue( qulonglong value );

    public:

        explicit KULongLongSpinBox( QWidget* pParent = nullptr );

        inline QString cleanText() const { return QString::number( m_value ); }

        inline qulonglong value() const { return m_value; }
        inline QString const& prefix() const { return m_prefix; }
        inline QString const& suffix() const { return m_suffix; }
        inline qulonglong singleStep() const { return m_singleStep; }
        inline qulonglong minimum() const { return m_minimum; }
        inline qulonglong maximum() const { return m_maximum; }

        void setPrefix( QString const& prefix );
        void setSuffix( QString const& suffix );
        void setSingleStep( qulonglong newStep ) { m_singleStep = newStep; }
        void setMinimum( qulonglong min );
        void setMaximum( qulonglong max );
        void setRange( qulonglong min, qulonglong max );

    protected:

        virtual void keyPressEvent( QKeyEvent* pEvent );
        virtual void focusOutEvent( QFocusEvent* pEvent );
        virtual void stepBy( int steps );
        virtual StepEnabled stepEnabled() const;
        virtual QValidator::State validate( QString& input, int& pos ) const;

    private:

        void selectCleanText();
        void lineEditEditingFinalize();
        bool validateInternal( QString& inputText, int& position, qulonglong& outValue ) const;

    private:

        QString         m_prefix;
        QString         m_suffix;
        qulonglong       m_singleStep = 1;
        qulonglong       m_minimum = std::numeric_limits<qulonglong>::min();
        qulonglong       m_maximum = std::numeric_limits<qulonglong>::max();
        qulonglong       m_value = 0;
    };
}