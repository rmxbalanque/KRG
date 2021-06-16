#pragma once

#include "Tools/Core/_Module/API.h"
#include "GraphEditorDrawingContext.h"
#include "Tools/Core/ThirdParty/KRG_RapidJson.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Core/Core/IntegralTypes.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    class BaseGraph;

    //-------------------------------------------------------------------------
    // Node Base
    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API BaseNode : public IRegisteredType
    {
        friend BaseGraph;

    public:

        KRG_REGISTER_TYPE( BaseNode );

        static BaseNode* CreateNodeFromSerializedData( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& nodeObjectValue, BaseGraph* pParentGraph );

    public:

        BaseNode() = default;
        virtual ~BaseNode();

        // Lifetime
        //-------------------------------------------------------------------------

        // Called whenever we created a node for the first time (not called via serialization)
        virtual void Initialize( BaseGraph* pParentGraph );

        // Called just before we destroy a node
        virtual void Shutdown();

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

        // Can this node be destroyed via user input - this is generally tied to whether the user can create a node of this type
        virtual bool IsDestroyable() const { return IsUserCreatable(); }

        // Returns the string path from the root graph
        virtual String GetPathFromRoot() const;

        // Node Visuals
        //-------------------------------------------------------------------------

        KRG_FORCE_INLINE Float2 const& GetCanvasPosition() const { return m_canvasPosition; }
        KRG_FORCE_INLINE ImVec2 const& GetSize() const { return m_size; }

        inline void SetCanvasPosition( ImVec2 const& newPosition ) { m_canvasPosition = newPosition; }

        // Get node highlight color
        virtual ImColor GetHighlightColor() const { return VisualSettings::s_genericNodeHighlightColor; }

        // Request an update of the node and pin sizes
        inline void RefreshVisualState() { m_isCalculatingSizes = true; }

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
        inline BaseGraph const* GetChildGraph() const { return m_pChildGraph; }
        inline BaseGraph* GetChildGraph() { return m_pChildGraph; }

        // Does this node have a graph that needs to be displayed in the secondary view? e.g. a condition graph for a selector node
        inline bool HasSecondaryGraph() const { return m_pSecondaryGraph != nullptr; }
        inline BaseGraph* GetSecondaryGraph() { return m_pSecondaryGraph; }
        inline BaseGraph const* GetSecondaryGraph() const { return m_pSecondaryGraph; }

        // Serialization
        //-------------------------------------------------------------------------

        void Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& nodeObjectValue );
        void Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const;

    protected:

        // Override this if you want to add extra controls to this node (the derived nodes will determine where this content is placed)
        virtual void DrawExtraControls( DrawingContext const& ctx ) {}

        // Set and initialize the secondary graph
        void SetSecondaryGraph( BaseGraph* pGraph );

        // Set and initialize the child graph
        void SetChildGraph( BaseGraph* pGraph );

        // Allow for custom serialization in derived types
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue ) {};
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const {};

        // Requests that the parent graph destroys this node
        void Destroy();

    protected:

        REGISTER UUID           m_ID;
        REGISTER Float2         m_canvasPosition = Float2( 0, 0 ); // Updated each frame
        ImVec2                  m_size = ImVec2( 0, 0 ); // Updated on size calculation
        ImVec2                  m_titleRectSize = ImVec2( 0, 0 );
        ImVec2                  m_controlsRectSize = ImVec2( 0, 0 );
        bool                    m_isCalculatingSizes = true;
        bool                    m_isHovered = false;

    private:

        BaseGraph*              m_pParentGraph = nullptr; // Private so that we can enforce how we add nodes to the graphs
        BaseGraph*              m_pChildGraph = nullptr;
        BaseGraph*              m_pSecondaryGraph = nullptr;
    };

    //-------------------------------------------------------------------------
    // Graph Base
    //-------------------------------------------------------------------------

    enum class SearchMode { Localized, Recursive };
    enum class SearchTypeMatch { Exact, Derived };

    class KRG_TOOLS_CORE_API BaseGraph : public IRegisteredType
    {
        friend class BaseGraphView;

    public:

        KRG_REGISTER_TYPE( BaseGraph );

        static BaseGraph* CreateGraphFromSerializedData( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue, BaseNode* pParentNode  );

    public:

        BaseGraph() = default;
        virtual ~BaseGraph();

        // Lifetime
        //-------------------------------------------------------------------------

        // Called whenever we created a graph for the first time (not called via serialization)
        virtual void Initialize( BaseNode* pParentNode );

        // Called just before we destroy a graph
        virtual void Shutdown();

        // Graph
        //-------------------------------------------------------------------------

        // Get the display title for this graph
        virtual char const* GetTitle() const { return HasParentNode() ? m_pParentNode->GetDisplayName() : "Root Graph"; }

        // Serialization
        void Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue );
        void Serialize( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const;

        // Parent Node
        inline bool HasParentNode() const { return m_pParentNode != nullptr; }
        inline bool IsRootGraph() const { return !HasParentNode(); }
        inline BaseNode* GetParentNode() { return m_pParentNode; }
        inline BaseNode const* GetParentNode() const { return m_pParentNode; }

        // Node Operations
        //-------------------------------------------------------------------------

        // Destroys and deletes the specified node (UUID copy is intentional)
        void DestroyNode( UUID nodeID );

        // Override this for additional control of what nodes can be placed
        virtual bool CanCreateNode( TypeSystem::TypeInfo const* pNodeTypeInfo ) const { return true; }

        // Override this for additional control of what nodes can be deleted
        virtual bool CanDeleteNode( BaseNode const* pNode )const { return true; }

        // Helpers
        //-------------------------------------------------------------------------

        // Find all nodes of a specific type in this graph. Note: doesnt clear the results array so ensure you feed in an empty array
        void FindAllNodesOfType( TypeSystem::TypeID typeID, TInlineVector<BaseNode*, 20>& results, SearchMode mode = SearchMode::Localized, SearchTypeMatch typeMatch = SearchTypeMatch::Exact ) const;

        template<typename T>
        TInlineVector<T*, 20> FindAllNodesOfType( SearchMode depth = SearchMode::Localized, SearchTypeMatch typeMatch = SearchTypeMatch::Exact )
        {
            static_assert( std::is_base_of<BaseNode, T>::value );
            TInlineVector<BaseNode*, 20> intermediateResults;
            FindAllNodesOfType( T::GetStaticTypeID(), intermediateResults, depth, typeMatch );

            // Transfer results to typed array
            TInlineVector<T*, 20> results;
            for ( auto const& pFoundNode : intermediateResults )
            {
                results.emplace_back( Cast<T>( pFoundNode ) );
            }
            return results;
        }

        template<typename T>
        TInlineVector<T const*, 20> FindAllNodesOfType( SearchMode depth = SearchMode::Localized, SearchTypeMatch typeMatch = SearchTypeMatch::Exact ) const
        {
            static_assert( std::is_base_of<BaseNode, T>::value );
            TInlineVector<BaseNode*, 20> intermediateResults;
            FindAllNodesOfType( T::GetStaticTypeID(), intermediateResults, depth, typeMatch );

            // Transfer results to typed array
            TInlineVector<T const*, 20> results;
            for ( auto const& pFoundNode : intermediateResults )
            {
                results.emplace_back( Cast<T>( pFoundNode ) );
            }
            return results;
        }

    protected:

        // Adds a node to this graph - Note: this transfers ownership of the node memory to this graph!
        // Adding of a node must go through this code path as we need to set the parent node ptr
        void AddNode( BaseNode* pNode )
        {
            KRG_ASSERT( pNode != nullptr && pNode->m_ID.IsValid() );
            KRG_ASSERT( FindNode( pNode->m_ID ) == nullptr );
            KRG_ASSERT( pNode->m_pParentGraph == this );
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

        // Called just before we destroy a node, allows derived graphs to handle the event
        virtual void PreDestroyNode( BaseNode* pNodeAboutToBeDestroyed ) {};

        // Called after we destroy a node, allows derived graphs to handle the event.
        virtual void PostDestroyNode( UUID const& nodeID ) {};

        // Allow for custom serialization in derived types
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue ) {}
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const {}

    protected:

        REGISTER UUID                           m_ID;
        TVector<BaseNode*>                      m_nodes;

    private:

        BaseNode*                               m_pParentNode = nullptr; // Private so that we can enforce usage
    };
}