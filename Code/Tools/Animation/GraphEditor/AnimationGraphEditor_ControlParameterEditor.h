#pragma once
#include "System/Core/Types/UUID.h"

struct ImGuiWindowClass;
namespace KRG { class UpdateContext; }

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class AnimationGraphToolsDefinition;
    struct DebugContext;

    //-------------------------------------------------------------------------

    class GraphControlParameterEditor
    {
        enum class OperationType
        {
            None,
            Rename,
            Delete
        };

    public:

        GraphControlParameterEditor( AnimationGraphToolsDefinition* pGraphDefinition );

        void UpdateAndDraw( UpdateContext const& context, DebugContext* pDebugContext, ImGuiWindowClass* pWindowClass, char const* pWindowName );

    private:

        void DrawAddParameterUI();

        void StartRename( UUID const& parameterID );
        void StartDelete( UUID const& parameterID );
        void DrawActiveOperationUI();

    private:

        AnimationGraphToolsDefinition*      m_pGraphDefinition = nullptr;
        UUID                                m_currentOperationParameterID;
        char                                m_buffer[255];
        OperationType                       m_activeOperation;
    };
}