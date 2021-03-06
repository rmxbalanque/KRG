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
    namespace TypeSystem { class ToolPropertyInstance; }

    //-------------------------------------------------------------------------

    namespace PropertyGrid
    {
        struct InitializationContext
        {
            InitializationContext( int32 desiredMinimumHeaderWidth = 0 ) 
                : m_maxHeaderWidth( desiredMinimumHeaderWidth )
            {
                KRG_ASSERT( desiredMinimumHeaderWidth >= 0 );
            }

            //-------------------------------------------------------------------------

            inline int32 GetIndent() const { return m_indentLevel * m_indentPerLevel; }

            inline int32 GetIndentLevel() const { return m_indentLevel; }

            inline void IncreaseIndentLevel() 
            { 
                m_indentLevel++;
            }

            inline void DecreaseIndentLevel()
            { 
                KRG_ASSERT( m_indentLevel > 0 );
                m_indentLevel--;
            }

            inline void SetIndentLevel( int32 indentLevel )
            {
                KRG_ASSERT( indentLevel >= 0 );
                m_indentLevel = indentLevel;
            }

            //-------------------------------------------------------------------------

            inline int32 GetMaxHeaderWidth() const { return m_maxHeaderWidth; }

            inline void UpdateMaxHeaderWidth( int32 headerWidth ) 
            {
                KRG_ASSERT( headerWidth >= 0 );
                m_maxHeaderWidth = Math::Max( m_maxHeaderWidth, headerWidth ); 
            }

        private:

            int32                 m_indentLevel = 0;
            int32                 m_indentPerLevel = 20;
            mutable int32         m_maxHeaderWidth = 0;
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

            PropertyEditor( InitializationContext& context, TypeSystem::ToolPropertyInstance* pPropertyInstance );

            virtual void SetHeaderWidth( InitializationContext& context, int32 headerWidth );

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

            TypeSystem::ToolPropertyInstance*           m_pPropertyInstance = nullptr;

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