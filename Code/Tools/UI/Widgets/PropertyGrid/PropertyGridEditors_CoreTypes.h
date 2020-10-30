#pragma once

#include "PropertyGridEditorWidget.h"
#include "System/Resource/ResourceTypeID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        class BoolEditor;
        class SignedIntegerEditor;
        class UnsignedIntegerEditor;
        class FloatingPointEditor;
        class ColorEditor;
        class UUIDEditor;
        class StringIDEditor;
        class StringEditor;
        class DataPathEditor;
        class ResourcePathEditor;
        class Float2Editor;
        class Float3Editor;
        class Float4Editor;
        class TransformEditor;
        class RotationEditor;
        class EnumEditor;
    }

    //-------------------------------------------------------------------------

    namespace PropertyGrid
    {
        class BoolPropertyEditor final : public PropertyEditor
        {
            Q_OBJECT
    
        public:

            BoolPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;

        private:

            ValueEditors::BoolEditor*                   m_pValueEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class NumericPropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            NumericPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;
            void CreateValueEditor();

        private:

            ValueEditors::SignedIntegerEditor*          m_pSignedValueEditor = nullptr;
            ValueEditors::UnsignedIntegerEditor*        m_pUnsignedValueEditor = nullptr;
            ValueEditors::FloatingPointEditor*          m_pFloatingPointEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class UUIDPropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            UUIDPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;

        private:

            ValueEditors::UUIDEditor*                   m_pValueEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class ColorPropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            ColorPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;

        private:

            ValueEditors::ColorEditor*                  m_pValueEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class StringPropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            StringPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;

        private:

            ValueEditors::StringEditor*                 m_pValueEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class StringIDPropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            StringIDPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;

        private:

            ValueEditors::StringIDEditor*               m_pValueEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class DataPathPropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            DataPathPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;

        private:

            ValueEditors::DataPathEditor*               m_pValueEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class ResourcePathPropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            ResourcePathPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            ResourceTypeID GetResourceTypeFilter() const;
            virtual void OnResetButtonClicked() override final;

        private:

            ValueEditors::ResourcePathEditor*           m_pValueEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class Float2PropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            Float2PropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;

        private:

            ValueEditors::Float2Editor*                 m_pValueEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class Float3PropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            Float3PropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;

        private:

            ValueEditors::Float3Editor*                 m_pValueEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class Float4PropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            Float4PropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;

        private:

            ValueEditors::Float4Editor*                 m_pValueEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class RotationPropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            RotationPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;
            void CreateValueEditor( QHBoxLayout* pLayout );

        private:

            ValueEditors::RotationEditor*               m_pValueEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class TransformPropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            TransformPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;
            void CreateValueEditor( QHBoxLayout* pLayout );

        private:


            ValueEditors::TransformEditor*              m_pValueEditor = nullptr;
        };

        //-------------------------------------------------------------------------

        class EnumPropertyEditor final : public PropertyEditor
        {
            Q_OBJECT

        public:

            EnumPropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

        private:

            virtual void OnResetButtonClicked() override final;

        private:


            ValueEditors::EnumEditor*                   m_pValueEditor = nullptr;
        };
    }
}