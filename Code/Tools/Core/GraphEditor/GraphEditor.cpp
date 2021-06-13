#include "GraphEditor.h"
#include "Tools/Core/TypeSystem/Serialization/TypeSerialization.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    ImVec2 const Settings::s_graphTitleMargin( 4, 4 );
    float const Settings::s_gridSpacing = 20;

    //-------------------------------------------------------------------------

    void DrawCanvasGridAndTitle( DrawingContext& ctx, char const* const pGraphTitle )
    {
        ImVec2 const windowTL = ctx.m_windowRect.GetTL();
        float const canvasWidth = ctx.m_windowRect.GetWidth();
        float const canvasHeight = ctx.m_windowRect.GetHeight();

        //-------------------------------------------------------------------------

        // Draw Grid
        for ( float x = Math::FModF( ctx.m_viewOffset.x, Settings::s_gridSpacing ); x < canvasWidth; x += Settings::s_gridSpacing )
        {
            ctx.m_pDrawList->AddLine( windowTL + ImVec2( x, 0.0f ), windowTL + ImVec2( x, canvasHeight ), Settings::s_gridLineColor );
        }

        for ( float y = Math::FModF( ctx.m_viewOffset.y, Settings::s_gridSpacing ); y < canvasHeight; y += Settings::s_gridSpacing )
        {
            ctx.m_pDrawList->AddLine( windowTL + ImVec2( 0.0f, y ), windowTL + ImVec2( canvasWidth, y ), Settings::s_gridLineColor );
        }

        // Draw title
        {
            ImGuiX::ScopedFont font( ImGuiX::Font::ExtraLarge );
            ctx.m_pDrawList->AddText( ctx.m_windowRect.Min + Settings::s_graphTitleMargin, Settings::s_graphTitleColor, pGraphTitle );
        }
    }

    void DrawEmptyGrid()
    {
        auto pWindow = ImGui::GetCurrentWindow();
        auto pDrawList = ImGui::GetWindowDrawList();

        //-------------------------------------------------------------------------

        auto const windowRect = pWindow->Rect();
        ImVec2 const windowTL = windowRect.GetTL();
        float const canvasWidth = windowRect.GetWidth();
        float const canvasHeight = windowRect.GetHeight();

        //-------------------------------------------------------------------------

        // Draw Grid
        for ( float x = Math::FModF( 0, Settings::s_gridSpacing ); x < canvasWidth; x += Settings::s_gridSpacing )
        {
            pDrawList->AddLine( windowTL + ImVec2( x, 0.0f ), windowTL + ImVec2( x, canvasHeight ), Settings::s_gridLineColor );
        }

        for ( float y = Math::FModF( 0, Settings::s_gridSpacing ); y < canvasHeight; y += Settings::s_gridSpacing )
        {
            pDrawList->AddLine( windowTL + ImVec2( 0.0f, y ), windowTL + ImVec2( canvasWidth, y ), Settings::s_gridLineColor );
        }
    }

    //-------------------------------------------------------------------------

    BaseNode::~BaseNode()
    {
        KRG::Delete( m_pChildGraph );
        KRG::Delete( m_pSecondaryGraph );
    }

    BaseNode* BaseNode::CreateNodeFromSerializedData( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& nodeObjectValue, BaseGraph* pParentGraph )
    {
        TypeSystem::TypeID const nodeTypeID = nodeObjectValue["TypeData"]["TypeID"].GetString();
        auto pTypeInfo = typeRegistry.GetTypeInfo( nodeTypeID );
        KRG_ASSERT( pTypeInfo != nullptr );

        BaseNode* pNode = TypeSystem::Serialization::CreateAndReadNativeType<BaseNode>( typeRegistry, nodeObjectValue["TypeData"] );
        pNode->m_pParentGraph = pParentGraph;
        pNode->Serialize( typeRegistry, nodeObjectValue );
        return pNode;
    }

    //-------------------------------------------------------------------------

    BaseGraph::~BaseGraph()
    {
        for ( auto pNode : m_nodes )
        {
            KRG::Delete( pNode );
        }
    }

    void BaseGraph::FindAllNodesOfType( TypeSystem::TypeID typeID, TInlineVector<BaseNode*, 20>& results, bool includeDerivedNodes ) const
    {
        for ( auto pNode : m_nodes )
        {
            if ( pNode->GetTypeID() == typeID )
            {
                results.emplace_back( pNode );
            }
            else if ( includeDerivedNodes )
            {
                if ( pNode->GetTypeInfo()->IsDerivedFrom( typeID ) )
                {
                    results.emplace_back( pNode );
                }
            }

            //-------------------------------------------------------------------------

            if ( pNode->HasChildGraph() )
            {
                pNode->GetChildGraph()->FindAllNodesOfType( typeID, results, includeDerivedNodes );
            }

            //-------------------------------------------------------------------------

            if ( pNode->HasSecondaryGraph() )
            {
                pNode->GetSecondaryGraph()->FindAllNodesOfType( typeID, results, includeDerivedNodes );
            }
        }
    }

    BaseGraph* BaseGraph::CreateGraphFromSerializedData( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue, BaseNode* pParentNode )
    {
        TypeSystem::TypeID const graphTypeID = graphObjectValue["TypeData"]["TypeID"].GetString();
        auto pTypeInfo = typeRegistry.GetTypeInfo( graphTypeID );
        KRG_ASSERT( pTypeInfo != nullptr );

        BaseGraph* pGraph = TypeSystem::Serialization::CreateAndReadNativeType<BaseGraph>( typeRegistry, graphObjectValue["TypeData"] );
        pGraph->m_pParentNode = pParentNode;
        pGraph->Serialize( typeRegistry, graphObjectValue );
        return pGraph;
    }
}