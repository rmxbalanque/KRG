#pragma once

#include "EditorContext.h"
#include "Tools/Entity/ToolEntityCollection.h"

#include <QObject>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem { class TypeRegistry; }
    class EntityOutlinerWidget;
    class EntityEditorWidget;

    //-------------------------------------------------------------------------

    class MapEditor : public QObject
    {
        Q_OBJECT

    public:

        MapEditor( TypeSystem::TypeRegistry const& typeRegistry );

        void Initialize( EditorContext const& context );
        void Shutdown();

    private:

        void ShowLoadDialog();
        void ShowSaveDialog();

        bool LoadMap( FileSystemPath const& path );
        bool SaveMap( FileSystemPath const& path );

        void CreateEntityOutlinerTab();
        void CreateEntityEditorTab();

        void OnEntitySelected( EntityModel::ToolEntity* pSelectedEntity );

    private:

        TypeSystem::TypeRegistry const&     m_typeRegistry;
        ads::CDockManager*                  m_pDockManager = nullptr;

        EntityModel::ToolEntityCollection   m_map;
        FileSystemPath                      m_currentMapPath;

        EntityOutlinerWidget*               m_pOutlinerWidget = nullptr;
        EntityEditorWidget*                 m_pEditorWidget = nullptr;
    };
}