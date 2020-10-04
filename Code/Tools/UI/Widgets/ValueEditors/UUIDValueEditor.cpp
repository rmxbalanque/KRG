#include "UUIDValueEditor.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "QBoxLayout"
#include "QLineEdit"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        UUIDEditor::UUIDEditor( UUID const& value )
            : ValueEditorBase()
            , m_value( value )
        {
            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setContentsMargins( 0, 0, 0, 0 );
            pMainLayout->setSpacing( 0 );

            //-------------------------------------------------------------------------

            m_pIDBox = new QLineEdit();
            m_pIDBox->setReadOnly( true );
            m_pIDBox->setPlaceholderText( "Invalid" );
            pMainLayout->addWidget( m_pIDBox, 1.0f );

            //-------------------------------------------------------------------------

            auto pGenerateButton = new QPushButton();
            pGenerateButton->setIcon( KIcon( MaterialDesign::Dice_6 ) );
            pGenerateButton->setStyleSheet( "QPushButton{ padding: 0px 0px 0px 0px; margin: 0px -2px 0px 0px; }" );
            pGenerateButton->setToolTip( "Generate new UUID" );
            pGenerateButton->setFlat( true );
            pMainLayout->addWidget( pGenerateButton );

            connect( pGenerateButton, &QPushButton::clicked, this, [this] () { SetValueInternal( UUID::GenerateID() ); } );

            //-------------------------------------------------------------------------

            UpdateDisplay();
        }

        void UUIDEditor::UpdateDisplay()
        {
            if ( m_value.IsValid() )
            {
                m_pIDBox->setText( m_value.ToString().c_str() );
            }
            else
            {
                m_pIDBox->setText( "" );
            }
        }

        void UUIDEditor::SetValueInternal( UUID newValue )
        {
            UUID oldValue = m_value;

            m_value = newValue;
            UpdateDisplay();
            emit OnValueChanged( m_value, oldValue );
        }
    }
}