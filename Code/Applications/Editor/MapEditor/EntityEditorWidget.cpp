#include "EntityEditorWidget.h"
#include "EntityComponentEditorWidget.h"
#include "Tools/Entity/ToolEntityComponent.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "Tools/UI/Widgets/PropertyGrid/PropertyGridWidget.h"
#include "QSplitter"
#include "QBoxLayout"
#include "EditorContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EntityEditorWidget::EntityEditorWidget( TypeSystem::TypeRegistry const& typeRegistry )
        : QWidget()
    {
        setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );

        //-------------------------------------------------------------------------

        m_pComponentEditor = new EntityComponentEditorWidget();
        connect( m_pComponentEditor, &EntityComponentEditorWidget::OnComponentSelected, this, &EntityEditorWidget::OnComponentSelected );

        //-------------------------------------------------------------------------

        m_pPropertyGrid = new PropertyGridWidget();

        //-------------------------------------------------------------------------

        auto pAddButton = new QPushButton();
        pAddButton->setIcon( KIcon( ForkAwesome::Plus ) );
        pAddButton->setText( "Add Component" );
        pAddButton->setStyleSheet( "color: white; background-color: green" );

        auto OnAddButtonClicked = [this] ()
        {
            
        };

        connect( pAddButton, &QPushButton::clicked, this, OnAddButtonClicked );

        // Layout
        //-------------------------------------------------------------------------

        auto pSplitter = new QSplitter( Qt::Orientation::Vertical );
        pSplitter->addWidget( m_pComponentEditor );
        pSplitter->addWidget( m_pPropertyGrid );

        auto pMainLayout = new QVBoxLayout( this );
        pMainLayout->setSpacing( 2 );
        pMainLayout->setMargin( 0 );
        pMainLayout->addWidget( pAddButton, 0 );
        pMainLayout->addWidget( pSplitter, 0 );
    }

    void EntityEditorWidget::SetEntityToEdit( EntityModel::ToolEntity* pEntity )
    {
        m_pComponentEditor->SetEntityToEdit( pEntity );
        m_pPropertyGrid->SetTypeInstanceToEdit( nullptr );
    }

    void EntityEditorWidget::AddComponent( TypeSystem::TypeInfo const* pComponentTypeInfo )
    {
        // TODO
    }

    void EntityEditorWidget::OnComponentSelected( EntityModel::ToolEntityComponent* pComponent )
    {
        m_pPropertyGrid->SetTypeInstanceToEdit( &pComponent->GetTypeInstance() );
    }
}