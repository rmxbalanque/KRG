#pragma once
#include "AnimationGraphTools_Graph.h"
#include "Nodes/AnimationToolsNode_Parameters.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ToolsGraphCompilationContext;

    //-------------------------------------------------------------------------

    class ToolsAnimationGraph
    {

    public:

        // Graph will be initialize in an invalid state, you need to either create a new graph or load an existing graph
        ToolsAnimationGraph() = default;
        ~ToolsAnimationGraph();

        bool IsValid() const { return m_pRootGraph != nullptr; }

        bool IsDirty() const { return false; }

        // Creates a new empty graph
        void CreateNew();

        // Load an existing graph
        bool Load( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue );

        // Saves this graph
        void Save( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonWriter& writer ) const;

        // Graph
        //-------------------------------------------------------------------------

        inline ToolsGraph* GetRootGraph() { return m_pRootGraph; }
        inline ToolsGraph const* GetRootGraph() const { return m_pRootGraph; }

        template<typename T>
        TInlineVector<T*, 20> FindAllNodesOfType( bool includeDerivedNodes = true )
        {
            static_assert( std::is_base_of<ToolsNode, T>::value );
            return m_pRootGraph->FindAllNodesOfType<T>( includeDerivedNodes );
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

        // Compilation
        //-------------------------------------------------------------------------

        bool Compile( ToolsGraphCompilationContext& context ) const;

    private:

        void EnsureUniqueParameterName( String& parameterName ) const;

        // Frees all memory and resets the internal state
        void ResetInternalState();

    private:

        ToolsGraph*                                 m_pRootGraph = nullptr;
        TVector<ControlParameterToolsNode*>         m_controlParameters;
        TVector<VirtualParameterToolsNode*>         m_virtualParameters;
    };
}