#pragma once
#include "ValueEditorBase.h"

//-------------------------------------------------------------------------

class QComboBox;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        class KRG_TOOLS_UI_API EnumEditor final : public ValueEditorBase
        {
            Q_OBJECT

         signals:

            void OnValueChanged( QStringList const& stringValues, int64 newValue, int64 OldValue );

        public:

            EnumEditor( QStringList const& stringValues, int64 value );
            EnumEditor( QStringList const& stringValues, QString const& value );

            inline int64 GetValue() const { return m_value; }
            inline QString const& GetStringValue() const { KRG_ASSERT( IsValidEnumValue( m_value ) ); return m_enumStringValues[m_value]; }
            inline QStringList GetEnumValues() const { return m_enumStringValues; }

            inline void SetValue( int64 value ) { SetValueInternal( value ); }
            inline void SetValue( QString const& value );

        private:

            void SetValueInternal( int64 newValue );

            inline int64 GetIndexForStringValue( QString const& enumValue ) const { return m_enumStringValues.indexOf( enumValue ); }
            inline bool IsValidEnumValue( int64 value ) const { return value >= 0 && value < (int64) m_enumStringValues.size(); }

        private:

            QComboBox*          m_pInput = nullptr;
            QStringList         m_enumStringValues;
            int64                 m_value = InvalidIndex;
            int64                 m_defaultValue = InvalidIndex;
        };
    }
}