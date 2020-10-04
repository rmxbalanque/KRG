#pragma once
#include "ValueEditorBase.h"
#include "System/Resource/ResourceID.h"

//-------------------------------------------------------------------------

class QLabel;
class QLineEdit;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        class KRG_TOOLS_UI_API DataPathEditor final : public ValueEditorBase
        {
            Q_OBJECT

       signals:

            void OnValueChanged( DataPath const& newValue, DataPath const& OldValue );

        public:

            DataPathEditor( FileSystemPath const& dataDirectoryPath, DataPath value );

            inline DataPath GetValue() const { return m_value; }
            inline void SetValue( DataPath value ) { SetValueInternal( value ); }

        private:

            void OnBrowseButtonClicked();
            void UpdateDisplay();
            void SetValueInternal( DataPath newValue );

        private:

            QLineEdit*              m_pPathDisplay = nullptr;
            FileSystemPath const    m_dataDirectoryPath;
            DataPath                m_value;
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_UI_API ResourcePathEditor final : public ValueEditorBase
        {
            Q_OBJECT

       signals:

            void OnValueChanged( ResourceID const& newValue, ResourceID const&OldValue );

        public:

            ResourcePathEditor( FileSystemPath const& dataDirectoryPath, ResourceTypeID resourceTypeFilter, ResourceID value );

            inline ResourceID GetValue() const { return m_value; }
            inline void SetValue( ResourceID value ) { SetValueInternal( value ); }

        private:

            void OnBrowseButtonClicked();
            void UpdateDisplay();
            void SetValueInternal( ResourceID newValue );

        private:

            QLabel*                 m_pTypeDisplay = nullptr;
            QLineEdit*              m_pPathDisplay = nullptr;
            FileSystemPath const    m_dataDirectoryPath;
            ResourceTypeID          m_resourceTypeFilter;
            ResourceID              m_value;
            QString                 m_fileDialogFilterString;
        };
    }
}