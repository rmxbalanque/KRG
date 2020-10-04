#pragma once
#include "ValueEditorBase.h"

//-------------------------------------------------------------------------

class QCheckBox;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        class KRG_TOOLS_UI_API BoolEditor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( bool newValue, bool OldValue );

        public:

            BoolEditor( bool value );

            inline bool GetValue() const { return m_value; }
            inline void SetValue( bool value ) { SetValueInternal( value ); }
        private:

            void SetValueInternal( bool newValue );

        private:

            QCheckBox*  m_pCheckbox = nullptr;
            bool        m_value = false;
        };
    }
}