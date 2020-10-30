#include "PropertyGridEditors_Containers.h"
#include "PropertyGridEditorFactory.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "Tools/Core/TypeSystem/ToolTypeInstance.h"
#include "QBoxLayout"
#include "QPushButton"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace PropertyGrid
    {
        CollapsiblePropertyEditorHeader::CollapsiblePropertyEditorHeader( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance )
            : QWidget()
        {
            KRG_ASSERT( pPropertyInstance != nullptr );

            m_pArrow = new KCollapsibleArrowWidget();

            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setContentsMargins( 0, 0, 0, 0 );
            pMainLayout->setSpacing( 2 );
            pMainLayout->setAlignment( Qt::AlignTop );

            pMainLayout->addWidget( m_pArrow, 0.0f );
            pMainLayout->addWidget( new QLabel( pPropertyInstance->GetFriendlyName() ), 0.0f );
        }

        void CollapsiblePropertyEditorHeader::mousePressEvent( QMouseEvent *event )
        {
            m_pArrow->SetArrowState( !m_pArrow->IsCollapsed() );
            emit Clicked();
        }

        //-------------------------------------------------------------------------

        ContainerPropertyEditor::ContainerPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance ) 
            , m_cachedContext( context )
        {
            m_pCollapsibleHeader = new CollapsiblePropertyEditorHeader( context, pPropertyInstance );
            m_pHeaderLayout->addWidget( m_pCollapsibleHeader, 1 );
            context.UpdateMaxHeaderWidth( m_pCollapsibleHeader->sizeHint().width() );

            //-------------------------------------------------------------------------

            m_pContentFrame = new QWidget();

            m_pContentLayout = new QVBoxLayout( m_pContentFrame );
            m_pContentLayout->setContentsMargins( 0, 4, 0, 4 );
            m_pContentLayout->setSpacing( 0 );

            m_pChildContentLayout->addWidget( m_pContentFrame, 0 );

            m_pContentFrame->setVisible( !m_pCollapsibleHeader->IsCollapsed() );

            //-------------------------------------------------------------------------

            auto onHeaderClicked = [this] ()
            {
                m_pContentFrame->setVisible( !m_pCollapsibleHeader->IsCollapsed() );
            };

            connect( m_pCollapsibleHeader, &CollapsiblePropertyEditorHeader::Clicked, this, onHeaderClicked );
        }

        void ContainerPropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            RecreateChildPropertyEditors();
            SetHeaderWidth( m_cachedContext, m_cachedHeaderWidth );
            UpdateCustomDisplay();
            NotifyValueChanged();
            setFocus();
        }

        void ContainerPropertyEditor::SetHeaderWidth( InitializationContext& context, S32 headerWidth )
        {
            m_cachedHeaderWidth = headerWidth;

            //-------------------------------------------------------------------------

            PropertyEditor::SetHeaderWidth( m_cachedContext, headerWidth );

            for ( auto pPropertyEditor : m_propertyEditors )
            {
                pPropertyEditor->SetHeaderWidth( context, context.GetMaxHeaderWidth() );
            }
        }

        //-------------------------------------------------------------------------

        StructurePropertyEditor::StructurePropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance )
            : ContainerPropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( !pPropertyInstance->IsCoreType() && !pPropertyInstance->IsArray() );

            //-------------------------------------------------------------------------

            QString structureName = m_pPropertyInstance->GetTypeID().GetAsStringID().ToString();
            structureName = structureName.right( structureName.length() - 5 );
            auto pInfoLabel = new QLabel( structureName );

            m_pEditorLayout->addWidget( pInfoLabel, 1 );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
            CreateChildPropertyEditors();
        }

        void StructurePropertyEditor::CreateChildPropertyEditors()
        {
            setUpdatesEnabled( false );

            for ( auto pPropertyEditor : m_propertyEditors )
            {
                m_pContentLayout->removeWidget( pPropertyEditor );
                delete pPropertyEditor;
            }

            m_propertyEditors.clear();

            //-------------------------------------------------------------------------

            m_cachedContext.IncreaseIndentLevel();

            for ( auto const& propertyInstance : m_pPropertyInstance->GetProperties() )
            {
                auto pPropertyEditor = PropertyGrid::CreateEditor( m_cachedContext, const_cast<TypeSystem::ToolPropertyInstance*>( &propertyInstance ) );
                if ( pPropertyEditor != nullptr )
                {
                    auto onChildValueChanged = [this, pPropertyEditor] ()
                    {
                        NotifyValueChanged();
                    };

                    connect( pPropertyEditor, &PropertyEditor::OnValueChanged, this, onChildValueChanged );

                    //-------------------------------------------------------------------------

                    m_propertyEditors.emplace_back( pPropertyEditor );
                    m_pContentLayout->addWidget( pPropertyEditor );
                }
            }

            m_cachedContext.DecreaseIndentLevel();

            setUpdatesEnabled( true );
        }

        //-------------------------------------------------------------------------

        StaticArrayPropertyEditor::StaticArrayPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance )
            : ContainerPropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->IsArray() );

            //-------------------------------------------------------------------------

            m_pEditorLayout->addWidget( new QLabel( QString("%1 Elements").arg( m_pPropertyInstance->GetNumArrayElements() ) ), 1 );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
            CreateChildPropertyEditors();
        }

        void StaticArrayPropertyEditor::CreateChildPropertyEditors()
        {
            setUpdatesEnabled( false );

            for ( auto pPropertyEditor : m_propertyEditors )
            {
                m_pContentLayout->removeWidget( pPropertyEditor );
                delete pPropertyEditor;
            }

            m_propertyEditors.clear();

            //-------------------------------------------------------------------------

            m_cachedContext.IncreaseIndentLevel();

            for ( auto const& propertyInstance : m_pPropertyInstance->GetProperties() )
            {
                auto pPropertyEditor = PropertyGrid::CreateEditor( m_cachedContext, const_cast<TypeSystem::ToolPropertyInstance*>( &propertyInstance ) );
                if ( pPropertyEditor != nullptr )
                {
                    auto onChildValueChanged = [this, pPropertyEditor] ()
                    {
                        NotifyValueChanged();
                    };

                    connect( pPropertyEditor, &PropertyEditor::OnValueChanged, this, onChildValueChanged );

                    //-------------------------------------------------------------------------

                    m_propertyEditors.emplace_back( pPropertyEditor );
                    m_pContentLayout->addWidget( pPropertyEditor );
                }
            }

            m_cachedContext.DecreaseIndentLevel();

            setUpdatesEnabled( true );
        }

        //-------------------------------------------------------------------------

        DynamicArrayPropertyEditor::DynamicArrayPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance )
            : ContainerPropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->IsArray() );

            //-------------------------------------------------------------------------

            m_pInfoLabel = new QLabel();

            auto pAddNewElementButton = new QPushButton();
            pAddNewElementButton->setFlat( true );
            pAddNewElementButton->setIcon( KIcon( MaterialDesign::Plus, Qt::green ) );
            pAddNewElementButton->setToolTip( "Add Row" );
            pAddNewElementButton->setStyleSheet( "QPushButton{ margin: 0px 0px 0px 0px; padding: -2px; qproperty-iconSize: 16px;}" );

            connect( pAddNewElementButton, &QPushButton::clicked, this, &DynamicArrayPropertyEditor::OnAddNewArrayElement );

            m_pEditorLayout->addWidget( m_pInfoLabel, 1.0f );
            m_pEditorLayout->addWidget( pAddNewElementButton, 0.0f, Qt::AlignCenter );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
            CreateChildPropertyEditors();
            UpdateCustomDisplay();
        }

        void DynamicArrayPropertyEditor::UpdateCustomDisplay()
        {
            m_pInfoLabel->setText( QString( "%1 Elements" ).arg( m_pPropertyInstance->GetNumArrayElements() ) );
        }

        void DynamicArrayPropertyEditor::CreateChildPropertyEditors()
        {
            setUpdatesEnabled( false );

            for ( auto pPropertyEditor : m_propertyEditors )
            {
                m_pContentLayout->removeWidget( pPropertyEditor );
                delete pPropertyEditor;
            }

            m_propertyEditors.clear();

            //-------------------------------------------------------------------------

            m_cachedContext.IncreaseIndentLevel();

            S32 const numArrayElements = m_pPropertyInstance->GetNumArrayElements();
            for ( S32 i = 0; i < numArrayElements; i++ )
            {
                auto pPropertyEditor = PropertyGrid::CreateEditor( m_cachedContext, const_cast<TypeSystem::ToolPropertyInstance*>( &m_pPropertyInstance->GetArrayElement( i ) ) );
                if ( pPropertyEditor != nullptr )
                {
                    auto pRemoveRowButton = new QPushButton();
                    pRemoveRowButton->setFlat( true );
                    pRemoveRowButton->setIcon( KIcon( MaterialDesign::Minus, Qt::red ) );
                    pRemoveRowButton->setToolTip( "Remove Row" );
                    pPropertyEditor->GetExtraControlsLayout()->addWidget( pRemoveRowButton, 0.0f );

                    connect( pRemoveRowButton, &QPushButton::clicked, this, [this, i] () { OnRemoveArrayElement( i ); } );

                    //-------------------------------------------------------------------------

                    auto onChildValueChanged = [this, pPropertyEditor] ()
                    {
                        NotifyValueChanged();
                    };

                    connect( pPropertyEditor, &PropertyEditor::OnValueChanged, this, onChildValueChanged );

                    //-------------------------------------------------------------------------

                    m_propertyEditors.emplace_back( pPropertyEditor );
                    m_pContentLayout->addWidget( pPropertyEditor );
                }
            }

            m_cachedContext.DecreaseIndentLevel();

            setUpdatesEnabled( true );
        }

        void DynamicArrayPropertyEditor::OnAddNewArrayElement()
        {
            m_pPropertyInstance->AddArrayElement();
            RecreateChildPropertyEditors();
            SetHeaderWidth( m_cachedContext, m_cachedHeaderWidth );
            UpdateCustomDisplay();
            NotifyValueChanged();
        }

        void DynamicArrayPropertyEditor::OnRemoveArrayElement( S32 arrayElementIdx )
        {
            m_pPropertyInstance->RemoveArrayElement( arrayElementIdx );
            RecreateChildPropertyEditors();
            SetHeaderWidth( m_cachedContext, m_cachedHeaderWidth );
            UpdateCustomDisplay();
            NotifyValueChanged();
        }
    }
}