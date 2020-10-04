#include "PathValueEditors.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "QBoxLayout"
#include "QLineEdit"
#include "QFileDialog"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace ValueEditors
    {
        DataPathEditor::DataPathEditor( FileSystemPath const& dataDirectoryPath, DataPath value )
            : ValueEditorBase()
            , m_dataDirectoryPath( dataDirectoryPath )
            , m_value( value )
        {
            KRG_ASSERT( dataDirectoryPath.IsValid() && dataDirectoryPath.IsDirectoryPath() );

            auto pMainLayout = new QHBoxLayout( this);
            pMainLayout->setContentsMargins( 0, 0, 0, 0 );
            pMainLayout->setSpacing( 2 );

            //-------------------------------------------------------------------------

            m_pPathDisplay = new QLineEdit();
            m_pPathDisplay->setReadOnly( true );
            pMainLayout->addWidget( m_pPathDisplay, 1.0f );

            //-------------------------------------------------------------------------

            auto pBrowseButton = new QPushButton();
            pBrowseButton->setIcon( KIcon( MaterialDesign::Folder_Search ) );
            pBrowseButton->setFlat( true );
            pBrowseButton->setStyleSheet( "QPushButton{ margin: 0px 0px 0px 0px; padding: 0px 0px 0px 0px; } " );
            pBrowseButton->setToolTip( "Browse for file..." );
            pMainLayout->addWidget( pBrowseButton );

            //-------------------------------------------------------------------------

            UpdateDisplay();

            connect( pBrowseButton, &QPushButton::clicked, this, &DataPathEditor::OnBrowseButtonClicked );
        }

        void DataPathEditor::OnBrowseButtonClicked()
        {
            QFileDialog dialog( this );
            dialog.setFileMode( QFileDialog::AnyFile );
            dialog.setViewMode( QFileDialog::Detail );
            dialog.setNameFilter( tr( "Files (*.*)" ) );

            // Set start directory
            //-------------------------------------------------------------------------

            if ( m_value.IsValid() )
            {
                auto const currentResourceFilePath = m_value.ToFileSystemPath( m_dataDirectoryPath );
                dialog.setDirectory( currentResourceFilePath.GetParentDirectory().c_str() );
            }
            else
            {
                dialog.setDirectory( m_dataDirectoryPath.c_str() );
            }

            // Restrict to data folder
            //-------------------------------------------------------------------------

            auto onChangeDirectory = [this, &dialog] ( QString const& newDirectoryString )
            {
                FileSystemPath const newDirectoryPath( newDirectoryString.toUtf8().data() );
                if ( !newDirectoryPath.IsUnderDirectory( m_dataDirectoryPath ) )
                {
                    dialog.setDirectory( m_dataDirectoryPath.c_str() );
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
                    auto selectedResourceDataPath = DataPath::FromFileSystemPath( m_dataDirectoryPath, FileSystemPath( selectedFileStr.toUtf8().data() ) );
                    if ( selectedResourceDataPath.IsValid() )
                    {
                        SetValueInternal( selectedResourceDataPath );
                    }
                }
            }
        }

        void DataPathEditor::UpdateDisplay()
        {
            m_pPathDisplay->setText( m_value.c_str() );
        }

        void DataPathEditor::SetValueInternal( DataPath newValue )
        {
            DataPath oldValue = m_value;

            m_value = newValue;
            UpdateDisplay();
            emit OnValueChanged( m_value, oldValue );
        }

        //-------------------------------------------------------------------------

        ResourcePathEditor::ResourcePathEditor( FileSystemPath const& dataDirectoryPath, ResourceTypeID resourceTypeFilter, ResourceID value )
            : ValueEditorBase()
            , m_dataDirectoryPath( dataDirectoryPath )
            , m_resourceTypeFilter( resourceTypeFilter )
            , m_value( value )
        {
            KRG_ASSERT( dataDirectoryPath.IsValid() && dataDirectoryPath.IsDirectoryPath() );

            if ( resourceTypeFilter.IsValid() )
            {
                m_fileDialogFilterString = QString( "%1 files ( *.%1)" ).arg( resourceTypeFilter.ToString().c_str() );
            }
            else
            {
                m_fileDialogFilterString = QString( "Resource Files ( *.*)" );
            }

            //-------------------------------------------------------------------------

            auto pMainLayout = new QHBoxLayout( this );
            pMainLayout->setContentsMargins( 0, 0, 0, 0 );
            pMainLayout->setSpacing( 2 );

            //-------------------------------------------------------------------------

            m_pTypeDisplay = new QLabel();
            m_pTypeDisplay->setFixedSize( 40, 23 );
            m_pTypeDisplay->setAlignment( Qt::AlignCenter );
            m_pTypeDisplay->setStyleSheet( "QLabel{ background-color: #464646; padding: 0px 2px; margin: 0px 2px 0px 0px; font: bold 10px; border: 1px solid #262626; border-radius: 4px;}" );
            pMainLayout->addWidget( m_pTypeDisplay, 0.0f );

            //-------------------------------------------------------------------------

            m_pPathDisplay = new QLineEdit();
            m_pPathDisplay->setReadOnly( true );
            pMainLayout->addWidget( m_pPathDisplay, 1.0f );

            //-------------------------------------------------------------------------

            auto pBrowseButton = new QPushButton();
            pBrowseButton->setIcon( KIcon( MaterialDesign::Folder_Search ) );
            pBrowseButton->setFlat( true );
            pBrowseButton->setStyleSheet( "QPushButton{ margin: 0px 0px 0px 0px; padding: 0px 0px 0px 0px; } " );
            pBrowseButton->setToolTip( QString( "Browse for %1 resource..." ).arg( resourceTypeFilter.ToString().c_str() ) );
            pMainLayout->addWidget( pBrowseButton );

            //-------------------------------------------------------------------------

            UpdateDisplay();

            connect( pBrowseButton, &QPushButton::clicked, this, &ResourcePathEditor::OnBrowseButtonClicked );
        }

        void ResourcePathEditor::OnBrowseButtonClicked()
        {
            QFileDialog dialog( this );
            dialog.setFileMode( QFileDialog::ExistingFile );
            dialog.setViewMode( QFileDialog::Detail );
            dialog.setNameFilter( m_fileDialogFilterString );

            // Set start directory
            //-------------------------------------------------------------------------

            if ( m_value.IsValid() )
            {
                auto const currentResourceFilePath = m_value.GetDataPath().ToFileSystemPath( m_dataDirectoryPath );
                dialog.setDirectory( currentResourceFilePath.GetParentDirectory().c_str() );
            }
            else
            {
                dialog.setDirectory( m_dataDirectoryPath.c_str() );
            }

            // Restrict to data folder
            //-------------------------------------------------------------------------

            auto onChangeDirectory = [this, &dialog] ( QString const& newDirectoryString )
            {
                FileSystemPath const newDirectoryPath( newDirectoryString.toUtf8().data() );
                if ( !newDirectoryPath.IsUnderDirectory( m_dataDirectoryPath ) )
                {
                    dialog.setDirectory( m_dataDirectoryPath.c_str() );
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
                    auto selectedResourceDataPath = DataPath::FromFileSystemPath( m_dataDirectoryPath, FileSystemPath( selectedFileStr.toUtf8().data() ) );
                    ResourceID newResourceID( selectedResourceDataPath );
                    if ( newResourceID.IsValid() )
                    {
                        SetValueInternal( newResourceID );
                    }
                }
            }
        }

        void ResourcePathEditor::UpdateDisplay()
        {
            if ( m_resourceTypeFilter == ResourceTypeID::Unknown )
            {
                m_pTypeDisplay->setText( "*" );
            }
            else
            {
                m_pTypeDisplay->setText( m_resourceTypeFilter.ToString().c_str() );
            }

            m_pPathDisplay->setText( m_value.c_str() );
        }

        void ResourcePathEditor::SetValueInternal( ResourceID newValue )
        {
            ResourceID oldValue = m_value;

            m_value = newValue;
            UpdateDisplay();
            emit OnValueChanged( m_value, oldValue );
        }
    }
}