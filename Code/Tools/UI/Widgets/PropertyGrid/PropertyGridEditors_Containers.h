#pragma once

#include "PropertyGridEditorWidget.h"
#include "Tools/UI/Widgets/CollapsibleWidget.h"

//-------------------------------------------------------------------------

class QGridLayout;
class QLabel;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace PropertyGrid
    {
        class CollapsiblePropertyEditorHeader : public QWidget
        {
            Q_OBJECT

        signals:

            void Clicked();

        public:

            CollapsiblePropertyEditorHeader( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance );

            bool IsCollapsed() const { return m_pArrow->IsCollapsed(); }

        private:

            virtual void mousePressEvent( QMouseEvent *event ) override;

        private:

            KCollapsibleArrowWidget*                        m_pArrow = nullptr;
        };

        //-------------------------------------------------------------------------

        class ContainerPropertyEditor : public PropertyEditor
        {
            Q_OBJECT

        public:

            ContainerPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance );

        protected:

            virtual void SetHeaderWidth( InitializationContext& context, S32 headerWidth ) override final;
            virtual void OnResetButtonClicked() override final;
            virtual void UpdateCustomDisplay() {}
            virtual void RecreateChildPropertyEditors() = 0;

        protected:

            CollapsiblePropertyEditorHeader*                m_pCollapsibleHeader = nullptr;
            QWidget*                                        m_pContentFrame = nullptr;
            QVBoxLayout*                                    m_pContentLayout = nullptr;
            TVector<PropertyGrid::PropertyEditor*>          m_propertyEditors;
            InitializationContext                           m_cachedContext;
            S32                                             m_cachedHeaderWidth = 0;
        };

        //-------------------------------------------------------------------------

        class StructurePropertyEditor final : public ContainerPropertyEditor
        {
            Q_OBJECT

        public:

            StructurePropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance );

        private:

            void CreateChildPropertyEditors();
            virtual void RecreateChildPropertyEditors() { CreateChildPropertyEditors(); }
        };

        //-------------------------------------------------------------------------

        class StaticArrayPropertyEditor final : public ContainerPropertyEditor
        {
            Q_OBJECT

        public:

            StaticArrayPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance );

        private:

            void CreateChildPropertyEditors();
            virtual void RecreateChildPropertyEditors() override { CreateChildPropertyEditors(); }
        };

        //-------------------------------------------------------------------------

        class DynamicArrayPropertyEditor final : public ContainerPropertyEditor
        {
            Q_OBJECT

        public:

            DynamicArrayPropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance );

        private:

            void CreateChildPropertyEditors();
            virtual void RecreateChildPropertyEditors() override { CreateChildPropertyEditors(); }
            virtual void UpdateCustomDisplay() override;
            
            void OnAddNewArrayElement();
            void OnRemoveArrayElement( S32 arrayElementIdx );

        private:

            QLabel*                                         m_pInfoLabel = nullptr;
        };
    }
}