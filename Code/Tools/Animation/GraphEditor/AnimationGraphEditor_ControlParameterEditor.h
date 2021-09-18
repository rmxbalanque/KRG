#pragma once
#include "AnimationGraphEditor_Model.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class ViewportManager; }

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphControlParameterEditor
    {
        enum class OperationType
        {
            None,
            Rename,
            Delete
        };

    public:

        GraphControlParameterEditor( GraphEditorModel& model );

        void Draw( UpdateContext const& context, Render::ViewportManager& viewportManager );

    private:

        void DrawAddParameterUI();

        void StartRename( UUID parameterID );
        void StartDelete( UUID parameterID );
        void DrawActiveOperationUI();

    private:

        GraphEditorModel&           m_model;
        UUID                        m_currentOperationParameterID;
        char                        m_buffer[255];
        OperationType               m_activeOperation;
    };
}