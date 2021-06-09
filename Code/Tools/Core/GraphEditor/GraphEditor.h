#pragma once

#include "Tools/Core/_Module/API.h"
#include "Tools/Core/ThirdParty/KRG_RapidJson.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Core/Core/IntegralTypes.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    struct Settings
    {
        // Colors
        //-------------------------------------------------------------------------

        static uint32 const s_gridBackgroundColor = IM_COL32( 40, 40, 50, 200 );
        static uint32 const s_gridLineColor = IM_COL32( 200, 200, 200, 40 );
        static uint32 const s_graphTitleColor = IM_COL32( 255, 255, 255, 255 );
        static uint32 const s_nodeBackgroundColor = IM_COL32( 50, 50, 50, 255 );
        static uint32 const s_nodeBackgroundHoveredColor = IM_COL32( 75, 75, 75, 255 );
        static uint32 const s_nodeBackgroundSelectedColor = IM_COL32( 75, 75, 75, 255 );
        static uint32 const s_invalidConnectionColor = IM_COL32( 255, 0, 0, 255 );
        static uint32 const s_selectionBoxOutlineColor = IM_COL32( 61, 133, 224, 30 );
        static uint32 const s_selectionBoxFillColor = IM_COL32( 61, 133, 224, 150 );

        // UI
        //-------------------------------------------------------------------------

        static ImVec2 const s_graphTitleMargin;
        static float const  s_gridSpacing;
    };

    //-------------------------------------------------------------------------

    enum class DragMode
    {
        None,
        View,
        Selection,
        Node,
        Connection,
    };

    // Graph drawing context
    //-------------------------------------------------------------------------

    struct DrawingContext
    {
        inline bool IsMouseInViewWindow() const
        {
            return m_windowRect.Contains( m_mouseScreenPos );
        }

        // Convert from a position relative to the window TL to a position relative to the screen TL
        inline ImVec2 WindowToScreenPosition( ImVec2 const& windowPosition ) const
        {
            return windowPosition + m_windowRect.Min;
        }

        // Convert from a position relative to the window TL to a position relative to the canvas origin
        inline ImVec2 WindowPositionToCanvasPosition( ImVec2 const& windowPosition ) const
        {
            return windowPosition + m_viewOffset;
        }

        // Convert from a position relative to the canvas origin to a position relative to the window TL
        inline ImVec2 CanvasPositionToWindowPosition( ImVec2 const& canvasPosition ) const
        {
            return canvasPosition - m_viewOffset;
        }

        // Convert from a position relative to the screen TL to a position relative to the window TL
        inline ImVec2 ScreenPositionToWindowPosition( ImVec2 const& screenPosition ) const
        {
            return screenPosition - m_windowRect.Min;
        }

        // Convert from a position relative to the screen TL to a position relative to the canvas origin
        inline ImVec2 ScreenPositionToCanvasPosition( ImVec2 const& screenPosition ) const
        {
            return screenPosition - m_windowRect.Min + m_viewOffset;
        }

        // Is a supplied rect within the canvas visible area
        inline bool IsItemVisible( ImRect const& itemCanvasRect ) const
        {
            return m_canvasVisibleRect.Overlaps( itemCanvasRect );
        }

    public:

        ImDrawList*         m_pDrawList = nullptr;
        ImVec2              m_viewOffset = ImVec2( 0, 0 );
        ImRect              m_windowRect;
        ImRect              m_canvasVisibleRect;
        ImVec2              m_mouseScreenPos = ImVec2( 0, 0 );
        ImVec2              m_mouseCanvasPos = ImVec2( 0, 0 );
    };

    //-------------------------------------------------------------------------
    // Drawing Helpers
    //-------------------------------------------------------------------------

    void DrawCanvasGridAndTitle( DrawingContext& ctx, char const* const pGraphTitle );
    void DrawEmptyGrid();

    //-------------------------------------------------------------------------
    // Node Base
    //-------------------------------------------------------------------------

    class BaseGraph;

    class KRG_TOOLS_CORE_API BaseNode : public IRegisteredType
    {
        friend BaseGraph;

    public:

        KRG_REGISTER_TYPE( BaseNode );

        static BaseNode* CreateNodeFromSerializedData( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& nodeObjectValue, BaseGraph* pParentGraph );

    public:

        virtual ~BaseNode();

        // Node Info
        //-------------------------------------------------------------------------

        // Get the node unique ID
        inline UUID const& GetID() const { return m_ID; }

        // Get node name
        virtual char const* GetDisplayName() const { return GetTypeName(); }

        // Get friendly type name
        virtual char const* GetTypeName() const = 0;

        // Can this node be created and added to a graph by a user? i.e. Does it show up in the context menu
        virtual bool IsUserCreatable() const { return true; }

        // Can this node be destroyed via user input - this is tied to whether the user can create a node of this type
        bool IsDestroyable() const { return IsUserCreatable(); }

        // Node Visuals
        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE Float2 const& GetCanvasPosition() const { return m_canvasPosition; }
        KRG_FORCE_INLINE ImVec2 const& GetSize() const { return m_size; }

        // Get node color
        virtual ImColor GetColor() const { return Settings::s_nodeBackgroundColor; }

        // Request an update of the node and pin sizes
        inline void RecalculateNodeSize() { m_isCalculatingSizes = true; }

        // Get the margin between the node contents and the outer border
        virtual ImVec2 GetNodeMargin() const { return ImVec2( 8, 4 ); }

        // Graphs
        //-------------------------------------------------------------------------

        // Get the graph that we belong to
        inline bool HasParentGraph() const { return m_pParentGraph != nullptr; }
        inline BaseGraph* GetParentGraph() { return m_pParentGraph; }
        inline BaseGraph const* GetParentGraph() const { return m_pParentGraph; }

        // Does this node represent a graph? e.g. a state machine node
        inline bool HasChildGraph() const { return m_pChildGraph != nullptr; }
        inline BaseGraph* GetChildGraph() { return m_pChildGraph; }
        inline BaseGraph const* GetChildGraph() const { return m_pChildGraph; }

        // Does this node have a graph that needs to be displayed in the secondary view? e.g. a condition graph for a selector node
        inline bool HasSecondaryGraph() const { return m_pSecondaryGraph != nullptr; }
        inline BaseGraph* GetSecondaryGraph() { return m_pSecondaryGraph; }
        inline BaseGraph const* GetSecondaryGraph() const { return m_pSecondaryGraph; }

        // Serialization
        //-------------------------------------------------------------------------

        virtual void Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& nodeObjectValue ) = 0;
        virtual void Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const = 0;

    protected:

        REGISTER UUID           m_ID = UUID::GenerateID();
        REGISTER Float2         m_canvasPosition = Float2( 0, 0 ); // Updated each frame
        ImVec2                  m_size = ImVec2( 0, 0 ); // Updated on size calculation
        bool                    m_isCalculatingSizes = true;

        BaseGraph*              m_pChildGraph = nullptr;
        BaseGraph*              m_pSecondaryGraph = nullptr;

    private:

        BaseGraph*              m_pParentGraph = nullptr; // Private so that we can enforce how we add nodes to the graphs
    };

    //-------------------------------------------------------------------------
    // Graph Base
    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API BaseGraph : public IRegisteredType
    {
    public:

        KRG_REGISTER_TYPE( BaseGraph );

        static BaseGraph* CreateGraphFromSerializedData( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue, BaseGraph* pParentGraph  );

    public:

        BaseGraph() = default;
        virtual ~BaseGraph();

        // Graph
        //-------------------------------------------------------------------------

        // Get the display title for this graph
        virtual char const* GetTitle() const { return "Unknown"; }

        // Serialization
        virtual void Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue ) = 0;
        virtual void Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const = 0;

        // Parent graph
        inline bool HasParentGraph() const { return m_pParentGraph != nullptr; }
        inline bool IsRootGraph() const { return !HasParentGraph(); }
        inline BaseGraph* GetParentGraph() { return m_pParentGraph; }
        inline BaseGraph const* GetParentGraph() const { return m_pParentGraph; }

        // Node Operations
        //-------------------------------------------------------------------------

        // Destroys and deletes the specified node
        virtual void DestroyNode( UUID const& nodeID ) = 0;

        // Helpers
        //-------------------------------------------------------------------------

        void FindAllNodesOfType( TypeSystem::TypeID typeID, TInlineVector<BaseNode*, 20>& results, bool includeDerivedNodes = true ) const;

        template<typename T>
        TInlineVector<T*, 20> FindAllNodesOfType( bool includeDerivedNodes = true ) const
        {
            static_assert( std::is_base_of<BaseNode, T>::value );
            TInlineVector<BaseNode*, 20> intermediateResults;
            FindAllNodesOfType( T::GetStaticTypeID(), intermediateResults, includeDerivedNodes );

            // Transfer results to typed array
            TInlineVector<T*, 20> results;
            for ( auto const& pFoundNode : intermediateResults )
            {
                results.emplace_back( SafeCast<T>( pFoundNode ) );
            }
            return results;
        }

    protected:

        // Adds a node to this graph - Note: this transfers ownership of the node memory to this graph!
        // Adding of a node must go through this code path as we need to set the parent node ptr
        void AddNode( BaseNode* pNode )
        {
            KRG_ASSERT( pNode != nullptr );
            KRG_ASSERT( FindNode( pNode->m_ID ) == nullptr );
            pNode->m_pParentGraph = this;
            m_nodes.emplace_back( pNode );
        }

        // Find a specific node in the graph
        inline BaseNode* FindNode( UUID const& nodeID ) const
        {
            for ( auto pNode : m_nodes )
            {
                if ( pNode->GetID() == nodeID )
                {
                    return pNode;
                }
            }

            return nullptr;
        }

    protected:

        REGISTER UUID                           m_ID = UUID::GenerateID();
        TVector<BaseNode*>                      m_nodes;

    private:

        BaseGraph*                              m_pParentGraph = nullptr; // Private so that we can enforce usage
    };
}