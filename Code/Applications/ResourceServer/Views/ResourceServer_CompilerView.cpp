#include "ResourceServer_CompilerView.h"
#include "Tools/UI/Widgets/IconWidgets.h"
#include "ResourceServer/ResourceServer.h"

#include "QPushButton"
#include "QBoxLayout"
#include "QScrollArea"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        CompilerView::CompilerView( ResourceServer& resourceServer )
            : m_resourceServer( resourceServer )
            , QWidget()
        {
            auto pCompilerListLayout = new QVBoxLayout( this );

            for ( auto const& compilerInfo : m_resourceServer.GetKnownCompilers() )
            {
                auto pCompilerNameWidget = new QLabel( compilerInfo.m_name.c_str() );
                auto pCompilerModuleWidget = new QLabel( compilerInfo.m_sourceModule.c_str() );
                auto pCompilerVersionWidget = new QLabel( QString("Version %1").arg( compilerInfo.m_version ) );

                size_t const numTypes = compilerInfo.m_outputTypes.size();
                QString typesList;
                for ( S32 i = 0; i < numTypes; i++ )
                {
                    typesList.append( compilerInfo.m_outputTypes[i].ToString().c_str() );
                    if ( i != numTypes - 1 )
                    {
                        typesList.append( ", " );
                    }
                }

                auto pCompilerResourceTypesWidget = new QLabel( typesList );

                //-------------------------------------------------------------------------

                auto pCompileWidgetDataLayout = new QVBoxLayout;
                pCompileWidgetDataLayout->setSpacing( 0 );
                pCompileWidgetDataLayout->addWidget( pCompilerNameWidget, 0 );
                pCompileWidgetDataLayout->addWidget( pCompilerResourceTypesWidget, 0 );
                pCompileWidgetDataLayout->addWidget( pCompilerVersionWidget, 0 );
                pCompileWidgetDataLayout->addWidget( pCompilerModuleWidget, 0 );

                auto pCompileWidgetLayout = new QHBoxLayout;
                pCompileWidgetLayout->addWidget( new KIconLabel( ForkAwesome::Cogs, 48, 48 ), 0, Qt::AlignTop );
                pCompileWidgetLayout->addLayout( pCompileWidgetDataLayout, 1 );

                //-------------------------------------------------------------------------

                auto pCompilerWidget = new QWidget;
                pCompilerWidget->setLayout( pCompileWidgetLayout );
                pCompilerListLayout->addWidget( pCompilerWidget );
            }

            //-------------------------------------------------------------------------

            auto pCompilerListWidget = new QWidget;
            pCompilerListWidget->setLayout( pCompilerListLayout );

            auto pScrollArea = new QScrollArea;
            pScrollArea->setWidgetResizable( true );
            pScrollArea->setWidget( pCompilerListWidget );

            //-------------------------------------------------------------------------

            auto pLayout = new QVBoxLayout( this );
            pLayout->addWidget( pScrollArea );
            setLayout( pLayout );
        }
    }
}