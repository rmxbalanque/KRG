#include "AnimationGraphTools_Compilation.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    ToolsGraphCompilationContext::~ToolsGraphCompilationContext()
    {
        for ( auto pSettings : m_nodeSettings )
        {
            KRG::Delete( pSettings );
        }
    }

    void ToolsGraphCompilationContext::SerializeSettings( cereal::BinaryOutputArchive& settingsArchive )
    {
        for ( auto pSettings : m_nodeSettings )
        {
            pSettings->Save( settingsArchive );
        }
    }
}