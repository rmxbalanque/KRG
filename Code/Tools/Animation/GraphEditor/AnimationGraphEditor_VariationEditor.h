#pragma once
#include "System/Core/Types/StringID.h"
#include "Tools/Core/Resource/ResourceFilePicker.h"

//-------------------------------------------------------------------------

struct ImGuiWindowClass;
namespace KRG { class UpdateContext; }
namespace KRG::Resource { class ResourceDatabase; }

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class VariationHierarchy;
    class EditorGraphDefinition;

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

        GraphVariationEditor( Resource::ResourceDatabase const* pResourceDatabase, EditorGraphDefinition* pGraphDefinition );

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

        EditorGraphDefinition*              m_pGraphDefinition = nullptr;
        StringID                            m_activeOperationVariationID;
        char                                m_buffer[255] = {0};
        Resource::ResourceFilePicker        m_resourcePicker;
        OperationType                       m_activeOperation = OperationType::None;
    };
}