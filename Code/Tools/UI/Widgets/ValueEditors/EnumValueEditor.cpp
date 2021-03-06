#include "EnumValueEditor.h"
#include "QBoxLayout"
#include "QComboBox"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        EnumEditor::EnumEditor( QStringList const& stringValues, int64 value )
            : ValueEditorBase()
            , m_enumStringValues( stringValues )
            , m_value( value )
        {
            KRG_ASSERT( stringValues.size() > 0 && IsValidEnumValue( value ) );

            m_pInput = new QComboBox();
            m_pInput->addItems( stringValues );
            m_pInput->setCurrentIndex( m_value );

            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setSpacing( 0 );
            pMainLayout->setMargin( 0 );
            pMainLayout->addWidget( m_pInput, 1.0f );

            //-------------------------------------------------------------------------

            connect( m_pInput, QOverload<int32>::of( &QComboBox::currentIndexChanged ), this, [this] ( int32 newIndex ) { SetValueInternal( newIndex ); } );
        }

        EnumEditor::EnumEditor( QStringList const& stringValues, QString const& value )
            : EnumEditor( stringValues, stringValues.indexOf( value ) )
        {}

        void EnumEditor::SetValueInternal( int64 newValue )
        {
            KRG_ASSERT( IsValidEnumValue( newValue ) );

            int64 oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableInputChangeHandler( m_enableInputChangeHandlers, false );
                m_pInput->setCurrentIndex( m_value );
            }

            m_pInput->setFocus();
            emit OnValueChanged( m_enumStringValues, m_value, oldValue );
        }

        void EnumEditor::SetValue( QString const& value )
        {
            int64 const valueIdx = GetIndexForStringValue( value );
            if ( valueIdx != InvalidIndex )
            {
                SetValueInternal( valueIdx );
            }
        }
    }
}