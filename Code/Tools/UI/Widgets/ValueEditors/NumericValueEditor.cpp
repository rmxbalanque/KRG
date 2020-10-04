#include "NumericValueEditor.h"
#include "Tools/UI/Widgets/LongLongSpinBoxWidget.h"
#include "QBoxLayout"
#include "QSpinBox"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        SignedIntegerEditor::SignedIntegerEditor( S64 value )
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

            auto onValueChanged = [this] ( S64 newValue )
            {
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( newValue );
                }
            };

            connect( m_pSpinBox, QOverload<S64>::of( &KLongLongSpinBox::valueChanged ), this, onValueChanged );
        }

        void SignedIntegerEditor::SetValueInternal( S64 newValue )
        {
            S64 const oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableChangeHandlers( m_enableInputChangeHandlers, false );
                m_pSpinBox->setValue( newValue );
            }

            m_pSpinBox->setFocus();
            emit OnValueChanged( m_value, oldValue );
        }

        void SignedIntegerEditor::SetRange( S64 min, S64 max )
        {
            m_pSpinBox->setRange( min, max );
        }

        void SignedIntegerEditor::SetWrapping( bool isWrappingEnabled )
        {
            m_pSpinBox->setWrapping( isWrappingEnabled );
        }

        //-------------------------------------------------------------------------

        UnsignedIntegerEditor::UnsignedIntegerEditor( U64 value )
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

            auto onValueChanged = [this] ( U64 newValue )
            {
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( newValue );
                }
            };

            connect( m_pSpinBox, QOverload<U64>::of( &KULongLongSpinBox::valueChanged ), this, onValueChanged );
        }

        void UnsignedIntegerEditor::SetValueInternal( U64 newValue )
        {
            U64 const oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableChangeHandlers( m_enableInputChangeHandlers, false );
                m_pSpinBox->setValue( newValue );
            }

            m_pSpinBox->setFocus();
            emit OnValueChanged( m_value, oldValue );
        }

        void UnsignedIntegerEditor::SetRange( U64 min, U64 max )
        {
            m_pSpinBox->setRange( min, max );
        }

        void UnsignedIntegerEditor::SetWrapping( bool isWrappingEnabled )
        {
            m_pSpinBox->setWrapping( isWrappingEnabled );
        }

        //-------------------------------------------------------------------------

        FloatingPointEditor::FloatingPointEditor( F64 value )
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

            auto onValueChanged = [this] ( F64 newValue )
            {
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( newValue );
                }
            };

            connect( m_pSpinBox, QOverload<F64>::of( &QDoubleSpinBox::valueChanged ), this, onValueChanged );
        }

        void FloatingPointEditor::SetValueInternal( F64 newValue )
        {
            F64 const oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableChangeHandlers( m_enableInputChangeHandlers, false );
                m_pSpinBox->setValue( newValue );
            }

            m_pSpinBox->setFocus();
            emit OnValueChanged( m_value, oldValue );
        }

        void FloatingPointEditor::SetRange( F64 min, F64 max )
        {
            m_pSpinBox->setRange( min, max );
        }

        void FloatingPointEditor::SetWrapping( bool isWrappingEnabled )
        {
            m_pSpinBox->setWrapping( isWrappingEnabled );
        }
    }
}