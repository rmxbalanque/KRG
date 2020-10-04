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

            void OnValueChanged( QStringList const& stringValues, S64 newValue, S64 OldValue );

        public:

            EnumEditor( QStringList const& stringValues, S64 value );
            EnumEditor( QStringList const& stringValues, QString const& value );

            inline S64 GetValue() const { return m_value; }
            inline QString const& GetStringValue() const { KRG_ASSERT( IsValidEnumValue( m_value ) ); return m_enumStringValues[m_value]; }
            inline QStringList GetEnumValues() const { return m_enumStringValues; }

            inline void SetValue( S64 value ) { SetValueInternal( value ); }
            inline void SetValue( QString const& value );

        private:

            void SetValueInternal( S64 newValue );

            inline S64 GetIndexForStringValue( QString const& enumValue ) const { return m_enumStringValues.indexOf( enumValue ); }
            inline bool IsValidEnumValue( S64 value ) const { return value >= 0 && value < (S64) m_enumStringValues.size(); }

        private:

            QComboBox*          m_pInput = nullptr;
            QStringList         m_enumStringValues;
            S64                 m_value = InvalidIndex;
            S64                 m_defaultValue = InvalidIndex;
        };
    }
}