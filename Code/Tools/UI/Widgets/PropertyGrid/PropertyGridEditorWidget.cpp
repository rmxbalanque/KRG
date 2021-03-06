#include "PropertyGridEditorWidget.h"
#include "Tools/Core/TypeSystem/ToolTypeInstance.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "QBoxLayout"
#include "QPushButton"
#include "QPainter"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace PropertyGrid
    {
        PropertyBullet::PropertyBullet()
            : QFrame()
        {
            setFixedSize( 16, 24 );
            setMaximumSize( 16, 24 );
            setMinimumSize( 16, 24 );
        }

        QSize PropertyBullet::sizeHint() const
        {
            return QSize( 16, 24 );
        }

        void PropertyBullet::paintEvent( QPaintEvent* pPaintEvent )
        {
            QPainter painter( this );
            painter.setBrush( QColor( 192, 192, 192 ) );
            painter.setPen( QColor( 64, 64, 64 ) );
            painter.drawEllipse( QPoint( 7, 11 ), 3, 3 );
        }

        //-------------------------------------------------------------------------

        PropertyEditor::PropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance )
            : QWidget()
            , m_pPropertyInstance( pPropertyInstance )
        {
            KRG_ASSERT( m_pPropertyInstance != nullptr );

            // Create overall layout of editor, derived classes should fill it out
            //-------------------------------------------------------------------------

            m_pMainLayout = new QGridLayout( this );
            m_pMainLayout->setContentsMargins( 0, 0, 0, 2 );
            m_pMainLayout->setSpacing( 0 );
            m_pMainLayout->setColumnStretch( 0, 0 );
            m_pMainLayout->setColumnStretch( 1, 0 );
            m_pMainLayout->setColumnStretch( 2, 1 );
            m_pMainLayout->setRowStretch( 0, 0 );
            m_pMainLayout->setRowStretch( 1, 0 );

            // Create header region
            //-------------------------------------------------------------------------

            m_pHeader = new QWidget();

            m_pHeaderLayout = new QHBoxLayout( m_pHeader );
            m_pHeaderLayout->setContentsMargins( context.GetIndent(), 0, 0, 0 );
            m_pHeaderLayout->setSpacing( 2 );
            m_pHeaderLayout->setAlignment( Qt::AlignLeft );

            m_pMainLayout->addWidget( m_pHeader, 0, 0 );

            // Create extra controls and editor region
            //-------------------------------------------------------------------------

            m_pExtraControlsLayout = new QHBoxLayout();
            m_pExtraControlsLayout->setContentsMargins( 0, 0, 0, 0 );
            m_pExtraControlsLayout->setSpacing( 0 );
            m_pMainLayout->addLayout( m_pExtraControlsLayout, 0, 1 );

            m_pEditorLayout = new QHBoxLayout();
            m_pEditorLayout->setContentsMargins( 0, 0, 0, 0 );
            m_pEditorLayout->setSpacing( 0 );
            m_pMainLayout->addLayout( m_pEditorLayout, 0, 2 );

            // Create child properties layout
            //-------------------------------------------------------------------------

            m_pChildContentLayout = new QVBoxLayout();
            m_pEditorLayout->setContentsMargins( 0, 0, 0, 0 );
            m_pEditorLayout->setSpacing( 0 );
            m_pMainLayout->addLayout( m_pChildContentLayout, 1, 0, 1, 3 );
        }

        void PropertyEditor::SetHeaderWidth( InitializationContext& context, int32 headerWidth )
        {
            KRG_ASSERT( m_pHeader != nullptr );
            KRG_ASSERT( headerWidth > 10 );
            m_pHeader->setFixedWidth( headerWidth );
            m_pHeader->setMaximumWidth( headerWidth );
        }

        void PropertyEditor::NotifyValueChanged()
        {
            UpdateResetButtonState();
            emit OnValueChanged();
        }

        void PropertyEditor::CreateDefaultHeader( InitializationContext& context )
        {
            KRG_ASSERT( m_pHeaderLayout != nullptr && m_pPropertyInstance != nullptr );

            auto pHeaderLabelWidget = new QWidget();
            pHeaderLabelWidget->setFixedHeight( 23 );

            auto pHeaderLabelLayout = new QHBoxLayout( pHeaderLabelWidget );
            pHeaderLabelLayout->setSpacing( 0 );
            pHeaderLabelLayout->setContentsMargins( 0, 0, 0, 0 );
            pHeaderLabelLayout->setAlignment( Qt::AlignBottom );

            pHeaderLabelLayout->addWidget( new PropertyBullet(), 0 );
            pHeaderLabelLayout->addWidget( new QLabel( m_pPropertyInstance->GetFriendlyName() ), 0 );

            //-------------------------------------------------------------------------

            m_pHeaderLayout->addWidget( pHeaderLabelWidget, 0, Qt::AlignTop );
            
            auto const headerSizeHint = pHeaderLabelWidget->sizeHint();
            context.UpdateMaxHeaderWidth( headerSizeHint.width() );
        }

        void PropertyEditor::CreateResetButton( InitializationContext& context )
        {
            KRG_ASSERT( m_pMainLayout != nullptr && m_pPropertyInstance != nullptr );

            m_pResetButton = new QPushButton();
            m_pResetButton->setFlat( true );
            m_pResetButton->setIcon( KIcon( MaterialDesign::Restart ) );
            m_pResetButton->setStyleSheet( "QPushButton{ margin: 0px 0px 0px 0px; padding: 0px; qproperty-iconSize: 14px;}" );
            m_pResetButton->setToolTip( "Reset value to default" );
            m_pResetButton->setDisabled( m_pPropertyInstance->IsDefaultValue() );

            connect( m_pResetButton, &QPushButton::clicked, this, &PropertyEditor::OnResetButtonClicked );

            //-------------------------------------------------------------------------

            m_pEditorLayout->addWidget( m_pResetButton, 0.0f );
            m_pEditorLayout->setAlignment( m_pResetButton, Qt::AlignCenter );
        }

        void PropertyEditor::UpdateResetButtonState()
        {
            if ( m_pResetButton != nullptr )
            {
                m_pResetButton->setEnabled( !m_pPropertyInstance->IsDefaultValue() );
            }
        }
    }
}