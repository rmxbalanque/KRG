#include "MapEditor.h"
#include "EntityOutlinerWidget.h"
#include "EditorContext.h"
#include "Tools/Entity/Serialization/EntityCollectionWriter.h"
#include "Tools/Entity/ToolEntityCollectionConverter.h"
#include "System/Entity/Collections/EntityCollectionDescriptor.h"
#include "System/Core/FileSystem/FileSystem.h"

#include <QMenuBar>
#include <QMenu>
#include <DockManager.h>
#include "QFileDialog"
#include "EntityEditorWidget.h"
#include "Tools/Entity/Serialization/EntityCollectionReader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    MapEditor::MapEditor( TypeSystem::TypeRegistry const& typeRegistry )
        : QObject()
        , m_typeRegistry( typeRegistry )
        , m_map( typeRegistry )
    {}

    void MapEditor::Initialize( EditorContext const& context )
    {
        m_pDockManager = context.m_pDockManager;

        //-------------------------------------------------------------------------

        auto pLoadAction = new QAction( "&Load Map", this );
        connect( pLoadAction, &QAction::triggered, this, [this] () { ShowLoadDialog(); } );

        auto pSaveAction = new QAction( "&Save Map", this );
        connect( pSaveAction, &QAction::triggered, this, [this] () { ShowSaveDialog(); } );

        auto pOpenOutliner = new QAction( "Entity Out&liner", this );
        connect( pOpenOutliner, &QAction::triggered, this, [this] () { CreateEntityOutlinerTab(); } );

        auto pOpenEditor = new QAction( "&Entity Editor", this );
        connect( pOpenEditor, &QAction::triggered, this, [this] () { CreateEntityEditorTab(); } );

        //-------------------------------------------------------------------------

        auto pMenu = new QMenu( "&Map" );
        pMenu->addAction( pLoadAction );
        pMenu->addAction( pSaveAction );
        pMenu->addAction( pOpenOutliner );
        pMenu->addAction( pOpenEditor );
        context.m_pMainMenu->addMenu( pMenu );

        //-------------------------------------------------------------------------

        CreateEntityOutlinerTab();
        CreateEntityEditorTab();
    }

    void MapEditor::Shutdown()
    {
        m_pDockManager = nullptr;
    }

    //-------------------------------------------------------------------------

    void MapEditor::ShowLoadDialog()
    {
        QFileDialog dialog;
        dialog.setFileMode( QFileDialog::AnyFile );
        dialog.setViewMode( QFileDialog::Detail );
        dialog.setNameFilter( tr( "Maps (*.map)" ) );

        dialog.setDirectory( "D:\\Kruger\\Data" );

        // Restrict to data folder
        //-------------------------------------------------------------------------

        auto onChangeDirectory = [this, &dialog] ( QString const& newDirectoryString )
        {
            FileSystemPath const newDirectoryPath( newDirectoryString.toUtf8().data() );
            if ( !newDirectoryPath.IsUnderDirectory( "D:\\Kruger\\Data" ) )
            {
                dialog.setDirectory( "D:\\Kruger\\Data" );
            }
        };

        connect( &dialog, &QFileDialog::directoryEntered, this, onChangeDirectory );

        //-------------------------------------------------------------------------

        if ( dialog.exec() )
        {
            if ( dialog.selectedFiles().size() > 0 )
            {
                // We only allow selecting a single file
                QString const& selectedFileStr = dialog.selectedFiles()[0];
                LoadMap( selectedFileStr.toUtf8().data() );
            }
        }
    }

    void MapEditor::ShowSaveDialog()
    {
        QFileDialog dialog;
        dialog.setFileMode( QFileDialog::AnyFile );
        dialog.setViewMode( QFileDialog::Detail );
        dialog.setNameFilter( tr( "Maps (*.map)" ) );

        dialog.setDirectory( "D:\\Kruger\\Data" );

        // Restrict to data folder
        //-------------------------------------------------------------------------

        auto onChangeDirectory = [this, &dialog] ( QString const& newDirectoryString )
        {
            FileSystemPath const newDirectoryPath( newDirectoryString.toUtf8().data() );
            if ( !newDirectoryPath.IsUnderDirectory( "D:\\Kruger\\Data" ) )
            {
                dialog.setDirectory( "D:\\Kruger\\Data" );
            }
        };

        connect( &dialog, &QFileDialog::directoryEntered, this, onChangeDirectory );

        //-------------------------------------------------------------------------

        if ( dialog.exec() )
        {
            if ( dialog.selectedFiles().size() > 0 )
            {
                // We only allow selecting a single file
                QString const& selectedFileStr = dialog.selectedFiles()[0];
                SaveMap( selectedFileStr.toUtf8().data() );
            }
        }
    }

    bool MapEditor::LoadMap( FileSystemPath const& path )
    {
        KRG_ASSERT( path.IsValid() );
       
        if ( !FileSystem::EnsurePathExists( path ) )
        {
            return false;
        }

        EntityModel::EntityCollectionDescriptor entityCollectionDescriptor;
        if ( !EntityModel::EntityCollectionReader::ReadCollection( m_typeRegistry, path, entityCollectionDescriptor ) )
        {
            return false;
        }

        if ( !EntityModel::ToolEntityCollectionConverter::ConvertToToolsFormat( m_typeRegistry, entityCollectionDescriptor, m_map ) )
        {
            return false;
        }

        //-------------------------------------------------------------------------

        if ( m_pOutlinerWidget != nullptr )
        {
            m_pOutlinerWidget->SetCollectionToOutline( &m_map );
        }

        return true;
    }

    bool MapEditor::SaveMap( FileSystemPath const& path )
    {
        KRG_ASSERT( path.IsValid() );

        if ( !FileSystem::EnsurePathExists( path ) )
        {
            return false;
        }

        EntityModel::EntityCollectionDescriptor entityCollectionDescriptor;

        if ( !EntityModel::ToolEntityCollectionConverter::ConvertFromToolsFormat( m_typeRegistry, m_map, entityCollectionDescriptor ) )
        {
            return false;
        }

        if ( !EntityModel::EntityCollectionWriter::WriteCollection( m_typeRegistry, path, entityCollectionDescriptor ) )
        {
            return false;
        }

        return true;
    }

    //-------------------------------------------------------------------------

    void MapEditor::CreateEntityOutlinerTab()
    {
        // We are only allowed one outliner widget
        if ( m_pOutlinerWidget != nullptr )
        {
            return;
        }

        //-------------------------------------------------------------------------

        m_pOutlinerWidget = new EntityOutlinerWidget();
        m_pOutlinerWidget->SetCollectionToOutline( &m_map );

        connect( m_pOutlinerWidget, &EntityOutlinerWidget::OnEntitySelected, this, &MapEditor::OnEntitySelected );

        //-------------------------------------------------------------------------

        auto pDockWidget = new ads::CDockWidget( "Entity Outliner" );
        pDockWidget->setWidget( m_pOutlinerWidget );
        pDockWidget->setFeature( ads::CDockWidget::DockWidgetDeleteOnClose, true );
        connect( pDockWidget, &ads::CDockWidget::closed, this, [this] () { m_pOutlinerWidget = nullptr; } );

        m_pDockManager->addDockWidget( ads::LeftDockWidgetArea, pDockWidget );
    }

    void MapEditor::CreateEntityEditorTab()
    {
        // We are only allowed one outliner widget
        if ( m_pEditorWidget != nullptr )
        {
            return;
        }

        //-------------------------------------------------------------------------

        m_pEditorWidget = new EntityEditorWidget( m_typeRegistry );
        //m_pEditorWidget->SetCollectionToOutline( &m_map );

        auto pDockWidget = new ads::CDockWidget( "Entity Editor" );
        pDockWidget->setWidget( m_pEditorWidget );
        pDockWidget->setFeature( ads::CDockWidget::DockWidgetDeleteOnClose, true );
        connect( pDockWidget, &ads::CDockWidget::closed, this, [this] () { m_pEditorWidget = nullptr; } );

        m_pDockManager->addDockWidget( ads::RightDockWidgetArea, pDockWidget );
    }

    //-------------------------------------------------------------------------

    void MapEditor::OnEntitySelected( EntityModel::ToolEntity* pSelectedEntity )
    {
        if ( m_pEditorWidget != nullptr )
        {
            m_pEditorWidget->SetEntityToEdit( pSelectedEntity );
        }
    }
}