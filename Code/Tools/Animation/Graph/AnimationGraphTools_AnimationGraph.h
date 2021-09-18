#pragma once
#include "AnimationGraphTools_FlowGraph.h"
#include "AnimationGraphTools_StateMachineGraph.h"
#include "AnimationGraphTools_Variations.h"
#include "Nodes/AnimationToolsNode_Parameters.h"
#include "Engine/Animation/Graph/AnimationGraphResources.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ToolsGraphCompilationContext;

    //-------------------------------------------------------------------------

    class KRG_TOOLS_ANIMATION_API ToolsAnimationGraph
    {

    public:

        // Graph will be initialize in an invalid state, you need to either create a new graph or load an existing graph
        ToolsAnimationGraph() = default;
        ~ToolsAnimationGraph();

        bool IsValid() const { return m_pRootGraph != nullptr; }

        bool IsDirty() const { return false; }

        // File Operations
        //-------------------------------------------------------------------------

        // Creates a new empty graph
        void CreateNew();

        // Load an existing graph
        bool Load( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue );

        // Saves this graph
        void Save( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const;

        // Graph
        //-------------------------------------------------------------------------

        inline FlowToolGraph* GetRootGraph() { return m_pRootGraph; }
        inline FlowToolGraph const* GetRootGraph() const { return m_pRootGraph; }

        template<typename T>
        TInlineVector<T*, 20> FindAllNodesOfType( GraphEditor::SearchMode mode = GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch typeMatch = GraphEditor::SearchTypeMatch::Exact )
        {
            static_assert( std::is_base_of<FlowToolsNode, T>::value );
            return m_pRootGraph->FindAllNodesOfType<T>( mode, typeMatch );
        }

        template<typename T>
        TInlineVector<T const*, 20> FindAllNodesOfType( GraphEditor::SearchMode mode = GraphEditor::SearchMode::Localized, GraphEditor::SearchTypeMatch typeMatch = GraphEditor::SearchTypeMatch::Exact ) const
        {
            static_assert( std::is_base_of<FlowToolsNode, T>::value );
            return m_pRootGraph->FindAllNodesOfType<T const>( mode, typeMatch );
        }

        // Parameters
        //-------------------------------------------------------------------------

        inline int32 GetNumControlParameters() const { return (int32) m_controlParameters.size(); }
        inline int32 GetNumVirtualParameters() const { return (int32) m_virtualParameters.size(); }

        inline TVector<ControlParameterToolsNode*> const& GetControlParameters() const { return m_controlParameters; }
        inline TVector<VirtualParameterToolsNode*> const& GetVirtualParameters() const { return m_virtualParameters; }

        ControlParameterToolsNode* FindControlParameter( UUID parameterID ) const;
        VirtualParameterToolsNode* FindVirtualParameter( UUID parameterID ) const;

        void CreateControlParameter( NodeValueType type );
        void CreateVirtualParameter( NodeValueType type );

        void RenameControlParameter( UUID parameterID, String newName );
        void RenameVirtualParameter( UUID parameterID, String newName );

        void DestroyControlParameter( UUID parameterID );
        void DestroyVirtualParameter( UUID parameterID );

        // Variations and Data Slots
        //-------------------------------------------------------------------------

        inline VariationHierarchy const& GetVariations() const { return m_variations; }
        inline VariationHierarchy& GetVariations() { return m_variations; }
        inline bool IsValidVariation( StringID variationID ) const { return m_variations.IsValidVariation( variationID ); }
        Variation const* GetVariation( StringID variationID ) const { return m_variations.GetVariation( variationID ); }
        Variation* GetVariation( StringID variationID ) { return m_variations.GetVariation( variationID ); }

        inline TInlineVector<DataSlotToolsNode*, 20> GetAllDataSlotNodes() { return FindAllNodesOfType<DataSlotToolsNode>( GraphEditor::SearchMode::Recursive, GraphEditor::SearchTypeMatch::Derived ); }
        inline TInlineVector<DataSlotToolsNode const*, 20> GetAllDataSlotNodes() const { return FindAllNodesOfType<DataSlotToolsNode>( GraphEditor::SearchMode::Recursive, GraphEditor::SearchTypeMatch::Derived ); }

        // Compilation
        //-------------------------------------------------------------------------

        bool Compile( ToolsGraphCompilationContext& context ) const;

    private:

        void EnsureUniqueParameterName( String& parameterName ) const;

        // Frees all memory and resets the internal state
        void ResetInternalState();

    private:

        FlowToolGraph*                                 m_pRootGraph = nullptr;
        VariationHierarchy                          m_variations;
        TVector<ControlParameterToolsNode*>         m_controlParameters;
        TVector<VirtualParameterToolsNode*>         m_virtualParameters;
    };
}