#pragma once
#include "ValueEditorBase.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

class QPushButton;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        class KRG_TOOLS_UI_API ColorEditor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( Color newValue, Color OldValue );

        public:

            ColorEditor( Color value );

            inline Color GetValue() const { return m_value; }
            inline void SetValue( Color value ) { SetValueInternal( value ); }

        private:

            void OnColorButtonClicked();
            void UpdateDisplay();
            void SetValueInternal( Color newValue );

        private:

            QPushButton*        m_pColorButton = nullptr;
            Color               m_value = Colors::Black;
        };
    }
}