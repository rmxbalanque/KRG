#pragma once
#include "AnimationGraphEditor_Model.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class ViewportManager; }

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphVariationEditor final
    {
        enum class OperationType
        {
            None,
            Create,
            Rename,
            Delete
        };

    public:

        GraphVariationEditor( GraphEditorModel& model ) : m_model( model ) {}

        void Draw( UpdateContext const& context, Render::ViewportManager& viewportManager );

    private:

        void DrawVariationTree();
        void DrawVariationTreeNode( VariationHierarchy& variationHierarchy, StringID variationID );
        void DrawOverridesTable();

        void StartCreate( StringID variationID );
        void StartRename( StringID variationID );
        void StartDelete( StringID variationID );
        void DrawActiveOperationUI();

    private:

        GraphEditorModel&           m_model;
        StringID                    m_activeOperationVariationID;
        char                        m_buffer[255];
        OperationType               m_activeOperation;
    };
}