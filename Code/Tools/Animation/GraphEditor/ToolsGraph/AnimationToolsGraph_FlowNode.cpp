#include "AnimationToolsGraph_FlowNode.h"
#include "AnimationToolsGraph_Variations.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    static void TraverseHierarchy( GraphEditor::BaseNode const* pNode, TVector<GraphEditor::BaseNode const*>& nodePath )
    {
        KRG_ASSERT( pNode != nullptr );
        nodePath.emplace_back( pNode );

        if ( pNode->HasParentGraph() && !pNode->GetParentGraph()->IsRootGraph() )
        {
            TraverseHierarchy( pNode->GetParentGraph()->GetParentNode(), nodePath );
        }
    }

    //-------------------------------------------------------------------------

    void Tools_GraphNode::DrawExtraControls( GraphEditor::DrawContext const& ctx )
    {
        NodeIndex runtimeNodeIdx = InvalidIndex;
        auto pDebugContext = reinterpret_cast<DebugContext*>( ctx.m_pUserContext );
        if ( pDebugContext != nullptr )
        {
            runtimeNodeIdx = pDebugContext->GetRuntimeNodeIndex( GetID() );

            // Some nodes dont have runtime representations
            if ( runtimeNodeIdx == InvalidIndex )
            {
                return;
            }
        }

        //-------------------------------------------------------------------------
        // Draw Pose Node
        //-------------------------------------------------------------------------

        if ( GetValueType() == ValueType::Pose )
        {
            if ( pDebugContext != nullptr && pDebugContext->IsNodeActive( runtimeNodeIdx ) )
            {
                PoseNodeDebugInfo const debugInfo = pDebugContext->GetPoseNodeDebugInfo( runtimeNodeIdx );
                DrawPoseNodeDebugInfo( ctx, GetSize().x, debugInfo );
            }
            else
            {
                DrawEmptyPoseNodeDebugInfo( ctx, GetSize().x );
            }
        }

        //-------------------------------------------------------------------------
        // Draw Value Node
        //-------------------------------------------------------------------------

        else
        {
            if ( pDebugContext != nullptr && pDebugContext->IsNodeActive( runtimeNodeIdx ) )
            {
                ImGui::Text( "Value: TODO" );
            }
            else
            {
                ImGui::NewLine();
            }
        }

        //-------------------------------------------------------------------------

        DrawInfoText( ctx );
    }

    //-------------------------------------------------------------------------

    bool Tools_DataSlotNode::AreSlotValuesValid() const
    {
        if ( m_defaultResourceID.GetResourceTypeID() != GetSlotResourceType() )
        {
            return false;
        }

        for ( auto const& variation : m_overrides )
        {
            if ( !variation.m_variationID.IsValid() )
            {
                return false;
            }

            if ( variation.m_resourceID.GetResourceTypeID() != GetSlotResourceType() )
            {
                return false;
            }
        }

        return true;
    }

    ResourceID Tools_DataSlotNode::GetValue( VariationHierarchy const& variationHierarchy, StringID variationID ) const
    {
        KRG_ASSERT( variationHierarchy.IsValidVariation( variationID ) );

        if ( variationID == AnimationGraphVariation::DefaultVariationID )
        {
            return m_defaultResourceID;
        }

        //-------------------------------------------------------------------------

        ResourceID resourceID;

        auto TryGetResourceID = [this, &resourceID] ( StringID variationID )
        {
            if ( variationID == AnimationGraphVariation::DefaultVariationID )
            {
                resourceID = m_defaultResourceID;
                return true;
            }

            for ( auto const& variation : m_overrides )
            {
                if ( variation.m_variationID == variationID )
                {
                    resourceID = variation.m_resourceID;
                    return true;
                }
            }

            return false;
        };

        //-------------------------------------------------------------------------

        // Try get the resource ID for this variation
        if ( TryGetResourceID( variationID ) )
        {
            return resourceID;
        }

        // Go up the hierarchy and return the first if a override exists
        StringID parentVariationID = variationHierarchy.GetParentVariationID( variationID );
        while ( parentVariationID.IsValid() )
        {
            if ( TryGetResourceID( parentVariationID ) )
            {
                break;
            }

            parentVariationID = variationHierarchy.GetParentVariationID( parentVariationID );
        }

        return resourceID;
    }

    ResourceID* Tools_DataSlotNode::GetOverrideValueForVariation( StringID variationID )
    {
        KRG_ASSERT( variationID.IsValid() );

        if ( variationID == AnimationGraphVariation::DefaultVariationID )
        {
            return &m_defaultResourceID;
        }

        for ( auto& variation : m_overrides )
        {
            if ( variation.m_variationID == variationID )
            {
                return &variation.m_resourceID;
            }
        }

        return nullptr;
    }

    void Tools_DataSlotNode::SetOverrideValueForVariation( StringID variationID, ResourceID const& resourceID )
    {
        KRG_ASSERT( variationID.IsValid() );
        KRG_ASSERT( !resourceID.IsValid() || resourceID.GetResourceTypeID() == GetSlotResourceType() );

        if ( variationID == AnimationGraphVariation::DefaultVariationID )
        {
            m_defaultResourceID = resourceID;
            return;
        }

        //-------------------------------------------------------------------------

        KRG_ASSERT( HasOverrideForVariation( variationID ) );

        for ( auto& variation : m_overrides )
        {
            if ( variation.m_variationID == variationID )
            {
                variation.m_resourceID = resourceID;
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
    }

    void Tools_DataSlotNode::CreateOverride( StringID variationID )
    {
        KRG_ASSERT( variationID.IsValid() && variationID != AnimationGraphVariation::DefaultVariationID );
        KRG_ASSERT( !HasOverrideForVariation( variationID ) );

        auto& createdOverride = m_overrides.emplace_back();
        createdOverride.m_variationID = variationID;
    }

    void Tools_DataSlotNode::RenameOverride( StringID oldVariationID, StringID newVariationID )
    {
        KRG_ASSERT( oldVariationID.IsValid() && newVariationID.IsValid() );
        KRG_ASSERT( oldVariationID != AnimationGraphVariation::DefaultVariationID && newVariationID != AnimationGraphVariation::DefaultVariationID );

        for ( auto& overrideValue : m_overrides )
        {
            if( overrideValue.m_variationID == oldVariationID )
            {
                overrideValue.m_variationID = newVariationID;
            }
        }
    }

    void Tools_DataSlotNode::RemoveOverride( StringID variationID )
    {
        KRG_ASSERT( variationID.IsValid() && variationID != AnimationGraphVariation::DefaultVariationID );

        for ( auto iter = m_overrides.begin(); iter != m_overrides.end(); ++iter )
        {
            if ( iter->m_variationID == variationID )
            {
                m_overrides.erase_unsorted( iter );
                return;
            }
        }

        KRG_UNREACHABLE_CODE();
    }
}