#pragma once

#include "System/Resource/ResourceTypeID.h"
#include "System/Core/Math/Math.h"

#include "QWidget"
#include "QFrame"

//-------------------------------------------------------------------------

class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QGridLayout;
class QPaintEvent;

//-------------------------------------------------------------------------

namespace KRG
{
    class DynamicPropertyInstance;

    //-------------------------------------------------------------------------

    namespace PropertyGrid
    {
        struct InitializationContext
        {
            InitializationContext( S32 desiredMinimumHeaderWidth = 0 ) 
                : m_maxHeaderWidth( desiredMinimumHeaderWidth )
            {
                KRG_ASSERT( desiredMinimumHeaderWidth >= 0 );
            }

            //-------------------------------------------------------------------------

            inline S32 GetIndent() const { return m_indentLevel * m_indentPerLevel; }

            inline S32 GetIndentLevel() const { return m_indentLevel; }

            inline void IncreaseIndentLevel() 
            { 
                m_indentLevel++;
            }

            inline void DecreaseIndentLevel()
            { 
                KRG_ASSERT( m_indentLevel > 0 );
                m_indentLevel--;
            }

            inline void SetIndentLevel( S32 indentLevel )
            {
                KRG_ASSERT( indentLevel >= 0 );
                m_indentLevel = indentLevel;
            }

            //-------------------------------------------------------------------------

            inline S32 GetMaxHeaderWidth() const { return m_maxHeaderWidth; }

            inline void UpdateMaxHeaderWidth( S32 headerWidth ) 
            {
                KRG_ASSERT( headerWidth >= 0 );
                m_maxHeaderWidth = Math::Max( m_maxHeaderWidth, headerWidth ); 
            }

        private:

            S32                 m_indentLevel = 0;
            S32                 m_indentPerLevel = 20;
            mutable S32         m_maxHeaderWidth = 0;
        };

        //-------------------------------------------------------------------------

        class PropertyBullet final : public QFrame
        {
            Q_OBJECT

        public:

            PropertyBullet();

        private:

            virtual QSize sizeHint() const override;
            virtual void paintEvent( QPaintEvent* pPaintEvent ) override;
        };

        //-------------------------------------------------------------------------
        // Base Property Editor
        //-------------------------------------------------------------------------
        /*
            ------------------------------------------------------------
            | Header || Extra Controls | Editor Widgets | Reset Button |
            ------------------------------------------------------------
            | Empty Layout for child properties                        |
            ------------------------------------------------------------
        */

        class PropertyEditor : public QWidget
        {
            Q_OBJECT

        signals:

            void OnValueChanged();

        public:

            PropertyEditor( InitializationContext& context, DynamicPropertyInstance* pPropertyInstance );

            virtual void SetHeaderWidth( InitializationContext& context, S32 headerWidth );

            // Useful for injecting extra controls from a parent widget i.e. array row controls
            QHBoxLayout* GetExtraControlsLayout() { return m_pExtraControlsLayout; }

        protected:

            void NotifyValueChanged();

            // Create a default header (indented label)
            void CreateDefaultHeader( InitializationContext& context );

            // Create a reset button for this editor (optional)
            void CreateResetButton( InitializationContext& context );

            // Update the disabled state of the reset button on the property state
            void UpdateResetButtonState();

            // Implementation of reset
            virtual void OnResetButtonClicked() = 0;

        protected:

            DynamicPropertyInstance*                       m_pPropertyInstance = nullptr;

            QGridLayout*                                m_pMainLayout = nullptr;
            QHBoxLayout*                                m_pHeaderLayout = nullptr;
            QHBoxLayout*                                m_pExtraControlsLayout = nullptr;
            QHBoxLayout*                                m_pEditorLayout = nullptr;
            QVBoxLayout*                                m_pChildContentLayout = nullptr;
            QPushButton*                                m_pResetButton = nullptr;

        private:

            QWidget*                                    m_pHeader = nullptr;
        };
    }
}