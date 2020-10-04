#pragma once
#include "ValueEditorBase.h"
#include "System/Core/Types/StringID.h"

//-------------------------------------------------------------------------

class QLineEdit;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        class KRG_TOOLS_UI_API StringIDEditor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( StringID newValue, StringID OldValue );

        public:

            StringIDEditor( StringID value );

            inline StringID GetValue() const { return m_value; }
            inline void SetValue( StringID value ) { SetValueInternal( value ); }

        private:

            void SetValueInternal( StringID value );
            void UpdateDisplay();

        private:

            QLineEdit*          m_pIDBox = nullptr;
            QLineEdit*          m_pTextBox = nullptr;
            StringID            m_value;
        };
    }
}