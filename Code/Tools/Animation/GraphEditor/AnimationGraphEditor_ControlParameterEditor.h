#pragma once
#include "System/Core/Types/UUID.h"

struct ImGuiWindowClass;
namespace KRG { class UpdateContext; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class EditorGraphDefinition;
    struct DebugContext;
    namespace GraphNodes { class VirtualParameterEditorNode; }

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

        GraphControlParameterEditor( EditorGraphDefinition* pGraphDefinition );

        // Draw the control parameter editor, returns true if there is a request the calling code needs to fulfil i.e. navigation
        bool UpdateAndDraw( UpdateContext const& context, DebugContext* pDebugContext, ImGuiWindowClass* pWindowClass, char const* pWindowName );

        // Get the virtual parameter graph we want to show
        GraphNodes::VirtualParameterEditorNode* GetVirtualParameterToEdit() { return m_pVirtualParamaterToEdit; }

    private:

        void DrawParameterList();
        void DrawParameterPreviewControls( DebugContext* pDebugContext );
        void DrawAddParameterCombo();

        void StartParameterRename( UUID const& parameterID );
        void StartParameterDelete( UUID const& parameterID );
        void DrawDialogs();

    private:

        EditorGraphDefinition*                          m_pGraphDefinition = nullptr;
        GraphNodes::VirtualParameterEditorNode*         m_pVirtualParamaterToEdit = nullptr;
        UUID                                            m_currentOperationParameterID;
        char                                            m_parameterNameBuffer[255];
        char                                            m_parameterCategoryBuffer[255];
        TVector<TArray<char, 255>>                      m_parameterPreviewBuffers;
        OperationType                                   m_activeOperation;
    };
}