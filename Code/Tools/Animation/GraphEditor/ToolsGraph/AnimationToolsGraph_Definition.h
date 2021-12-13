#pragma once
#include "AnimationToolsGraph_FlowGraph.h"
#include "AnimationToolsGraph_StateMachineGraph.h"
#include "AnimationToolsGraph_Variations.h"
#include "Nodes/AnimationToolsNode_Parameters.h"
#include "Engine/Animation/Graph/AnimationGraphResources.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ToolsGraphCompilationContext;

    //-------------------------------------------------------------------------

    class AnimationGraphToolsDefinition
    {

    public:

        // Graph will be initialize in an invalid state, you need to either create a new graph or load an existing graph
        AnimationGraphToolsDefinition() = default;
        ~AnimationGraphToolsDefinition();

        inline bool IsValid() const { return m_pRootGraph != nullptr; }
        inline bool IsDirty() const { return m_isDirty; }
        inline void MarkDirty() { m_isDirty = true; }
        inline void ClearDirty() { m_isDirty = false; }

        // File Operations
        //-------------------------------------------------------------------------

        // Creates a new empty graph
        void CreateNew();

        // Load an existing graph
        bool LoadFromJson( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphDescriptorObjectValue );

        // Saves this graph
        void SaveToJson( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const;

        // Graph
        //-------------------------------------------------------------------------

        inline FlowGraph* GetRootGraph() { return m_pRootGraph; }
        inline FlowGraph const* GetRootGraph() const { return m_pRootGraph; }

        template<typename T>
        TInlineVector<T*, 20> FindAllNodesOfType( GraphEditor::SearchMode mode = GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch typeMatch = GraphEditor::SearchTypeMatch::Exact )
        {
            static_assert( std::is_base_of<Tools_GraphNode, T>::value );
            return m_pRootGraph->FindAllNodesOfType<T>( mode, typeMatch );
        }

        template<typename T>
        TInlineVector<T const*, 20> FindAllNodesOfType( GraphEditor::SearchMode mode = GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch typeMatch = GraphEditor::SearchTypeMatch::Exact ) const
        {
            static_assert( std::is_base_of<Tools_GraphNode, T>::value );
            return m_pRootGraph->FindAllNodesOfType<T const>( mode, typeMatch );
        }

        // Parameters
        //-------------------------------------------------------------------------

        inline int32 GetNumControlParameters() const { return (int32) m_controlParameters.size(); }
        inline int32 GetNumVirtualParameters() const { return (int32) m_virtualParameters.size(); }

        inline TVector<Tools_ControlParameterNode*> const& GetControlParameters() const { return m_controlParameters; }
        inline TVector<Tools_VirtualParameterNode*> const& GetVirtualParameters() const { return m_virtualParameters; }

        Tools_ControlParameterNode* FindControlParameter( UUID parameterID ) const;
        Tools_VirtualParameterNode* FindVirtualParameter( UUID parameterID ) const;

        void CreateControlParameter( ValueType type );
        void CreateVirtualParameter( ValueType type );

        void RenameControlParameter( UUID parameterID, String newName );
        void RenameVirtualParameter( UUID parameterID, String newName );

        void DestroyControlParameter( UUID parameterID );
        void DestroyVirtualParameter( UUID parameterID );

        // Variations and Data Slots
        //-------------------------------------------------------------------------

        inline bool IsDefaultVariationSelected() const { return m_selectedVariationID == AnimationGraphVariation::DefaultVariationID; }
        inline StringID GetSelectedVariationID() const { return m_selectedVariationID; }
        inline void SetSelectedVariation( StringID variationID ) { KRG_ASSERT( IsValidVariation( variationID ) ); m_selectedVariationID = variationID; }

        inline VariationHierarchy const& GetVariationHierarchy() const { return m_variationHierarchy; }
        inline VariationHierarchy& GetVariationHierarchy() { return m_variationHierarchy; }
        inline bool IsValidVariation( StringID variationID ) const { return m_variationHierarchy.IsValidVariation( variationID ); }
        Variation const* GetVariation( StringID variationID ) const { return m_variationHierarchy.GetVariation( variationID ); }
        Variation* GetVariation( StringID variationID ) { return m_variationHierarchy.GetVariation( variationID ); }

        inline TInlineVector<Tools_DataSlotNode*, 20> GetAllDataSlotNodes() { return FindAllNodesOfType<Tools_DataSlotNode>( GraphEditor::SearchMode::Recursive, GraphEditor::SearchTypeMatch::Derived ); }
        inline TInlineVector<Tools_DataSlotNode const*, 20> GetAllDataSlotNodes() const { return FindAllNodesOfType<Tools_DataSlotNode>( GraphEditor::SearchMode::Recursive, GraphEditor::SearchTypeMatch::Derived ); }

        // Compilation
        //-------------------------------------------------------------------------

        bool Compile( ToolsGraphCompilationContext& context ) const;

    private:

        void EnsureUniqueParameterName( String& parameterName ) const;

        // Frees all memory and resets the internal state
        void ResetInternalState();

    private:

        FlowGraph*                                      m_pRootGraph = nullptr;
        VariationHierarchy                              m_variationHierarchy;
        TVector<Tools_ControlParameterNode*>            m_controlParameters;
        TVector<Tools_VirtualParameterNode*>            m_virtualParameters;
        StringID                                        m_selectedVariationID = AnimationGraphVariation::DefaultVariationID;
        bool                                            m_isDirty = false;
    };
}