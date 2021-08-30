#pragma once
#include "Tools/Core/Editor/EditorTool.h"
#include "AnimationGraphEditor_Model.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GraphControlParameterEditor final : public TEditorTool<GraphEditorModel>
    {
        enum class OperationType
        {
            None,
            Rename,
            Delete
        };

    public:

        GraphControlParameterEditor( EditorModel* pModel );

        virtual char const* const GetName() { return "Control Parameters"; }
        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        void DrawAddParameterUI();

        void StartRename( UUID parameterID );
        void StartDelete( UUID parameterID );
        void DrawActiveOperationUI();

    private:

        UUID                m_currentOperationParameterID;
        char                m_buffer[255];
        OperationType       m_activeOperation;
    };
}