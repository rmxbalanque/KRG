#include "StringIDValueEditor.h"
#include "QBoxLayout"
#include "QLineEdit"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        StringIDEditor::StringIDEditor( StringID value )
            : ValueEditorBase()
            , m_value( value )
        {
            m_pIDBox = new QLineEdit();
            m_pIDBox->setFixedWidth( 75 );
            m_pIDBox->setStyleSheet( "QLineEdit{ background-color: #464646; padding: 0px 2px; margin: 0px 2px 0px 0px; font: bold 10px; border: 1px solid #262626; border-radius: 4px;}" );
            m_pIDBox->setSizePolicy( QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
            m_pIDBox->setReadOnly( true );

            //-------------------------------------------------------------------------

            m_pTextBox = new QLineEdit();
            m_pTextBox->setText( m_value.c_str() );
            UpdateDisplay();

            //-------------------------------------------------------------------------

            auto pEditorLayout = new QHBoxLayout( this );
            pEditorLayout->setContentsMargins( 0, 0, 0, 0 );
            pEditorLayout->setSpacing( 0 );
            pEditorLayout->addWidget( m_pIDBox );
            pEditorLayout->addWidget( m_pTextBox, 1.0f );

            //-------------------------------------------------------------------------

            auto onInputChanged = [this] ( QString const& text ) 
            {
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( StringID( text.toUtf8().data() ) ); 
                }
            };

            connect( m_pTextBox, &QLineEdit::textEdited, this, onInputChanged );
        }

        void StringIDEditor::SetValueInternal( StringID value )
        {
            StringID oldValue = m_value;
            m_value = value;

            {
                TScopedValue<bool> disableInputChangeHandler( m_enableInputChangeHandlers, false );
                m_pTextBox->setText( m_value.c_str() );
            }

            m_pTextBox->setFocus();
            UpdateDisplay();
            emit OnValueChanged( m_value, oldValue );
        }

        void StringIDEditor::UpdateDisplay()
        {
            if ( m_value.IsValid() )
            {
                QString IDStr = QString( "%1" ).arg( (U32) m_value );
                m_pIDBox->setText( IDStr );
            }
            else
            {
                m_pIDBox->setText( "Invalid" );
            }
        }
    }
}