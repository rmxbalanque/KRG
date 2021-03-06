#include "BoolValueEditor.h"
#include "QBoxLayout"
#include "QCheckBox"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        BoolEditor::BoolEditor( bool value )
            : ValueEditorBase()
            , m_value( value )
        {
            m_pCheckbox = new QCheckBox();
            m_pCheckbox->setChecked( m_value );

            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setSpacing( 0 );
            pMainLayout->setMargin( 0 );
            pMainLayout->addWidget( m_pCheckbox, 1.0f );

            //-------------------------------------------------------------------------

            auto onValueChanged = [this]( int32 newState )
            {
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( newState != 0 );
                }
            };
            connect( m_pCheckbox, &QCheckBox::stateChanged, this, onValueChanged );
        }

        void BoolEditor::SetValueInternal( bool newValue )
        {
            bool oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableChangeHandlers( m_enableInputChangeHandlers, false );
                m_pCheckbox->setChecked( newValue );
            }

            m_pCheckbox->setFocus();
            emit OnValueChanged( m_value, oldValue );
        }
    }
}