#pragma once
#include "ValueEditorBase.h"
#include "System/Core/Math/Math.h"
#include "System/Core/Math/Matrix.h"
#include "System/Core/Math/Transform.h"

//-------------------------------------------------------------------------

class QDoubleSpinBox;
class QBoxLayout;
class QPushButton;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        class KRG_TOOLS_UI_API Float2Editor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( Float2 const& newValue, Float2 const& oldValue );

        public:

            Float2Editor( Float2 const& value );

            Float2 const& GetValue() const { return m_value; }
            void SetValue( Float2 const& value ) { SetValueInternal( value ); }

        private:

            void SetValueInternal( Float2 const& value );

        private:

            QDoubleSpinBox*       m_inputs[2] = {nullptr, nullptr };
            Float2                m_value;
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_UI_API Float3Editor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( Float3 const& newValue, Float3 const& oldValue );

        public:

            Float3Editor( Float3 const& value );

            Float3 const& GetValue() const { return m_value; }
            void SetValue( Float3 const& value ) { SetValueInternal( value ); }

        private:

            void SetValueInternal( Float3 const& value );

        private:

            QDoubleSpinBox*       m_inputs[3] = {nullptr, nullptr };
            Float3                m_value;
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_UI_API Float4Editor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( Float4 const& newValue, Float4 const& oldValue );

        public:

            Float4Editor( Float4 const& value );

            Float4 const& GetValue() const { return m_value; }
            void SetValue( Float4 const& value ) { SetValueInternal( value ); }

        private:

            void SetValueInternal( Float4 const& value );

        private:

            QDoubleSpinBox*       m_inputs[4] = {nullptr, nullptr };
            Float4                m_value;
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_UI_API RotationEditor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( EulerAngles const& newValue, EulerAngles const& oldValue );

        public:

            RotationEditor( EulerAngles const& value );
            RotationEditor( Quaternion const& value );

            EulerAngles const& GetValue() const { return m_value; }
            void SetValue( EulerAngles const& value ) { SetValueInternal( value ); }
            void SetValue( Quaternion const& value ) { SetValueInternal( value.ToEulerAngles() ); }

        private:

            void SetValueInternal( EulerAngles const& value );

        private:

            QDoubleSpinBox*         m_inputs[3] = {nullptr, nullptr };
            QPushButton*            m_clampButtons[3] = { nullptr, nullptr };
            EulerAngles             m_value;
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_UI_API TransformEditor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( Matrix const& newValue, Matrix const& oldValue );

        public:

            TransformEditor( Matrix const& value );
            TransformEditor( Transform const& value );

            Matrix const& GetValue() const { return m_value; }
            void SetValue( Matrix const& value ) { SetValueInternal( value ); }
            void SetValue( Transform const& value ) { SetValueInternal( value.ToMatrix() ); }

        private:

            void SetValueInternal( Matrix const& value );

        private:

            QDoubleSpinBox*  m_inputs[9];
            Matrix           m_value;
        };
    }
}