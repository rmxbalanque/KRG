#pragma once
#include "AnimationToolsGraph_Common.h"
#include "Tools/Core/GraphEditor/Flow/FlowGraph.h"
#include "Engine/Animation/Graph/AnimationGraphCommon.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ToolsGraphCompilationContext;
    class VariationHierarchy;

    //-------------------------------------------------------------------------

    class Tools_GraphNode : public GraphEditor::Flow::Node
    {
        KRG_REGISTER_TYPE( Tools_GraphNode );

    public:

        using GraphEditor::Flow::Node::Node;

        // Get the type of node this is, this is either the output type for the nodes with output or the first input for nodes with no outputs
        KRG_FORCE_INLINE ValueType GetValueType() const
        {
            if ( GetNumOutputPins() > 0 )
            {
                return ValueType( GetOutputPin( 0 )->m_type );
            }
            else if ( GetNumInputPins() > 0 )
            {
                return ValueType( GetInputPin( 0 )->m_type );
            }
            else
            {
                return ValueType::Unknown;
            }
        }

        virtual ImColor GetNodeColor() const override { return GetColorForValueType( GetValueType() ); }

        virtual ImColor GetPinColor( GraphEditor::Flow::Pin const& pin ) const override { return GetColorForValueType( (ValueType) pin.m_type ); }

        // Get the types of graphs that this node is allowed to be placed in
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const = 0;

        // Is this node a persistent node i.e. is it always initialized 
        virtual bool IsPersistentNode() const { return false; }

        // Compile this node into its runtime representation. Returns the node index of the compiled node.
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const { return InvalidIndex; }

    protected:

        KRG_FORCE_INLINE void CreateInputPin( char const* pPinName, ValueType pinType ) { GraphEditor::Flow::Node::CreateInputPin( pPinName, (uint32) pinType ); }
        KRG_FORCE_INLINE void CreateOutputPin( char const* pPinName, ValueType pinType, bool allowMultipleOutputConnections = false ) { GraphEditor::Flow::Node::CreateOutputPin( pPinName, (uint32) pinType, allowMultipleOutputConnections ); }

        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

        // Draw any extra information about the node
        virtual void DrawInfoText( GraphEditor::DrawingContext const& ctx ) {}
    };

    //-------------------------------------------------------------------------

    class Tools_DataSlotNode : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_DataSlotNode );

    public:

        struct OverrideValue : public IRegisteredType
        {
            KRG_REGISTER_TYPE( OverrideValue );

            KRG_REGISTER StringID               m_variationID;
            KRG_REGISTER ResourceID             m_resourceID;
        };

    public:

        bool AreSlotValuesValid() const;
        virtual ResourceTypeID GetSlotResourceType() const { return ResourceTypeID::Unknown; }

        // This will return the final resolved resource value for this slot
        ResourceID GetValue( VariationHierarchy const& variationHierarchy, StringID variationID ) const;

        // Variation override management
        //-------------------------------------------------------------------------

        bool HasOverrideForVariation( StringID variationID ) const { return GetOverrideValueForVariation( variationID ) != nullptr; }
        ResourceID* GetOverrideValueForVariation( StringID variationID );
        ResourceID const* GetOverrideValueForVariation( StringID variationID ) const { return const_cast<Tools_DataSlotNode*>( this )->GetOverrideValueForVariation( variationID ); }

        void CreateOverride( StringID variationID );
        void RenameOverride( StringID oldVariationID, StringID newVariationID );
        void RemoveOverride( StringID variationID );
        void SetOverrideValueForVariation( StringID variationID, ResourceID const& resourceID );

    protected:

        KRG_REGISTER ResourceID                 m_defaultResourceID;
        KRG_REGISTER TVector<OverrideValue>     m_overrides;
    };
}