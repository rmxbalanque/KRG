#pragma once
#include "System/Core/Types/StringID.h"
#include "Tools/Core/Resource/ResourceFilePicker.h"

//-------------------------------------------------------------------------

struct ImGuiWindowClass;
namespace KRG { struct EditorContext; class UpdateContext; }

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class VariationHierarchy;
    class AnimationGraphEditorDefinition;

    //-------------------------------------------------------------------------

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

        GraphVariationEditor( EditorContext const& editorContext, AnimationGraphEditorDefinition* pGraphDefinition );

        void UpdateAndDraw( UpdateContext const& context, ImGuiWindowClass* pWindowClass, char const* pWindowName );

    private:

        void DrawVariationTree();
        void DrawVariationTreeNode( VariationHierarchy& variationHierarchy, StringID variationID );
        void DrawOverridesTable();

        void StartCreate( StringID variationID );
        void StartRename( StringID variationID );
        void StartDelete( StringID variationID );
        void DrawActiveOperationUI();

    private:

        EditorContext const&                m_editorContext;
        AnimationGraphEditorDefinition*      m_pGraphDefinition = nullptr;
        StringID                            m_activeOperationVariationID;
        char                                m_buffer[255];
        Resource::ResourceFilePicker        m_resourcePicker;
        OperationType                       m_activeOperation;
    };
}