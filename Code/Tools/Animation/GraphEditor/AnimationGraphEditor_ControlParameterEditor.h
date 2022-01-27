#pragma once
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

struct ImGuiWindowClass;
namespace KRG { class UpdateContext; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class EditorGraphDefinition;
    struct DebugContext;

    namespace GraphNodes 
    {
        class ControlParameterEditorNode;
        class VirtualParameterEditorNode;
    }

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

        struct ParameterPreviewState
        {
            ParameterPreviewState( GraphNodes::ControlParameterEditorNode* pParameter ) : m_pParameter( pParameter ) { KRG_ASSERT( m_pParameter != nullptr ); }
            virtual ~ParameterPreviewState() = default;

            virtual void DrawPreviewEditor( DebugContext* pDebugContext ) = 0;

        public:

            GraphNodes::ControlParameterEditorNode* m_pParameter = nullptr;
        };

    public:

        GraphControlParameterEditor( EditorGraphDefinition* pGraphDefinition );
        ~GraphControlParameterEditor();

        // Draw the control parameter editor, returns true if there is a request the calling code needs to fulfill i.e. navigation
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

        void CreatePreviewStates();
        void DestroyPreviewStates();

    private:

        EditorGraphDefinition*                          m_pGraphDefinition = nullptr;
        GraphNodes::VirtualParameterEditorNode*         m_pVirtualParamaterToEdit = nullptr;
        UUID                                            m_currentOperationParameterID;
        OperationType                                   m_activeOperation;
        char                                            m_parameterNameBuffer[255];
        char                                            m_parameterCategoryBuffer[255];
        TVector<ParameterPreviewState*>                 m_parameterPreviewStates;
    };
}