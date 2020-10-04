#pragma once
#include "ValueEditorBase.h"


//-------------------------------------------------------------------------

class QSpinBox;
class QDoubleSpinBox;
class QSlider;

//-------------------------------------------------------------------------

namespace KRG
{
    class KLongLongSpinBox;
    class KULongLongSpinBox;

    //-------------------------------------------------------------------------

    namespace ValueEditors
    {
        class KRG_TOOLS_UI_API SignedIntegerEditor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( S64 newValue, S64 OldValue );

        public:

            SignedIntegerEditor( S64 value );

            void SetRange( S64 min, S64 max );
            void SetWrapping( bool isWrappingEnabled );

            inline S64 GetValue() const { return m_value; }
            inline void SetValue( S64 value ) { SetValueInternal( value ); }

        private:

            void SetValueInternal( S64 newValue );

        private:

            KLongLongSpinBox*   m_pSpinBox = nullptr;
            S64                 m_value = false;
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_UI_API UnsignedIntegerEditor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( U64 newValue, U64 OldValue );

        public:

            UnsignedIntegerEditor( U64 value );

            void SetRange( U64 min, U64 max );
            void SetWrapping( bool isWrappingEnabled );

            inline U64 GetValue() const { return m_value; }
            inline void SetValue( U64 value ) { SetValueInternal( value ); }

        private:

            void SetValueInternal( U64 newValue );

        private:

            KULongLongSpinBox*  m_pSpinBox = nullptr;
            U64                 m_value = false;
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_UI_API FloatingPointEditor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( F64 newValue, F64 OldValue );

        public:

            FloatingPointEditor( F64 value );

            void SetRange( F64 min, F64 max );
            void SetWrapping( bool isWrappingEnabled );

            inline F64 GetValue() const { return m_value; }
            inline void SetValue( F64 value ) { SetValueInternal( value ); }

        private:

            void SetValueInternal( F64 newValue );

        private:

            QDoubleSpinBox*     m_pSpinBox = nullptr;
            F64                 m_value = false;
        };
    }
}