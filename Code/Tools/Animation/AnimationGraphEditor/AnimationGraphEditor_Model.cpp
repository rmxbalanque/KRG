#include "AnimationGraphEditor_Model.h"
#include "Tools/Core/Thirdparty/KRG_RapidJson.h"
#include "Tools/Core/Thirdparty/pfd/portable-file-dialogs.h"
#include "Tools/Animation/Graph/AnimationGraphTools_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    GraphEditorModel::GraphEditorModel()
    {
        NewGraph();
    }

    GraphEditorModel::~GraphEditorModel()
    {
        KRG::Delete( m_pGraph );
    }

    void GraphEditorModel::Initialize( UpdateContext const& context )
    {
        EditorModel::Initialize( context );

        m_registeredNodeTypes = m_pTypeRegistry->GetAllDerivedTypes( ToolsNode::GetStaticTypeID(), false, false );

        for ( auto pNodeType : m_registeredNodeTypes )
        {
            auto pDefaultNode = SafeCast<ToolsNode const>( pNodeType->m_pTypeHelper->GetDefaultTypeInstancePtr() );
            if ( pDefaultNode->IsUserCreatable() )
            {
                m_categorizedNodeTypes.AddItem( pDefaultNode->GetCategory(), pDefaultNode->GetTypeName(), pNodeType );
            }
        }
    }

    void GraphEditorModel::Shutdown( UpdateContext const& context )
    {
        EditorModel::Shutdown( context );
    }

    //-------------------------------------------------------------------------

    bool GraphEditorModel::ShowDirtyDialog()
    {
        if ( m_pGraph->IsDirty() )
        {
            pfd::message msg( "Save Changes?", "Do you wish to save your changes, before proceeding?", pfd::choice::yes_no_cancel, pfd::icon::warning );

            switch ( msg.result() )
            {
                case pfd::button::ok:
                {
                    SaveGraph();
                }
                break;

                case pfd::button::cancel:
                {
                    return false;
                }
                break;
            }
        }

        return true;
    }

    void GraphEditorModel::NewGraph()
    {
        ShowDirtyDialog();

        //-------------------------------------------------------------------------

        KRG::Delete( m_pGraph );
        m_pGraph = KRG::New<ToolsAnimationGraph>();
        m_pGraph->CreateNew();
        m_currentlyOpenGraphPath = FileSystem::Path();
        m_pNodeGraphToView = m_pGraph->GetRootGraph();
    }

    void GraphEditorModel::OpenGraph()
    {
        ShowDirtyDialog();

        //-------------------------------------------------------------------------

        FileSystem::Path newGraphPath = m_sourceDataDirectory;
        pfd::open_file openDialog( "Save Graph", newGraphPath.c_str(), { "Animation Graph Files (.ag)", "*.ag", } );
        auto const& result = openDialog.result();
        if ( !result.empty() )
        {
            newGraphPath = result.back().c_str();

            if ( newGraphPath.IsValid() )
            {
                JsonFileReader reader;
                if ( !reader.ReadFromFile( newGraphPath ) )
                {
                    return;
                }

                // Destroy the old graph and try to the load the specified graph
                KRG::Delete( m_pGraph );
                m_pGraph = KRG::New<ToolsAnimationGraph>();
                if ( m_pGraph->Load( *m_pTypeRegistry, reader.GetDocument() ) )
                {
                    m_pNodeGraphToView = m_pGraph->GetRootGraph();
                    m_currentlyOpenGraphPath = newGraphPath;
                }
                else // Load failed, so clean up
                {
                    NewGraph();
                }
            }
        }
    }

    void GraphEditorModel::SaveGraph()
    {
        if ( m_currentlyOpenGraphPath.IsValid() )
        {
            JsonFileWriter writer;
            m_pGraph->Save( *m_pTypeRegistry, *writer.GetWriter() );
            writer.WriteToFile( m_currentlyOpenGraphPath );
        }
        else
        {
            SaveGraphAsNewFile();
        }
    }

    void GraphEditorModel::SaveGraphAsNewFile()
    {
        FileSystem::Path newGraphPath = m_sourceDataDirectory;
        pfd::save_file saveDialog( "Save Graph", newGraphPath.c_str(), { "Animation Graph Files (.ag)", "*.ag", } );
        newGraphPath = saveDialog.result().c_str();

        if ( newGraphPath.IsValid() )
        {
            JsonFileWriter writer;
            m_pGraph->Save( *m_pTypeRegistry, *writer.GetWriter() );
            writer.WriteToFile( newGraphPath );
            m_currentlyOpenGraphPath = newGraphPath;
        }
    }

    void GraphEditorModel::SimulateCompilation()
    {
        ToolsGraphCompilationContext context;
        bool result = m_pGraph->Compile( context );
    }
}