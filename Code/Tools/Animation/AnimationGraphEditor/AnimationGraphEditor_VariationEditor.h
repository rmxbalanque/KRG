#pragma once
#include "Tools/Core/Editor/EditorTool.h"
#include "AnimationGraphEditor_Model.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphVariationEditor final : public TEditorTool<GraphEditorModel>
    {
        enum class OperationType
        {
            None,
            Create,
            Rename,
            Delete
        };

    public:

        GraphVariationEditor( EditorModel* pModel );

        virtual char const* const GetName() { return "Data Set"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        void DrawVariationTree();
        void DrawVariationTreeNode( VariationHierarchy& variationHierarchy, StringID variationID );
        void DrawOverridesTable();

        void StartCreate( StringID variationID );
        void StartRename( StringID variationID );
        void StartDelete( StringID variationID );
        void DrawActiveOperationUI();

    private:

        StringID            m_activeOperationVariationID;
        char                m_buffer[255];
        OperationType       m_activeOperation;
    };
}