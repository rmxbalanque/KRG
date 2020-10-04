#include "PropertyGridEditors_CoreTypes.h"
#include "PropertyGridEditorFactory.h"
#include "Tools/UI/Widgets/ValueEditors/BoolValueEditor.h"
#include "Tools/UI/Widgets/ValueEditors/NumericValueEditor.h"
#include "Tools/UI/Widgets/ValueEditors/UUIDValueEditor.h"
#include "Tools/UI/Widgets/ValueEditors/StringValueEditor.h"
#include "Tools/UI/Widgets/ValueEditors/StringIDValueEditor.h"
#include "Tools/UI/Widgets/ValueEditors/ColorValueEditor.h"
#include "Tools/UI/Widgets/ValueEditors/MathValueEditors.h"
#include "Tools/UI/Widgets/ValueEditors/PathValueEditors.h"
#include "Tools/UI/Widgets/ValueEditors/EnumValueEditor.h"
#include "QBoxLayout"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace PropertyGrid
    {
        BoolPropertyEditor::BoolPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::Bool );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );

            //-------------------------------------------------------------------------

            m_pValueEditor = new ValueEditors::BoolEditor( m_pPropertyInstance->GetValue<bool>() );
            m_pEditorLayout->addWidget( m_pValueEditor, 1.0f );

            auto onValueChanged = [this] ( bool newValue, bool oldValue )
            {
                m_pPropertyInstance->SetValue<bool>( newValue );
                NotifyValueChanged();
            };

            connect( m_pValueEditor, &ValueEditors::BoolEditor::OnValueChanged, this, onValueChanged );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
        }

        void BoolPropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<bool>() );
            NotifyValueChanged();
            setFocus();
        }

        //-------------------------------------------------------------------------

        NumericPropertyEditor::NumericPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( m_pPropertyInstance != nullptr );

            //-------------------------------------------------------------------------

            auto const CoreType = TypeSystem::GetCoreType( pPropertyInstance->GetTypeID() );
            switch ( CoreType )
            {
                case TypeSystem::CoreTypes::Bool:
                case TypeSystem::CoreTypes::U8:
                case TypeSystem::CoreTypes::U16:
                case TypeSystem::CoreTypes::U32:
                case TypeSystem::CoreTypes::U64:
                case TypeSystem::CoreTypes::S8:
                case TypeSystem::CoreTypes::S16:
                case TypeSystem::CoreTypes::S32:
                case TypeSystem::CoreTypes::S64:
                case TypeSystem::CoreTypes::F32:
                case TypeSystem::CoreTypes::F64:
                case TypeSystem::CoreTypes::Radians:
                case TypeSystem::CoreTypes::Degrees:
                case TypeSystem::CoreTypes::Microseconds:
                case TypeSystem::CoreTypes::Milliseconds:
                case TypeSystem::CoreTypes::Seconds:
                case TypeSystem::CoreTypes::Percentage:
                break;

                default: // Invalid property
                {
                    KRG_HALT();
                }
            }

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );
            CreateValueEditor();
            CreateResetButton( context );
        }

        void NumericPropertyEditor::CreateValueEditor()
        {
            auto const CoreType = TypeSystem::GetCoreType( m_pPropertyInstance->GetTypeID() );
            switch ( CoreType )
            {
                case TypeSystem::CoreTypes::U8:
                {
                    m_pUnsignedValueEditor = new ValueEditors::UnsignedIntegerEditor( m_pPropertyInstance->GetValue<U8>() );
                    m_pUnsignedValueEditor->SetRange( std::numeric_limits<U8>::min(), std::numeric_limits<U8>::max() );

                    m_pEditorLayout->addWidget( m_pUnsignedValueEditor, 1.0f );

                    auto onValueChanged = [this] ( U8 newValue, U8 oldValue )
                    {
                        m_pPropertyInstance->SetValue<U8>( newValue );
                        NotifyValueChanged();
                    };

                    connect( m_pUnsignedValueEditor, &ValueEditors::UnsignedIntegerEditor::OnValueChanged, this, onValueChanged );
                }
                break;

                case TypeSystem::CoreTypes::U16:
                {
                    m_pUnsignedValueEditor = new ValueEditors::UnsignedIntegerEditor( m_pPropertyInstance->GetValue<U16>() );
                    m_pUnsignedValueEditor->SetRange( std::numeric_limits<U16>::min(), std::numeric_limits<U16>::max() );
                    m_pEditorLayout->addWidget( m_pUnsignedValueEditor, 1.0f );

                    auto onValueChanged = [this] ( U16 newValue, U16 oldValue )
                    {
                        m_pPropertyInstance->SetValue<U16>( newValue );
                        NotifyValueChanged();
                    };

                    connect( m_pUnsignedValueEditor, &ValueEditors::UnsignedIntegerEditor::OnValueChanged, this, onValueChanged );
                }
                break;

                case TypeSystem::CoreTypes::U32:
                {
                    m_pUnsignedValueEditor = new ValueEditors::UnsignedIntegerEditor( m_pPropertyInstance->GetValue<U32>() );
                    m_pUnsignedValueEditor->SetRange( std::numeric_limits<U32>::min(), std::numeric_limits<U32>::max() );
                    m_pEditorLayout->addWidget( m_pUnsignedValueEditor, 1.0f );

                    auto onValueChanged = [this] ( U32 newValue, U32 oldValue )
                    {
                        m_pPropertyInstance->SetValue<U32>( newValue );
                        NotifyValueChanged();
                    };

                    connect( m_pUnsignedValueEditor, &ValueEditors::UnsignedIntegerEditor::OnValueChanged, this, onValueChanged );
                }
                break;

                case TypeSystem::CoreTypes::U64:
                {
                    m_pUnsignedValueEditor = new ValueEditors::UnsignedIntegerEditor( m_pPropertyInstance->GetValue<U64>() );
                    m_pUnsignedValueEditor->SetRange( std::numeric_limits<U64>::min(), std::numeric_limits<U64>::max() );
                    m_pEditorLayout->addWidget( m_pUnsignedValueEditor, 1.0f );

                    auto onValueChanged = [this] ( U64 newValue, U64 oldValue )
                    {
                        m_pPropertyInstance->SetValue<U64>( newValue );
                        NotifyValueChanged();
                    };

                    connect( m_pUnsignedValueEditor, &ValueEditors::UnsignedIntegerEditor::OnValueChanged, this, onValueChanged );
                }
                break;

                //-------------------------------------------------------------------------

                case TypeSystem::CoreTypes::S8:
                {
                    m_pSignedValueEditor = new ValueEditors::SignedIntegerEditor( m_pPropertyInstance->GetValue<S8>() );
                    m_pSignedValueEditor->SetRange( std::numeric_limits<S8>::min(), std::numeric_limits<S8>::max() );
                    m_pEditorLayout->addWidget( m_pSignedValueEditor, 1.0f );

                    auto onValueChanged = [this] ( S8 newValue, S8 oldValue )
                    {
                        m_pPropertyInstance->SetValue<S8>( newValue );
                        NotifyValueChanged();
                    };

                    connect( m_pSignedValueEditor, &ValueEditors::SignedIntegerEditor::OnValueChanged, this, onValueChanged );
                }
                break;

                case TypeSystem::CoreTypes::S16:
                {
                    m_pSignedValueEditor = new ValueEditors::SignedIntegerEditor( m_pPropertyInstance->GetValue<S16>() );
                    m_pSignedValueEditor->SetRange( std::numeric_limits<S16>::min(), std::numeric_limits<S16>::max() );
                    m_pEditorLayout->addWidget( m_pSignedValueEditor, 1.0f );

                    auto onValueChanged = [this] ( S16 newValue, S16 oldValue )
                    {
                        m_pPropertyInstance->SetValue<S16>( newValue );
                        NotifyValueChanged();
                    };

                    connect( m_pSignedValueEditor, &ValueEditors::SignedIntegerEditor::OnValueChanged, this, onValueChanged );
                }
                break;

                case TypeSystem::CoreTypes::S32:
                {
                    m_pSignedValueEditor = new ValueEditors::SignedIntegerEditor( m_pPropertyInstance->GetValue<S32>() );
                    m_pSignedValueEditor->SetRange( std::numeric_limits<S32>::min(), std::numeric_limits<S32>::max() );
                    m_pEditorLayout->addWidget( m_pSignedValueEditor, 1.0f );

                    auto onValueChanged = [this] ( S32 newValue, S32 oldValue )
                    {
                        m_pPropertyInstance->SetValue<S32>( newValue );
                        NotifyValueChanged();
                    };

                    connect( m_pSignedValueEditor, &ValueEditors::SignedIntegerEditor::OnValueChanged, this, onValueChanged );
                }
                break;

                case TypeSystem::CoreTypes::S64:
                {
                    m_pSignedValueEditor = new ValueEditors::SignedIntegerEditor( m_pPropertyInstance->GetValue<S64>() );
                    m_pSignedValueEditor->SetRange( std::numeric_limits<S64>::min(), std::numeric_limits<S64>::max() );
                    m_pEditorLayout->addWidget( m_pSignedValueEditor, 1.0f );

                    auto onValueChanged = [this] ( S64 newValue, S64 oldValue )
                    {
                        m_pPropertyInstance->SetValue<S8>( newValue );
                        NotifyValueChanged();
                    };

                    connect( m_pSignedValueEditor, &ValueEditors::SignedIntegerEditor::OnValueChanged, this, onValueChanged );
                }
                break;

                //-------------------------------------------------------------------------

                case TypeSystem::CoreTypes::F32:
                case TypeSystem::CoreTypes::Radians:
                case TypeSystem::CoreTypes::Degrees:
                case TypeSystem::CoreTypes::Microseconds:
                case TypeSystem::CoreTypes::Milliseconds:
                case TypeSystem::CoreTypes::Seconds:
                case TypeSystem::CoreTypes::Percentage:
                {
                    m_pFloatingPointEditor = new ValueEditors::FloatingPointEditor( m_pPropertyInstance->GetValue<F32>() );
                    m_pFloatingPointEditor->SetRange( -FLT_MAX, FLT_MAX );
                    m_pEditorLayout->addWidget( m_pFloatingPointEditor, 1.0f );

                    auto onValueChanged = [this] ( F32 newValue, F32 oldValue )
                    {
                        m_pPropertyInstance->SetValue<F32>( newValue );
                        NotifyValueChanged();
                    };

                    connect( m_pFloatingPointEditor, &ValueEditors::FloatingPointEditor::OnValueChanged, this, onValueChanged );
                }
                break;

                case TypeSystem::CoreTypes::F64:
                {
                    m_pFloatingPointEditor = new ValueEditors::FloatingPointEditor( m_pPropertyInstance->GetValue<F64>() );
                    m_pFloatingPointEditor->SetRange( -DBL_MAX, DBL_MAX );
                    m_pEditorLayout->addWidget( m_pFloatingPointEditor, 1.0f );

                    auto onValueChanged = [this] ( F64 newValue, F64 oldValue )
                    {
                        m_pPropertyInstance->SetValue<F64>( newValue );
                        NotifyValueChanged();
                    };

                    connect( m_pFloatingPointEditor, &ValueEditors::FloatingPointEditor::OnValueChanged, this, onValueChanged );
                }
                break;
            }
        }

        void NumericPropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            
            //-------------------------------------------------------------------------

            auto const CoreType = TypeSystem::GetCoreType( m_pPropertyInstance->GetTypeID() );
            switch ( CoreType )
            {
                case TypeSystem::CoreTypes::U8:
                {
                    m_pUnsignedValueEditor->SetValue( m_pPropertyInstance->GetValue<U8>() );
                }
                break;

                case TypeSystem::CoreTypes::U16:
                {
                    m_pUnsignedValueEditor->SetValue( m_pPropertyInstance->GetValue<U16>() );
                }
                break;

                case TypeSystem::CoreTypes::U32:
                {
                    m_pUnsignedValueEditor->SetValue( m_pPropertyInstance->GetValue<U32>() );
                }
                break;

                case TypeSystem::CoreTypes::U64:
                {
                    m_pUnsignedValueEditor->SetValue( m_pPropertyInstance->GetValue<U64>() );
                }
                break;

                //-------------------------------------------------------------------------

                case TypeSystem::CoreTypes::S8:
                {
                    m_pSignedValueEditor->SetValue( m_pPropertyInstance->GetValue<S8>() );
                }
                break;

                case TypeSystem::CoreTypes::S16:
                {
                    m_pSignedValueEditor->SetValue( m_pPropertyInstance->GetValue<S16>() );
                }
                break;

                case TypeSystem::CoreTypes::S32:
                {
                    m_pSignedValueEditor->SetValue( m_pPropertyInstance->GetValue<S32>() );
                }
                break;

                case TypeSystem::CoreTypes::S64:
                {
                    m_pSignedValueEditor->SetValue( m_pPropertyInstance->GetValue<S64>() );
                }
                break;

                //-------------------------------------------------------------------------

                case TypeSystem::CoreTypes::F32:
                case TypeSystem::CoreTypes::Radians:
                case TypeSystem::CoreTypes::Degrees:
                case TypeSystem::CoreTypes::Microseconds:
                case TypeSystem::CoreTypes::Milliseconds:
                case TypeSystem::CoreTypes::Seconds:
                case TypeSystem::CoreTypes::Percentage:
                {
                    m_pFloatingPointEditor->SetValue( m_pPropertyInstance->GetValue<F32>() );
                }
                break;

                case TypeSystem::CoreTypes::F64:
                {
                    m_pFloatingPointEditor->SetValue( m_pPropertyInstance->GetValue<F64>() );
                }
                break;
            }

            NotifyValueChanged();
            setFocus();
        }

        //-------------------------------------------------------------------------

        UUIDPropertyEditor::UUIDPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::UUID );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );

            //-------------------------------------------------------------------------

            m_pValueEditor = new ValueEditors::UUIDEditor( m_pPropertyInstance->GetValue<UUID>() );
            m_pEditorLayout->addWidget( m_pValueEditor, 1.0f );

            auto onValueChanged = [this] ( UUID newValue, UUID oldValue )
            {
                m_pPropertyInstance->SetValue<UUID>( newValue );
                NotifyValueChanged();
            };

            connect( m_pValueEditor, &ValueEditors::UUIDEditor::OnValueChanged, this, onValueChanged );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
        }

        void UUIDPropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<UUID>() );
            NotifyValueChanged();
            setFocus();
        }

        //-------------------------------------------------------------------------

        ColorPropertyEditor::ColorPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::Color );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );

            //-------------------------------------------------------------------------

            m_pValueEditor = new ValueEditors::ColorEditor( m_pPropertyInstance->GetValue<Color>() );
            m_pEditorLayout->addWidget( m_pValueEditor, 1.0f );

            auto onValueChanged = [this] ( Color newValue, Color oldValue )
            {
                m_pPropertyInstance->SetValue<Color>( newValue );
                NotifyValueChanged();
            };

            connect( m_pValueEditor, &ValueEditors::ColorEditor::OnValueChanged, this, onValueChanged );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
        }

        void ColorPropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<Color>() );
            NotifyValueChanged();
            setFocus();
        }

        //-------------------------------------------------------------------------

        StringPropertyEditor::StringPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::String );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );

            //-------------------------------------------------------------------------

            m_pValueEditor = new ValueEditors::StringEditor( m_pPropertyInstance->GetValue<String>() );
            m_pEditorLayout->addWidget( m_pValueEditor, 1.0f );

            auto onValueChanged = [this] ( String newValue, String oldValue )
            {
                m_pPropertyInstance->SetValue<String>( newValue );
                NotifyValueChanged();
            };

            connect( m_pValueEditor, &ValueEditors::StringEditor::OnValueChanged, this, onValueChanged );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
        }

        void StringPropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<String>() );
            NotifyValueChanged();
            setFocus();
        }

        //-------------------------------------------------------------------------

        StringIDPropertyEditor::StringIDPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::StringID );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );
          
            //-------------------------------------------------------------------------

            m_pValueEditor = new ValueEditors::StringIDEditor( m_pPropertyInstance->GetValue<StringID>() );
            m_pEditorLayout->addWidget( m_pValueEditor, 1.0f );

            auto onValueChanged = [this] ( StringID newValue, StringID oldValue )
            {
                m_pPropertyInstance->SetValue<StringID>( newValue );
                NotifyValueChanged();
            };

            connect( m_pValueEditor, &ValueEditors::StringIDEditor::OnValueChanged, this, onValueChanged );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
        }

        void StringIDPropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<StringID>() );
            NotifyValueChanged();
            setFocus();
        }

        //-------------------------------------------------------------------------

        DataPathPropertyEditor::DataPathPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::DataPath );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );

            //-------------------------------------------------------------------------

            m_pValueEditor = new ValueEditors::DataPathEditor( FileSystemPath( "D:\\Kruger\\Data" ), m_pPropertyInstance->GetValue<DataPath>() );
            m_pEditorLayout->addWidget( m_pValueEditor, 1.0f );

            auto onValueChanged = [this] ( DataPath const& newValue, DataPath const& oldValue )
            {
                m_pPropertyInstance->SetValue<DataPath>( newValue );
                NotifyValueChanged();
            };

            connect( m_pValueEditor, &ValueEditors::DataPathEditor::OnValueChanged, this, onValueChanged );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
        }

        void DataPathPropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<DataPath>() );
            NotifyValueChanged();
            setFocus();
        }

        //-------------------------------------------------------------------------

        ResourcePathPropertyEditor::ResourcePathPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::ResourceID || pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::TResourcePtr || pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::ResourcePtr );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );

            //-------------------------------------------------------------------------

            m_pValueEditor = new ValueEditors::ResourcePathEditor( FileSystemPath( "D:\\Kruger\\Data" ), m_pPropertyInstance->GetResourceTypeIDForResourceProperty(), m_pPropertyInstance->GetValue<ResourceID>() );
            m_pEditorLayout->addWidget( m_pValueEditor, 1.0f );

            auto onValueChanged = [this] ( ResourceID newValue, ResourceID const& oldValue )
            {
                m_pPropertyInstance->SetValue<ResourceID>( newValue );
                NotifyValueChanged();
            };

            connect( m_pValueEditor, &ValueEditors::ResourcePathEditor::OnValueChanged, this, onValueChanged );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
        }

        void ResourcePathPropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<ResourceID>() );
            NotifyValueChanged();
            setFocus();
        }

        //-------------------------------------------------------------------------

        Float2PropertyEditor::Float2PropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::Float2 );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );

            //-------------------------------------------------------------------------

            m_pValueEditor = new ValueEditors::Float2Editor( m_pPropertyInstance->GetValue<Float2>() );
            m_pEditorLayout->addWidget( m_pValueEditor, 1.0f );

            auto onValueChanged = [this] ( Float2 const& newValue, Float2 const& oldValue )
            {
                m_pPropertyInstance->SetValue<Float2>( newValue );
                NotifyValueChanged();
            };

            connect( m_pValueEditor, &ValueEditors::Float2Editor::OnValueChanged, this, onValueChanged );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
        }

        void Float2PropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<Float2>() );
            NotifyValueChanged();
            setFocus();
        }

        //-------------------------------------------------------------------------

        Float3PropertyEditor::Float3PropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::Float3 );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );

            //-------------------------------------------------------------------------

            m_pValueEditor = new ValueEditors::Float3Editor( m_pPropertyInstance->GetValue<Float3>() );
            m_pEditorLayout->addWidget( m_pValueEditor, 1.0f );

            auto onValueChanged = [this] ( Float3 const& newValue, Float3 const& oldValue )
            {
                m_pPropertyInstance->SetValue<Float3>( newValue );
                NotifyValueChanged();
            };

            connect( m_pValueEditor, &ValueEditors::Float3Editor::OnValueChanged, this, onValueChanged );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
        }

        void Float3PropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<Float3>() );
            NotifyValueChanged();
            setFocus();
        }

        //-------------------------------------------------------------------------

        Float4PropertyEditor::Float4PropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::Float4 );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );

            //-------------------------------------------------------------------------

            m_pValueEditor = new ValueEditors::Float4Editor( m_pPropertyInstance->GetValue<Float4>() );
            m_pEditorLayout->addWidget( m_pValueEditor, 1.0f );

            auto onValueChanged = [this] ( Float4 const& newValue, Float4 const& oldValue )
            {
                m_pPropertyInstance->SetValue<Float4>( newValue );
                NotifyValueChanged();
            };

            connect( m_pValueEditor, &ValueEditors::Float4Editor::OnValueChanged, this, onValueChanged );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
        }

        void Float4PropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<Float4>() );
            NotifyValueChanged();
            setFocus();
        }

        //-------------------------------------------------------------------------

        RotationPropertyEditor::RotationPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::Quaternion || pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::EulerAngles );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );
            CreateValueEditor( m_pEditorLayout );
            CreateResetButton( context );
        }

        void RotationPropertyEditor::CreateValueEditor( QHBoxLayout* pLayout )
        {
            KRG_ASSERT( pLayout != nullptr && m_pPropertyInstance != nullptr );

            if ( m_pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::Quaternion )
            {
                m_pValueEditor = new ValueEditors::RotationEditor( m_pPropertyInstance->GetValue<Quaternion>() );

                //-------------------------------------------------------------------------

                auto onValueChanged = [this] ( EulerAngles const& newValue, EulerAngles const& oldValue )
                {
                    m_pPropertyInstance->SetValue<Quaternion>( Quaternion( newValue ) );
                    NotifyValueChanged();
                };

                connect( m_pValueEditor, &ValueEditors::RotationEditor::OnValueChanged, this, onValueChanged );
            }
            else
            {
                m_pValueEditor = new ValueEditors::RotationEditor( m_pPropertyInstance->GetValue<EulerAngles>() );

                //-------------------------------------------------------------------------

                auto onValueChanged = [this] ( EulerAngles const& newValue, EulerAngles const& oldValue )
                {
                    m_pPropertyInstance->SetValue<EulerAngles>( newValue );
                    NotifyValueChanged();
                };

                connect( m_pValueEditor, &ValueEditors::RotationEditor::OnValueChanged, this, onValueChanged );
            }

            pLayout->addWidget( m_pValueEditor, 1.0f );
        }

        void RotationPropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();

            if ( m_pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::Quaternion )
            {
                m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<Quaternion>() );
            }
            else
            {
                m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<EulerAngles>() );
            }

            NotifyValueChanged();
            setFocus();
        }

        //-------------------------------------------------------------------------

        TransformPropertyEditor::TransformPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::Transform || pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::Matrix );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );
            CreateValueEditor( m_pEditorLayout );
            CreateResetButton( context );
        }

        void TransformPropertyEditor::CreateValueEditor( QHBoxLayout* pLayout )
        {
            KRG_ASSERT( pLayout != nullptr && m_pPropertyInstance != nullptr );

            if ( m_pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::Transform )
            {
                m_pValueEditor = new ValueEditors::TransformEditor( m_pPropertyInstance->GetValue<Transform>() );

                //-------------------------------------------------------------------------

                auto onValueChanged = [this] ( Matrix const& newValue, Matrix const& oldValue )
                {
                    m_pPropertyInstance->SetValue<Transform>( Transform( newValue ) );
                    NotifyValueChanged();
                };

                connect( m_pValueEditor, &ValueEditors::TransformEditor::OnValueChanged, this, onValueChanged );
            }
            else
            {
                m_pValueEditor = new ValueEditors::TransformEditor( m_pPropertyInstance->GetValue<Matrix>() );

                //-------------------------------------------------------------------------

                auto onValueChanged = [this] ( Matrix const& newValue, Matrix const& oldValue )
                {
                    m_pPropertyInstance->SetValue<Matrix>( newValue );
                    NotifyValueChanged();
                };

                connect( m_pValueEditor, &ValueEditors::TransformEditor::OnValueChanged, this, onValueChanged );
            }

            pLayout->addWidget( m_pValueEditor, 1.0f );
        }

        void TransformPropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            NotifyValueChanged();

            if ( m_pPropertyInstance->GetTypeID() == TypeSystem::CoreTypes::Transform )
            {
                m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<Transform>() );
            }
            else
            {
                m_pValueEditor->SetValue( m_pPropertyInstance->GetValue<Matrix>() );
            }

            setFocus();
        }

        //-------------------------------------------------------------------------

        EnumPropertyEditor::EnumPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance )
            : PropertyEditor( context, pPropertyInstance )
        {
            KRG_ASSERT( pPropertyInstance->IsEnumProperty() );

            //-------------------------------------------------------------------------

            CreateDefaultHeader( context );

            //-------------------------------------------------------------------------

            QStringList stringValues;
            for ( auto const& enumValue : m_pPropertyInstance->GetEnumValues() )
            {
                stringValues.append( enumValue );
            }

            m_pValueEditor = new ValueEditors::EnumEditor( stringValues, m_pPropertyInstance->GetStringValue().c_str() );
            m_pEditorLayout->addWidget( m_pValueEditor, 1.0f );

            auto onValueChanged = [this] ( QStringList const& stringValues, S32 const& newValue, S32 const& oldValue )
            {
                m_pPropertyInstance->SetStringValue( stringValues[newValue].toUtf8().data() );
                NotifyValueChanged();
            };

            connect( m_pValueEditor, &ValueEditors::EnumEditor::OnValueChanged, this, onValueChanged );

            //-------------------------------------------------------------------------

            CreateResetButton( context );
        }

        void EnumPropertyEditor::OnResetButtonClicked()
        {
            m_pPropertyInstance->ResetToDefaultValue();
            m_pValueEditor->SetValue( m_pPropertyInstance->GetStringValue().c_str() );
            NotifyValueChanged();
            setFocus();
        }
    }
}