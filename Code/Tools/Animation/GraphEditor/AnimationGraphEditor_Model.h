#pragma once
#include "ToolsGraph/AnimationToolsGraph.h"
#include "Tools/Core/Trees/CategoryTree.h"
#include "Tools/Core/Resource/ResourceEditorWorkspace.h"
#include "Engine/Animation/Components/AnimationGraphComponent.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class Entity;
    namespace Animation { class AnimationGraphComponent; }
}

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphEditorModel
    {
    public:

        GraphEditorModel( ResourceEditorContext const& editorContext, ResourceID const& graphResourceID );
        ~GraphEditorModel();

        inline FileSystem::Path const& GetSourceDataDirectory() const { return m_editorContext.m_sourceDataDirectory; }
        inline FileSystem::Path const& GetCompiledDataDirectory() const { return m_editorContext.m_compiledDataDirectory; }
        inline TypeSystem::TypeRegistry const* GetTypeRegistry() const { return m_editorContext.m_pTypeRegistry; }
        inline Resource::ResourceSystem* GetResourceSystem() const { return m_editorContext.m_pResourceSystem; }

        bool SaveGraph();

        // Graph
        //-------------------------------------------------------------------------

        AnimationToolsGraph* GetGraph() { return m_pGraph; }
        AnimationToolsGraph const* GetGraph() const { return m_pGraph; }
        GraphEditor::BaseGraph* GetCurrentlyViewedGraph() const { return m_pViewedGraph; }

        void NavigateTo( GraphEditor::BaseNode const* pNode );
        void NavigateTo( GraphEditor::BaseGraph const* pGraph );

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

        // Preview
        //-------------------------------------------------------------------------

        inline bool IsPreviewing() const { return m_isPreviewing; }
        void StartPreview( EntityWorld* pPreviewWorld );
        void StopPreview( EntityWorld* pPreviewWorld );

        inline DebugContext* GetDebugContext() 
        {
            if ( !m_isPreviewing || !m_pGraphComponent->IsInitialized() )
            {
                return nullptr;
            }

            return &m_debugContext; 
        }

    private:

        // Shows the dirty flag dialog, return whether the calling operation should continue
        bool ShowDirtyDialog();

        // Saving
        void GenerateAnimGraphVariationDescriptors();

    public:

        ResourceEditorContext const&                m_editorContext;

        AnimationToolsGraph*                        m_pGraph = nullptr;
        FileSystem::Path                            m_graphFilePath;
        StringID                                    m_selectedVariationID = AnimationGraphVariation::DefaultVariationID;

        TVector<TypeSystem::TypeInfo const*>        m_registeredNodeTypes;
        CategoryTree<TypeSystem::TypeInfo const*>   m_categorizedNodeTypes;

        GraphEditor::BaseGraph*                     m_pViewedGraph = nullptr;
        TVector<GraphEditor::BaseNode*>             m_selectedNodes;

        Entity*                                     m_pPreviewEntity = nullptr;
        AnimationGraphComponent*                    m_pGraphComponent = nullptr;
        DebugContext                                m_debugContext;
        bool                                        m_isPreviewing = false;
    };
}