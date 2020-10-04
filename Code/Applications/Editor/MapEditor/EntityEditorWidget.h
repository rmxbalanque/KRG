#pragma once
#include "QWidget"
#include "QPushButton"
#include "QStackedLayout"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem 
    {
        struct TypeInfo;
        class TypeRegistry;
    }

    class ToolEntity;
    class ToolEntityComponent;
    class EntityComponentEditorWidget;
    class PropertyGridWidget;

    //-------------------------------------------------------------------------

    class EntityEditorWidget : public QWidget
    {
        Q_OBJECT

    public:

        EntityEditorWidget( TypeSystem::TypeRegistry const& typeRegistry );

        void SetEntityToEdit( ToolEntity* pEntity );

    private:

        void AddComponent( TypeSystem::TypeInfo const* pComponentTypeInfo );
        void OnComponentSelected( ToolEntityComponent* pComponent );

    private:

        EntityComponentEditorWidget*    m_pComponentEditor = nullptr;
        PropertyGridWidget*             m_pPropertyGrid = nullptr;
    };
}
