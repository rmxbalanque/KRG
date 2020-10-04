#include "LongLongSpinBoxWidget.h"
#include "System/Core/Core.h"
#include <QtWidgets/QLineEdit>
#include "qevent.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace
    {
        static QRegularExpression const g_matchAllNonAlphaNumeric( "[-`~!@#$%^&*()_—+=|:;<>«»,.?/{}\'\"\\[\\]\\\\]" );
    }

    //-------------------------------------------------------------------------

    KLongLongSpinBox::KLongLongSpinBox( QWidget *parent ) 
        : QAbstractSpinBox( parent )
    {
        setValue( m_value );
    }

    void KLongLongSpinBox::setValue( qlonglong newValue )
    {
        qlonglong const clampedValue = qBound( m_minimum, newValue, m_maximum );
        if ( m_value != clampedValue )
        {
            QString const valueString = QString::number( clampedValue );
            lineEdit()->setText( m_prefix + valueString + m_suffix );

            m_value = clampedValue;
            emit valueChanged( clampedValue );
            emit valueChanged( valueString );
        }
    }

    void KLongLongSpinBox::setPrefix( const QString &prefix )
    {
        m_prefix = prefix;
        setValue( m_value );
    }

    void KLongLongSpinBox::setSuffix( const QString &suffix )
    {
        m_suffix = suffix;
        setValue( m_value );
    }

    void KLongLongSpinBox::setMinimum( qlonglong min )
    {
        m_minimum = min;
        if ( m_maximum < m_minimum )
        {
            m_maximum = m_minimum;
        }

        setValue( m_value );
    }

    void KLongLongSpinBox::setMaximum( qlonglong max )
    {
        m_maximum = max;
        if ( m_maximum < m_minimum )
        {
            m_maximum = m_minimum;
        }

        setValue( m_value );
    }

    void KLongLongSpinBox::setRange( qlonglong min, qlonglong max )
    {
        if ( min < max )
        {
            m_minimum = min;
            m_maximum = max;
        }
        else
        {
            m_minimum = max;
            m_maximum = min;
        }

        setValue( m_value );
    }

    void KLongLongSpinBox::keyPressEvent( QKeyEvent *event )
    {
        switch ( event->key() )
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            {
                selectCleanText();
                lineEditEditingFinalize();
            }
            break;
        }

        QAbstractSpinBox::keyPressEvent( event );
    }

    void KLongLongSpinBox::focusOutEvent( QFocusEvent *event )
    {
        lineEditEditingFinalize();
        QAbstractSpinBox::focusOutEvent( event );
    }

    QAbstractSpinBox::StepEnabled KLongLongSpinBox::stepEnabled() const
    {
        StepEnabled se = QAbstractSpinBox::StepNone;

        if ( isReadOnly() )
        {
            return se;
        }

        //-------------------------------------------------------------------------

        if ( wrapping() || m_value < m_maximum )
        {
            se |= StepUpEnabled;
        }
        if ( wrapping() || m_value > m_minimum )
        {
            se |= StepDownEnabled;
        }

        return se;
    }

    void KLongLongSpinBox::stepBy( int steps )
    {
        if ( isReadOnly() )
        {
            return;
        }

        if ( m_prefix + QString::number( m_value ) + m_suffix != lineEdit()->text() )
        {
            lineEditEditingFinalize();
        }

        // This emulates the behavior of the Qt spinbox, in that it clamps to the min/max when it would overflow and requires a second step to wrap around
        qlonglong newValue = m_value + ( steps * m_singleStep );
        if ( wrapping() )
        {
            if ( newValue > m_maximum )
            {
                if ( m_value == m_maximum )
                {
                    newValue = m_minimum;
                }
                else
                {
                    newValue = m_maximum;
                }
            }
            else if ( newValue < m_minimum )
            {
                if ( m_value == m_minimum )
                {
                    newValue = m_maximum;
                }
                else
                {
                    newValue = m_minimum;
                }
            }
        }
        else
        {
            newValue = qBound( m_minimum, newValue, m_maximum );
        }

        setValue( newValue );
        selectCleanText();
    }

    QValidator::State KLongLongSpinBox::validate( QString& textValue, int& position ) const
    {
        qlonglong newValue;
        return validateInternal( textValue, position, newValue ) ? QValidator::Acceptable : QValidator::Invalid;
    }

    void KLongLongSpinBox::lineEditEditingFinalize()
    {
        QString text = lineEdit()->text();
        int position = 0;
        qlonglong newValue;

        if ( validateInternal( text, position, newValue ) )
        {
            setValue( m_value );
        }
        else // Set to previous value
        {
            setValue( m_value );
        }
    }

    void KLongLongSpinBox::selectCleanText()
    {
        lineEdit()->setSelection( m_prefix.length(), lineEdit()->text().length() - m_prefix.length() - m_suffix.length() );
    }

    bool KLongLongSpinBox::validateInternal( QString& inputText, int& position, qlonglong& outValue ) const
    {
        if ( inputText.isEmpty() )
        {
            return true;
        }

        inputText = inputText.remove( m_prefix ).remove( m_suffix ).remove( ',' ).remove( ' ' );

        //-------------------------------------------------------------------------

        if ( m_minimum >= 0 && inputText.startsWith( '-' ) )
        {
            return false;
        }

        if ( m_maximum < 0 && inputText.startsWith( '+' ) )
        {
            return false;
        }

        if ( inputText.length() == 1 && ( inputText.startsWith( '+' ) || inputText.startsWith( '-' ) ) )
        {
            return true;
        }

        //-------------------------------------------------------------------------

        bool ok;
        outValue = inputText.toLongLong( &ok );
        if ( !ok )
        {
            return false;
        }

        if ( outValue < m_minimum || outValue > m_maximum )
        {
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------

    KULongLongSpinBox::KULongLongSpinBox( QWidget *parent )
        : QAbstractSpinBox( parent )
    {
        setValue( m_value );
    }

    void KULongLongSpinBox::setValue( qulonglong newValue )
    {
        qulonglong const clampedValue = qBound( m_minimum, newValue, m_maximum );
        if ( m_value != clampedValue )
        {
            QString const valueString = QString::number( clampedValue );
            lineEdit()->setText( m_prefix + valueString + m_suffix );

            m_value = clampedValue;
            emit valueChanged( clampedValue );
            emit valueChanged( valueString );
        }
    }

    void KULongLongSpinBox::setPrefix( const QString &prefix )
    {
        m_prefix = prefix;
        setValue( m_value );
    }

    void KULongLongSpinBox::setSuffix( const QString &suffix )
    {
        m_suffix = suffix;
        setValue( m_value );
    }

    void KULongLongSpinBox::setMinimum( qulonglong min )
    {
        m_minimum = min;
        if ( m_maximum < m_minimum )
        {
            m_maximum = m_minimum;
        }

        setValue( m_value );
    }

    void KULongLongSpinBox::setMaximum( qulonglong max )
    {
        m_maximum = max;
        if ( m_maximum < m_minimum )
        {
            m_maximum = m_minimum;
        }

        setValue( m_value );
    }

    void KULongLongSpinBox::setRange( qulonglong min, qulonglong max )
    {
        if ( min < max )
        {
            m_minimum = min;
            m_maximum = max;
        }
        else
        {
            m_minimum = max;
            m_maximum = min;
        }

        setValue( m_value );
    }

    void KULongLongSpinBox::keyPressEvent( QKeyEvent *event )
    {
        switch ( event->key() )
        {
            case Qt::Key_Enter:
            case Qt::Key_Return:
            {
                selectCleanText();
                lineEditEditingFinalize();
            }
            break;
        }

        QAbstractSpinBox::keyPressEvent( event );
    }

    void KULongLongSpinBox::focusOutEvent( QFocusEvent *event )
    {
        lineEditEditingFinalize();
        QAbstractSpinBox::focusOutEvent( event );
    }

    QAbstractSpinBox::StepEnabled KULongLongSpinBox::stepEnabled() const
    {
        StepEnabled se = QAbstractSpinBox::StepNone;

        if ( isReadOnly() )
        {
            return se;
        }

        //-------------------------------------------------------------------------

        if ( wrapping() || m_value < m_maximum )
        {
            se |= StepUpEnabled;
        }
        if ( wrapping() || m_value > m_minimum )
        {
            se |= StepDownEnabled;
        }

        return se;
    }

    void KULongLongSpinBox::stepBy( int steps )
    {
        if ( isReadOnly() )
        {
            return;
        }

        if ( m_prefix + QString::number( m_value ) + m_suffix != lineEdit()->text() )
        {
            lineEditEditingFinalize();
        }

        // This emulates the behavior of the Qt spinbox, in that it clamps to the min/max when it would overflow and requires a second step to wrap around
        qulonglong newValue = m_value + ( steps * m_singleStep );
        if ( wrapping() )
        {
            if ( newValue > m_maximum )
            {
                if ( m_value == m_maximum )
                {
                    newValue = m_minimum;
                }
                else
                {
                    newValue = m_maximum;
                }
            }
            else if ( newValue < m_minimum )
            {
                if ( m_value == m_minimum )
                {
                    newValue = m_maximum;
                }
                else
                {
                    newValue = m_minimum;
                }
            }
        }
        else
        {
            newValue = qBound( m_minimum, newValue, m_maximum );
        }

        setValue( newValue );
        selectCleanText();
    }

    QValidator::State KULongLongSpinBox::validate( QString& textValue, int& position ) const
    {
        qulonglong newValue;
        return validateInternal( textValue, position, newValue ) ? QValidator::Acceptable : QValidator::Invalid;
    }

    void KULongLongSpinBox::lineEditEditingFinalize()
    {
        QString text = lineEdit()->text();
        int position = 0;
        qulonglong newValue;

        if ( validateInternal( text, position, newValue ) )
        {
            setValue( newValue );
        }
        else // Set to previous value
        {
            setValue( m_value );
        }
    }

    void KULongLongSpinBox::selectCleanText()
    {
        lineEdit()->setSelection( m_prefix.length(), lineEdit()->text().length() - m_prefix.length() - m_suffix.length() );
    }

    bool KULongLongSpinBox::validateInternal( QString& inputText, int& position, qulonglong& outValue ) const
    {
        if ( inputText.isEmpty() )
        {
            return true;
        }

        inputText = inputText.remove( m_prefix ).remove( m_suffix );
        inputText = inputText.remove( g_matchAllNonAlphaNumeric );

        //-------------------------------------------------------------------------

        if ( m_minimum >= 0 && inputText.startsWith( '-' ) )
        {
            return false;
        }

        if ( m_maximum < 0 && inputText.startsWith( '+' ) )
        {
            return false;
        }

        if ( inputText.length() == 1 && ( inputText.startsWith( '+' ) || inputText.startsWith( '-' ) ) )
        {
            return true;
        }

        //-------------------------------------------------------------------------

        bool ok;
        outValue = inputText.toULongLong( &ok );
        if ( !ok )
        {
            return false;
        }

        if ( outValue < m_minimum || outValue > m_maximum )
        {
            return false;
        }

        return true;
    }
}