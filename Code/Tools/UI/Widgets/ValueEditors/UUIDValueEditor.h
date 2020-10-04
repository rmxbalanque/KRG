#pragma once
#include "ValueEditorBase.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

class QLineEdit;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        class KRG_TOOLS_UI_API UUIDEditor final : public ValueEditorBase
        {
            Q_OBJECT

        signals:

            void OnValueChanged( UUID const& newValue, UUID const& oldValue );

        public:

            UUIDEditor( UUID const& value = UUID() );

            inline UUID GetValue() const { return m_value; }
            inline void SetValue( UUID const& value ) { SetValueInternal( value ); }

        private:

            void SetValueInternal( UUID newValue );
            void UpdateDisplay();

        private:

            QLineEdit*          m_pIDBox = nullptr;
            UUID                m_value;
        };
    }
}