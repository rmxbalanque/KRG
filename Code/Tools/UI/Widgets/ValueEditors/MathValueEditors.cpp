#include "MathValueEditors.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "QBoxLayout"
#include "QDoubleSpinBox"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        namespace
        {
            QWidget* CreateLabel( char const* pText, char const* pColor )
            {
                auto pLabel = new QLabel( pText );
                pLabel->setFixedWidth( 16 );
                pLabel->setAlignment( Qt::AlignCenter );
                pLabel->setStyleSheet( QString( "QLabel{ background-color: #464646; color: %1; margin: 1px 0px 1px 0px; font: bold 10px; border: 1px solid #262626; border-top-left-radius: 4px; border-bottom-left-radius: 4px;}").arg( pColor ) );
                return pLabel;
            }

            QDoubleSpinBox* CreateVectorInput()
            {
                auto pInput = new QDoubleSpinBox();
                pInput->setMinimumWidth( 60 );
                pInput->setStyleSheet( "QDoubleSpinBox{ margin: 0px 0px 0px 0px; }" );
                return pInput;
            }

            KIconLabel* CreateFeatureLabel( MaterialDesign codePoint, QString const& tooltipText = QString() )
            {
                auto pLabel = new KIconLabel( codePoint, 14, 14 );
                pLabel->setToolTip( tooltipText );
                pLabel->setStyleSheet( "QLabel{ background-color: #464646; padding: 0px 4px; margin: 1px 2px 1px 0px; font: bold 10px; border: 1px solid #262626; border-radius: 4px;}" );
                return pLabel;
            }

            QPushButton* CreateClampButton()
            {
                auto pClampButton = new QPushButton();
                pClampButton->setFlat( true );
                pClampButton->setIcon( KIcon( MaterialDesign::Angle_Acute ) );
                pClampButton->setToolTip( "Clamp Angle to [-360,360]" );
                pClampButton->setStyleSheet( "QPushButton{ padding: 0px 0px; margin: 0px 0px 0px 0px; }" );

                return pClampButton;
            }
        }

        //-------------------------------------------------------------------------
        // FLOAT2
        //-------------------------------------------------------------------------

        Float2Editor::Float2Editor( Float2 const& value )
            : ValueEditorBase()
            , m_value( value )
        {
            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setSpacing( 0 );
            pMainLayout->setMargin( 0 );

            //-------------------------------------------------------------------------

            {
                auto pInputLayout = new QHBoxLayout();
                pInputLayout->addWidget( CreateLabel( "X", "#FF0000" ) );

                m_inputs[0] = CreateVectorInput();
                pInputLayout->addWidget( m_inputs[0], 1.0f );
                pMainLayout->addLayout( pInputLayout );
            }

            //-------------------------------------------------------------------------

            {
                auto pInputLayout = new QHBoxLayout();
                pInputLayout->addWidget( CreateLabel( "Y", "#00FF00" ) );

                m_inputs[1] = CreateVectorInput();
                pInputLayout->addWidget( m_inputs[1], 1.0f );
                pMainLayout->addLayout( pInputLayout );
            }

            //-------------------------------------------------------------------------

            auto onSpinBoxValueChanged = [this] ( F64 )
            { 
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( Float2( m_inputs[0]->value(), m_inputs[1]->value() ) );
                }
            };

            for ( auto i = 0; i < 2; i++ )
            {
                m_inputs[i]->setSingleStep( 0.01f );
                m_inputs[i]->setDecimals( 2 );
                m_inputs[i]->setRange( -FLT_MAX, FLT_MAX );
                m_inputs[i]->setValue( m_value[i] );
                connect( m_inputs[i], QOverload<F64>::of( &QDoubleSpinBox::valueChanged ), this, onSpinBoxValueChanged );
            }
        }

        void Float2Editor::SetValueInternal( Float2 const& newValue )
        {
            Float2 oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableChangeHandlers( m_enableInputChangeHandlers, false );
                m_inputs[0]->setValue( m_value[0] );
                m_inputs[1]->setValue( m_value[1] );
            }

            emit OnValueChanged( m_value, oldValue );
        }

        //-------------------------------------------------------------------------
        // FLOAT3
        //-------------------------------------------------------------------------

        Float3Editor::Float3Editor( Float3 const& value )
            : ValueEditorBase()
            , m_value( value )
        {
            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setSpacing( 0 );
            pMainLayout->setMargin( 0 );

            //-------------------------------------------------------------------------

            {
                auto pInputLayout = new QHBoxLayout();
                pInputLayout->addWidget( CreateLabel( "X", "#FF0000" ) );

                m_inputs[0] = CreateVectorInput();
                pInputLayout->addWidget( m_inputs[0], 1.0f );
                pMainLayout->addLayout( pInputLayout );
            }

            //-------------------------------------------------------------------------

            {
                auto pInputLayout = new QHBoxLayout();
                pInputLayout->addWidget( CreateLabel( "Y", "#00FF00" ) );

                m_inputs[1] = CreateVectorInput();
                pInputLayout->addWidget( m_inputs[1], 1.0f );
                pMainLayout->addLayout( pInputLayout );
            }

            //-------------------------------------------------------------------------

            {
                auto pInputLayout = new QHBoxLayout();
                pInputLayout->addWidget( CreateLabel( "Z", "#18AAFF" ) );

                m_inputs[2] = CreateVectorInput();
                pInputLayout->addWidget( m_inputs[2], 1.0f );
                pMainLayout->addLayout( pInputLayout );
            }

            //-------------------------------------------------------------------------

            auto onSpinBoxValueChanged = [this] ( F64 )
            {
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( Float3( m_inputs[0]->value(), m_inputs[1]->value(), m_inputs[2]->value() ) );
                }
            };

            for ( auto i = 0; i < 3; i++ )
            {
                m_inputs[i]->setSingleStep( 0.01f );
                m_inputs[i]->setDecimals( 2 );
                m_inputs[i]->setRange( -FLT_MAX, FLT_MAX );
                m_inputs[i]->setValue( m_value[i] );
                connect( m_inputs[i], QOverload<F64>::of( &QDoubleSpinBox::valueChanged ), this, onSpinBoxValueChanged );
            }
        }

        void Float3Editor::SetValueInternal( Float3 const& newValue )
        {
            Float3 oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableChangeHandlers( m_enableInputChangeHandlers, false );
                m_inputs[0]->setValue( m_value[0] );
                m_inputs[1]->setValue( m_value[1] );
                m_inputs[2]->setValue( m_value[2] );
            }

            emit OnValueChanged( m_value, oldValue );
        }

        //-------------------------------------------------------------------------
        // FLOAT4
        //-------------------------------------------------------------------------

        Float4Editor::Float4Editor( Float4 const& value )
            : ValueEditorBase()
            , m_value( value )
        {
            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setSpacing( 0 );
            pMainLayout->setMargin( 0 );

            //-------------------------------------------------------------------------

            {
                auto pInputLayout = new QHBoxLayout();
                pInputLayout->addWidget( CreateLabel( "X", "#FF0000" ) );

                m_inputs[0] = CreateVectorInput();
                pInputLayout->addWidget( m_inputs[0], 1.0f );
                pMainLayout->addLayout( pInputLayout );
            }

            //-------------------------------------------------------------------------

            {
                auto pInputLayout = new QHBoxLayout();
                pInputLayout->addWidget( CreateLabel( "Y", "#00FF00" ) );

                m_inputs[1] = CreateVectorInput();
                pInputLayout->addWidget( m_inputs[1], 1.0f );
                pMainLayout->addLayout( pInputLayout );
            }

            //-------------------------------------------------------------------------

            {
                auto pInputLayout = new QHBoxLayout();
                pInputLayout->addWidget( CreateLabel( "Z", "#18AAFF" ) );

                m_inputs[2] = CreateVectorInput();
                pInputLayout->addWidget( m_inputs[2], 1.0f );
                pMainLayout->addLayout( pInputLayout );
            }

            //-------------------------------------------------------------------------

            {
                auto pInputLayout = new QHBoxLayout();
                pInputLayout->addWidget( CreateLabel( "W", "#AAAAAA" ) );

                m_inputs[3] = CreateVectorInput();
                pInputLayout->addWidget( m_inputs[3], 1.0f );
                pMainLayout->addLayout( pInputLayout );
            }

            //-------------------------------------------------------------------------

            auto onSpinBoxValueChanged = [this] ( F64 )
            {
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( Float4( m_inputs[0]->value(), m_inputs[1]->value(), m_inputs[2]->value(), m_inputs[3]->value() ) );
                }
            };

            for ( auto i = 0; i < 4; i++ )
            {
                m_inputs[i]->setSingleStep( 0.01f );
                m_inputs[i]->setDecimals( 2 );
                m_inputs[i]->setRange( -FLT_MAX, FLT_MAX );
                m_inputs[i]->setValue( m_value[i] );
                connect( m_inputs[i], QOverload<F64>::of( &QDoubleSpinBox::valueChanged ), this, onSpinBoxValueChanged );
            }
        }

        void Float4Editor::SetValueInternal( Float4 const& newValue )
        {
            Float4 oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableChangeHandlers( m_enableInputChangeHandlers, false );
                m_inputs[0]->setValue( m_value[0] );
                m_inputs[1]->setValue( m_value[1] );
                m_inputs[2]->setValue( m_value[2] );
                m_inputs[3]->setValue( m_value[3] );
            }

            emit OnValueChanged( m_value, oldValue );
        }

        //-------------------------------------------------------------------------
        // ROTATION
        //-------------------------------------------------------------------------

        RotationEditor::RotationEditor( EulerAngles const& value )
            : ValueEditorBase()
            , m_value( value.GetClamped() )
        {
            auto pMainLayout = new QVBoxLayout( this );
            pMainLayout->setSpacing( 0 );
            pMainLayout->setMargin( 0 );

            //-------------------------------------------------------------------------

            {
                auto pInputLayout = new QHBoxLayout();
                pInputLayout->addWidget( CreateLabel( "X", "#FF0000" ) );

                m_inputs[0] = CreateVectorInput();
                pInputLayout->addWidget( m_inputs[0], 1.0f );

                m_clampButtons[0] = CreateClampButton();
                pInputLayout->addWidget( m_clampButtons[0] );

                pMainLayout->addLayout( pInputLayout );
            }

            //-------------------------------------------------------------------------

            {
                auto pInputLayout = new QHBoxLayout();
                pInputLayout->addWidget( CreateLabel( "Y", "#00FF00" ) );

                m_inputs[1] = CreateVectorInput();
                pInputLayout->addWidget( m_inputs[1], 1.0f );

                m_clampButtons[1] = CreateClampButton();
                pInputLayout->addWidget( m_clampButtons[1] );

                pMainLayout->addLayout( pInputLayout );
            }

            //-------------------------------------------------------------------------

            {
                auto pInputLayout = new QHBoxLayout();
                pInputLayout->addWidget( CreateLabel( "Z", "#18AAFF" ) );

                m_inputs[2] = CreateVectorInput();
                pInputLayout->addWidget( m_inputs[2], 1.0f );

                m_clampButtons[2] = CreateClampButton();
                pInputLayout->addWidget( m_clampButtons[2] );

                pMainLayout->addLayout( pInputLayout );
            }

            //-------------------------------------------------------------------------

            auto onSpinBoxValueChanged = [this] ( F64 )
            {
                if ( m_enableInputChangeHandlers )
                {
                    SetValueInternal( EulerAngles( m_inputs[0]->value(), m_inputs[1]->value(), m_inputs[2]->value() ) );
                }
            };

            for ( auto i = 0; i < 3; i++ )
            {
                m_inputs[i]->setSingleStep( 1.0f );
                m_inputs[i]->setDecimals( 2 );
                m_inputs[i]->setRange( -FLT_MAX, FLT_MAX );
                m_inputs[i]->setValue( (F32) m_value[i].ToDegrees() );

                connect( m_inputs[i], QOverload<F64>::of( &QDoubleSpinBox::valueChanged ), this, onSpinBoxValueChanged );

                auto onClampButtonPressed = [this, i] ()
                {
                    auto clampedValue = m_value;
                    clampedValue[i].Clamp();
                    if ( clampedValue[i] != m_value[i] )
                    {
                        SetValueInternal( clampedValue );
                    }
                };

                connect( m_clampButtons[i], &QPushButton::clicked, this, onClampButtonPressed );
            }
        }

        RotationEditor::RotationEditor( Quaternion const& value )
            : RotationEditor( value.ToEulerAngles() )
        {}

        void RotationEditor::SetValueInternal( EulerAngles const& newValue )
        {
            EulerAngles oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableChangeHandlers( m_enableInputChangeHandlers, false );
                m_inputs[0]->setValue( (F32) m_value[0].ToDegrees() );
                m_inputs[1]->setValue( (F32) m_value[1].ToDegrees() );
                m_inputs[2]->setValue( (F32) m_value[2].ToDegrees() );
            }

            emit OnValueChanged( m_value, oldValue );
        }

        //-------------------------------------------------------------------------
        // TRANSFORM
        //-------------------------------------------------------------------------

        TransformEditor::TransformEditor( Matrix const& value )
            : ValueEditorBase()
            , m_value( value )
        {
            auto pMainLayout = new QGridLayout( this );
            pMainLayout->setSpacing( 1 );
            pMainLayout->setMargin( 0 );

            //-------------------------------------------------------------------------

            // Rotation
            {
                pMainLayout->addWidget( CreateFeatureLabel( MaterialDesign::Axis_Arrow, "Translation" ), 0, 0 );

                {
                    auto pInputLayout = new QHBoxLayout();
                    pInputLayout->addWidget( CreateLabel( "X", "#FF0000" ) );

                    m_inputs[0] = CreateVectorInput();
                    pInputLayout->addWidget( m_inputs[0], 1.0f );
                    pMainLayout->addLayout( pInputLayout, 0, 1 );
                }

                //-------------------------------------------------------------------------

                {
                    auto pInputLayout = new QHBoxLayout();
                    pInputLayout->addWidget( CreateLabel( "Y", "#00FF00" ) );

                    m_inputs[1] = CreateVectorInput();
                    pInputLayout->addWidget( m_inputs[1], 1.0f );
                    pMainLayout->addLayout( pInputLayout, 0, 2 );
                }

                //-------------------------------------------------------------------------

                {
                    auto pInputLayout = new QHBoxLayout();
                    pInputLayout->addWidget( CreateLabel( "Z", "#18AAFF" ) );

                    m_inputs[2] = CreateVectorInput();
                    pInputLayout->addWidget( m_inputs[2], 1.0f );
                    pMainLayout->addLayout( pInputLayout, 0, 3 );
                }
            }

            // Translation
            {
                pMainLayout->addWidget( CreateFeatureLabel( MaterialDesign::Rotate_Orbit, "Rotation" ), 1, 0 );

                {
                    auto pInputLayout = new QHBoxLayout();
                    pInputLayout->addWidget( CreateLabel( "X", "#FF0000" ) );

                    m_inputs[3] = CreateVectorInput();
                    pInputLayout->addWidget( m_inputs[3], 1.0f );
                    pMainLayout->addLayout( pInputLayout, 1, 1 );
                }

                //-------------------------------------------------------------------------

                {
                    auto pInputLayout = new QHBoxLayout();
                    pInputLayout->addWidget( CreateLabel( "Y", "#00FF00" ) );

                    m_inputs[4] = CreateVectorInput();
                    pInputLayout->addWidget( m_inputs[4], 1.0f );
                    pMainLayout->addLayout( pInputLayout, 1, 2 );
                }

                //-------------------------------------------------------------------------

                {
                    auto pInputLayout = new QHBoxLayout();
                    pInputLayout->addWidget( CreateLabel( "Z", "#18AAFF" ) );

                    m_inputs[5] = CreateVectorInput();
                    pInputLayout->addWidget( m_inputs[5], 1.0f );
                    pMainLayout->addLayout( pInputLayout, 1, 3 );
                }
            }

            // Scale
            {
                pMainLayout->addWidget( CreateFeatureLabel( MaterialDesign::Arrow_Up_Down, "Scale" ), 2, 0 );

                {
                    auto pInputLayout = new QHBoxLayout();
                    pInputLayout->addWidget( CreateLabel( "X", "#FF0000" ) );

                    m_inputs[6] = CreateVectorInput();
                    pInputLayout->addWidget( m_inputs[6], 1.0f );
                    pMainLayout->addLayout( pInputLayout, 2, 1 );
                }

                //-------------------------------------------------------------------------

                {
                    auto pInputLayout = new QHBoxLayout();
                    pInputLayout->addWidget( CreateLabel( "Y", "#00FF00" ) );

                    m_inputs[7] = CreateVectorInput();
                    pInputLayout->addWidget( m_inputs[7], 1.0f );
                    pMainLayout->addLayout( pInputLayout, 2, 2 );
                }

                //-------------------------------------------------------------------------

                {
                    auto pInputLayout = new QHBoxLayout();
                    pInputLayout->addWidget( CreateLabel( "Z", "#18AAFF" ) );

                    m_inputs[8] = CreateVectorInput();
                    pInputLayout->addWidget( m_inputs[8], 1.0f );
                    pMainLayout->addLayout( pInputLayout, 2, 3 );
                }
            }

            //-------------------------------------------------------------------------

            auto onSpinBoxValueChanged = [this] ( F64 )
            {
                if ( m_enableInputChangeHandlers )
                {
                    //SetValueInternal( Float3( m_inputs[0]->value(), m_inputs[1]->value(), m_inputs[2]->value() ) );
                }
            };

            for ( auto i = 0; i < 9; i++ )
            {
                m_inputs[i]->setSingleStep( 0.01f );
                m_inputs[i]->setDecimals( 2 );
                m_inputs[i]->setRange( -FLT_MAX, FLT_MAX );
                connect( m_inputs[i], QOverload<F64>::of( &QDoubleSpinBox::valueChanged ), this, onSpinBoxValueChanged );
            }
        }

        TransformEditor::TransformEditor( Transform const& value )
            : TransformEditor( value.ToMatrix() )
        {}

        void TransformEditor::SetValueInternal( Matrix const& newValue )
        {
            Matrix oldValue = m_value;
            m_value = newValue;

            {
                TScopedValue<bool> disableChangeHandlers( m_enableInputChangeHandlers, false );
                //m_pRotationEditor->setValue( m_value[0] );
                //m_pTranslationEditor->setValue( m_value[1] );
                //m_pScaleEditor->setValue( m_value[2] );
            }

            emit OnValueChanged( m_value, oldValue );
        }
    }
}