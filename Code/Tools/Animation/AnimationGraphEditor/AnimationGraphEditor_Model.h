#pragma once
#include "Tools/Core/Editor/EditorModel.h"
#include "Tools/Animation/Graph/AnimationGraphTools_AnimationGraph.h"
#include "System/Core/Types/Containers.h"
#include "Tools/Core/Trees/CategoryTree.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphEditorModel : public EditorModel
    {
    public:

        GraphEditorModel();
        ~GraphEditorModel();

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

        // File Operations
        //-------------------------------------------------------------------------

        void NewGraph();
        void OpenGraph();
        void SaveGraph();
        void SaveGraphAsNewFile();

        // Graph
        //-------------------------------------------------------------------------

        ToolsAnimationGraph* GetGraph() { return m_pGraph; }
        ToolsAnimationGraph const* GetGraph() const { return m_pGraph; }
        ToolsGraph* GetCurrentlyViewedGraph() const { return m_pNodeGraphToView; }

        void SimulateCompilation();

        // Node Types
        //-------------------------------------------------------------------------

        inline Category<TypeSystem::TypeInfo const*> const& GetNodeTypes() const { return m_categorizedNodeTypes.GetRootCategory(); }

        // Selection
        //-------------------------------------------------------------------------

        inline TVector<ToolsNode*> const& GetSelectedNodes() const { return m_selectedNodes; }
        inline void ClearSelection() { m_selectedNodes.clear(); }
        inline void SetSelection( TVector<ToolsNode*> const& inSelection ) { m_selectedNodes = inSelection; }
        
        inline void SetSelection( ToolsNode* pNode )
        {
            KRG_ASSERT( pNode != nullptr );
            m_selectedNodes.clear();
            m_selectedNodes.emplace_back( pNode );
        }

        inline void AddToSelection( ToolsNode* pNode )
        {
            KRG_ASSERT( pNode != nullptr );
            m_selectedNodes.emplace_back( pNode );
        }

        inline void RemoveFromSelection( ToolsNode* pNode )
        {
            m_selectedNodes.erase_first( pNode );
        }

        inline bool IsSelected( ToolsNode* pNode ) const
        {
            return VectorContains( m_selectedNodes, pNode );
        }

    private:

        // Shows the dirty flag dialog, return whether the calling operation should continue
        bool ShowDirtyDialog();

    public:

        ToolsAnimationGraph*                        m_pGraph = nullptr;
        FileSystem::Path                            m_currentlyOpenGraphPath;
        ToolsGraph*                                 m_pNodeGraphToView = nullptr;
        TVector<ToolsNode*>                         m_selectedNodes;
        TVector<TypeSystem::TypeInfo const*>        m_registeredNodeTypes;
        CategoryTree<TypeSystem::TypeInfo const*>   m_categorizedNodeTypes;
    };
}