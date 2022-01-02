#include "Animation_EditorGraph_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    EditorGraphCompilationContext::EditorGraphCompilationContext()
        : GraphDefinition()
    {
        m_instanceNodeStartOffsets.emplace_back( 0 );
    }

    EditorGraphCompilationContext::~EditorGraphCompilationContext()
    {
        for ( auto pSettings : m_nodeSettings )
        {
            KRG::Delete( pSettings );
        }
    }

    void EditorGraphCompilationContext::SerializeSettings( cereal::BinaryOutputArchive& settingsArchive )
    {
        for ( auto pSettings : m_nodeSettings )
        {
            pSettings->Save( settingsArchive );
        }
    }
}