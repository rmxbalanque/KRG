#include "StringValueEditor.h"
#include "QBoxLayout"
#include "QLineEdit"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        StringEditor::StringEditor( String const& value )
            : ValueEditorBase()
            , m_value( value )
        {
            m_pInputBox = new QLineEdit();
            m_pInputBox->setText( m_value.c_str() );

            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setSpacing( 0 );
            pMainLayout->setMargin( 0 );
            pMainLayout->addWidget( m_pInputBox, 1.0f );

            //-------------------------------------------------------------------------

            auto onInputChanged = [this] ( QString const& text )
            {
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( text.toUtf8().data() );
                }
            };

            connect( m_pInputBox, &QLineEdit::textEdited, this, onInputChanged );
        }

        void StringEditor::SetValueInternal( String const& value )
        {
            String oldValue = m_value;
            m_value = value;

            {
                TScopedValue<bool> disableInputChangeHandler( m_enableInputChangeHandlers, false );
                m_pInputBox->setText( m_value.c_str() );
            }

            m_pInputBox->setFocus();
            emit OnValueChanged( m_value, oldValue );
        }
    }
}