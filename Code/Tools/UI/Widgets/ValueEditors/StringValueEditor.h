#pragma once
#include "ValueEditorBase.h"
#include "System/Core/Types/String.h"

//-------------------------------------------------------------------------

class QLineEdit;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        class KRG_TOOLS_UI_API StringEditor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( String const& newValue, String const& oldValue );

        public:

            StringEditor( String const& value );

            String const& GetValue() const { return m_value; }
            void SetValue( String const& value ) { SetValueInternal( value ); }

        private:

            void OnTextEdited( QString const& text );

            void SetValueInternal( String const& value );

        private:

            QLineEdit*      m_pInputBox = nullptr;
            String          m_value;
        };
    }
}