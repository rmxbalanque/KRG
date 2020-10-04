#include "PropertyGridWidget.h"
#include "PropertyGridEditorFactory.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "Tools/UI/Widgets/SearchBoxWidget.h"
#include "QScrollArea"
#include "QBoxLayout"

//-------------------------------------------------------------------------

namespace KRG
{
    PropertyGridWidget::PropertyGridWidget( QWidget* pParent )
        : QWidget( pParent )
    {
        auto pFilter = new KSearchBoxWidget();
        pFilter->SetPlaceholderText( "Filter Properties..." );

        //-------------------------------------------------------------------------

        m_pDefaultMessage = new QLabel( "Nothing Selected" );

        //-------------------------------------------------------------------------

        auto pContentWidget = new QWidget();
        m_pContentLayout = new QVBoxLayout( pContentWidget );
        m_pContentLayout->setAlignment( Qt::AlignTop );
        m_pContentLayout->setContentsMargins( 0, 0, 0, 0 );
        m_pContentLayout->setSpacing( 0 );

        m_pScrollArea = new QScrollArea();
        m_pScrollArea->setWidgetResizable( true );
        m_pScrollArea->setWidget( pContentWidget );
        m_pScrollArea->setVisible( false );

        //-------------------------------------------------------------------------

        auto pMainLayout = new QVBoxLayout( this );
        pMainLayout->setSpacing( 4 );
        pMainLayout->setMargin( 0 );
        pMainLayout->addWidget( pFilter, 0 );
        pMainLayout->addWidget( m_pDefaultMessage, 1, Qt::AlignCenter );
        pMainLayout->addWidget( m_pScrollArea, 1 );
    }

    void PropertyGridWidget::SetTypeInstanceToEdit( DynamicTypeInstance* pTypeInstance )
    {
        m_pTypeInstance = pTypeInstance;
        GenerateGridWidgets();
    }

    void PropertyGridWidget::GenerateGridWidgets()
    {
        setUpdatesEnabled( false );

        //-------------------------------------------------------------------------

        for ( auto pPropertyEditor : m_propertyEditors )
        {
            m_pContentLayout->removeWidget( pPropertyEditor );
            delete pPropertyEditor;
        }

        m_propertyEditors.clear();

        //-------------------------------------------------------------------------

        if ( m_pTypeInstance == nullptr )
        {
            m_pDefaultMessage->setVisible( true );
            m_pScrollArea->setVisible( false );
            return;
        }

        m_pDefaultMessage->setVisible( false );
        m_pScrollArea->setVisible( true );

        //-------------------------------------------------------------------------

        PropertyGrid::InitializationContext initContext( 200 );

        for ( auto const& propertyInstance : m_pTypeInstance->GetProperties() )
        {
            auto pPropertyEditor = PropertyGrid::CreateEditor( initContext, const_cast<DynamicPropertyInstance*>( &propertyInstance ) );
            if ( pPropertyEditor != nullptr )
            {
                m_propertyEditors.emplace_back( pPropertyEditor );
                m_pContentLayout->addWidget( pPropertyEditor );
            }
            else
            {
                if ( propertyInstance.IsArrayProperty() )
                {
                    m_pContentLayout->addWidget( new QLabel( QString( "Cannot create property editor for array" ) ) );
                }
                else
                {
                    m_pContentLayout->addWidget( new QLabel( QString( "Cannot create property editor for type: %1" ).arg( propertyInstance.GetTypeID().GetAsStringID().c_str() ) ) );
                }
            }
        }

        //m_pContentLayout->addStretch( 1.0f );

        //-------------------------------------------------------------------------

        for ( auto pPropertyEditor : m_propertyEditors )
        {
            pPropertyEditor->SetHeaderWidth( initContext, initContext.GetMaxHeaderWidth() );
        }

        //-------------------------------------------------------------------------

        setUpdatesEnabled( true );
    }
}