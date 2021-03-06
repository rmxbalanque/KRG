#include "NumericValueEditor.h"
#include "Tools/UI/Widgets/LongLongSpinBoxWidget.h"
#include "QBoxLayout"
#include "QSpinBox"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        SignedIntegerEditor::SignedIntegerEditor( int64 value )
            : ValueEditorBase()
            , m_value( value )
        {
            m_pSpinBox = new KLongLongSpinBox();
            m_pSpinBox->setValue( m_value );
            m_pSpinBox->setWrapping( false );

            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setSpacing( 0 );
            pMainLayout->setMargin( 0 );
            pMainLayout->addWidget( m_pSpinBox, 1.0f );

            //-------------------------------------------------------------------------

            auto onValueChanged = [this] ( int64 newValue )
            {
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( newValue );
                }
            };

            connect( m_pSpinBox, QOverload<int64>::of( &KLongLongSpinBox::valueChanged ), this, onValueChanged );
        }

        void SignedIntegerEditor::SetValueInternal( int64 newValue )
        {
            int64 const oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableChangeHandlers( m_enableInputChangeHandlers, false );
                m_pSpinBox->setValue( newValue );
            }

            m_pSpinBox->setFocus();
            emit OnValueChanged( m_value, oldValue );
        }

        void SignedIntegerEditor::SetRange( int64 min, int64 max )
        {
            m_pSpinBox->setRange( min, max );
        }

        void SignedIntegerEditor::SetWrapping( bool isWrappingEnabled )
        {
            m_pSpinBox->setWrapping( isWrappingEnabled );
        }

        //-------------------------------------------------------------------------

        UnsignedIntegerEditor::UnsignedIntegerEditor( uint64 value )
            : ValueEditorBase()
            , m_value( value )
        {
            m_pSpinBox = new KULongLongSpinBox();
            m_pSpinBox->setValue( m_value );
            m_pSpinBox->setWrapping( false );

            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setSpacing( 0 );
            pMainLayout->setMargin( 0 );
            pMainLayout->addWidget( m_pSpinBox, 1.0f );

            //-------------------------------------------------------------------------

            auto onValueChanged = [this] ( uint64 newValue )
            {
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( newValue );
                }
            };

            connect( m_pSpinBox, QOverload<uint64>::of( &KULongLongSpinBox::valueChanged ), this, onValueChanged );
        }

        void UnsignedIntegerEditor::SetValueInternal( uint64 newValue )
        {
            uint64 const oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableChangeHandlers( m_enableInputChangeHandlers, false );
                m_pSpinBox->setValue( newValue );
            }

            m_pSpinBox->setFocus();
            emit OnValueChanged( m_value, oldValue );
        }

        void UnsignedIntegerEditor::SetRange( uint64 min, uint64 max )
        {
            m_pSpinBox->setRange( min, max );
        }

        void UnsignedIntegerEditor::SetWrapping( bool isWrappingEnabled )
        {
            m_pSpinBox->setWrapping( isWrappingEnabled );
        }

        //-------------------------------------------------------------------------

        FloatingPointEditor::FloatingPointEditor( double value )
            : ValueEditorBase()
            , m_value( value )
        {
            m_pSpinBox = new QDoubleSpinBox();
            m_pSpinBox->setRange( -DBL_MAX, DBL_MAX );
            m_pSpinBox->setValue( m_value );
            m_pSpinBox->setWrapping( false );

            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setSpacing( 0 );
            pMainLayout->setMargin( 0 );
            pMainLayout->addWidget( m_pSpinBox, 1.0f );

            //-------------------------------------------------------------------------

            auto onValueChanged = [this] ( double newValue )
            {
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( newValue );
                }
            };

            connect( m_pSpinBox, QOverload<double>::of( &QDoubleSpinBox::valueChanged ), this, onValueChanged );
        }

        void FloatingPointEditor::SetValueInternal( double newValue )
        {
            double const oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableChangeHandlers( m_enableInputChangeHandlers, false );
                m_pSpinBox->setValue( newValue );
            }

            m_pSpinBox->setFocus();
            emit OnValueChanged( m_value, oldValue );
        }

        void FloatingPointEditor::SetRange( double min, double max )
        {
            m_pSpinBox->setRange( min, max );
        }

        void FloatingPointEditor::SetWrapping( bool isWrappingEnabled )
        {
            m_pSpinBox->setWrapping( isWrappingEnabled );
        }
    }
}