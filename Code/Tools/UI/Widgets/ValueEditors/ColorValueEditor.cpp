#include "ColorValueEditor.h"
#include "QBoxLayout"
#include "QColorDialog"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        ColorEditor::ColorEditor( Color value )
            : ValueEditorBase()
            , m_value( value )
        {
            m_pColorButton = new QPushButton( "" );
            m_pColorButton->setObjectName( "ColorButton" );
            m_pColorButton->setFlat( true );
            UpdateDisplay();

            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setSpacing( 0 );
            pMainLayout->setMargin( 0 );
            pMainLayout->addWidget( m_pColorButton, 1.0f );

            //-------------------------------------------------------------------------

            auto OnColorButtonClicked = [this] ()
            {
                QColor const currentColor( m_value.m_byteColor.m_r, m_value.m_byteColor.m_g, m_value.m_byteColor.m_b, m_value.m_byteColor.m_a );
                QColor const newColor = QColorDialog::getColor( currentColor, this );
                SetValue( Color( newColor.red(), newColor.green(), newColor.blue(), newColor.alpha() ) );
            };

            connect( m_pColorButton, &QPushButton::clicked, this, OnColorButtonClicked );
        }

        void ColorEditor::UpdateDisplay()
        {
            QColor const color( m_value.m_byteColor.m_r, m_value.m_byteColor.m_g, m_value.m_byteColor.m_b, m_value.m_byteColor.m_a );

            m_pColorButton->setStyleSheet
            (
                QString( "QPushButton{ margin: 0px 1px 0 1px; padding: 0px; background-color: %1; border: none; }" ).arg( color.name() )
            );

            setFocus();
        }

        void ColorEditor::SetValueInternal( Color newValue )
        {
            Color oldValue = m_value;

            m_value = newValue;
            UpdateDisplay();
            emit OnValueChanged( m_value, oldValue );
        }
    }
}