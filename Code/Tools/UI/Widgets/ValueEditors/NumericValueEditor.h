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

            void OnValueChanged( int64 newValue, int64 OldValue );

        public:

            SignedIntegerEditor( int64 value );

            void SetRange( int64 min, int64 max );
            void SetWrapping( bool isWrappingEnabled );

            inline int64 GetValue() const { return m_value; }
            inline void SetValue( int64 value ) { SetValueInternal( value ); }

        private:

            void SetValueInternal( int64 newValue );

        private:

            KLongLongSpinBox*   m_pSpinBox = nullptr;
            int64                 m_value = false;
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_UI_API UnsignedIntegerEditor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( uint64 newValue, uint64 OldValue );

        public:

            UnsignedIntegerEditor( uint64 value );

            void SetRange( uint64 min, uint64 max );
            void SetWrapping( bool isWrappingEnabled );

            inline uint64 GetValue() const { return m_value; }
            inline void SetValue( uint64 value ) { SetValueInternal( value ); }

        private:

            void SetValueInternal( uint64 newValue );

        private:

            KULongLongSpinBox*  m_pSpinBox = nullptr;
            uint64                 m_value = false;
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_UI_API FloatingPointEditor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( double newValue, double OldValue );

        public:

            FloatingPointEditor( double value );

            void SetRange( double min, double max );
            void SetWrapping( bool isWrappingEnabled );

            inline double GetValue() const { return m_value; }
            inline void SetValue( double value ) { SetValueInternal( value ); }

        private:

            void SetValueInternal( double newValue );

        private:

            QDoubleSpinBox*     m_pSpinBox = nullptr;
            double                 m_value = false;
        };
    }
}