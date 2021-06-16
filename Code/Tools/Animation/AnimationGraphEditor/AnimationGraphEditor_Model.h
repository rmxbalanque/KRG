#pragma once
#include "Tools/Core/Editor/EditorModel.h"
#include "Tools/Animation/Graph/AnimationGraphTools_AnimationGraph.h"
#include "Tools/Core/Trees/CategoryTree.h"
#include "Engine/Animation/Graph/AnimationGraphResources.h"
#include "System/Core/Types/Containers.h"

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
        GraphEditor::BaseGraph* GetCurrentlyViewedGraph() const { return m_pViewedGraph; }

        void NavigateTo( GraphEditor::BaseNode const* pNode );
        void NavigateTo( GraphEditor::BaseGraph const* pGraph );

        void SimulateCompilation();

        // Variation
        //-------------------------------------------------------------------------

        inline bool IsDefaultVariationSelected() const { return m_selectedVariationID == AnimationGraphVariation::DefaultVariationID; }
        inline StringID GetSelectedVariationID() const { return m_selectedVariationID; }
        inline void SetSelectedVariation( StringID variationID ) { KRG_ASSERT( m_pGraph->IsValidVariation( variationID ) ); m_selectedVariationID = variationID; }

        // Node Types
        //-------------------------------------------------------------------------

        inline Category<TypeSystem::TypeInfo const*> const& GetNodeTypes() const { return m_categorizedNodeTypes.GetRootCategory(); }

        // Selection
        //-------------------------------------------------------------------------

        inline TVector<GraphEditor::BaseNode*> const& GetSelectedNodes() const { return m_selectedNodes; }
        inline void ClearSelection() { m_selectedNodes.clear(); }
        inline void SetSelection( TVector<GraphEditor::BaseNode*> const& inSelection ) { m_selectedNodes = inSelection; }
        
        inline void SetSelection( GraphEditor::BaseNode* pNode )
        {
            KRG_ASSERT( pNode != nullptr );
            m_selectedNodes.clear();
            m_selectedNodes.emplace_back( pNode );
        }

        inline void AddToSelection( GraphEditor::BaseNode* pNode )
        {
            KRG_ASSERT( pNode != nullptr );
            m_selectedNodes.emplace_back( pNode );
        }

        inline void RemoveFromSelection( GraphEditor::BaseNode* pNode )
        {
            m_selectedNodes.erase_first( pNode );
        }

        inline bool IsSelected( GraphEditor::BaseNode* pNode ) const
        {
            return VectorContains( m_selectedNodes, pNode );
        }

    private:

        // Shows the dirty flag dialog, return whether the calling operation should continue
        bool ShowDirtyDialog();

    public:

        ToolsAnimationGraph*                        m_pGraph = nullptr;
        FileSystem::Path                            m_currentlyOpenGraphPath;
        StringID                                    m_selectedVariationID = AnimationGraphVariation::DefaultVariationID;

        TVector<TypeSystem::TypeInfo const*>        m_registeredNodeTypes;
        CategoryTree<TypeSystem::TypeInfo const*>   m_categorizedNodeTypes;

        GraphEditor::BaseGraph*                     m_pViewedGraph = nullptr;
        TVector<GraphEditor::BaseNode*>             m_selectedNodes;
    };
}