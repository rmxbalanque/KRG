#include "EditorSettings.h"
#include "System/Core/Settings/ConfigSettings.h"

//-------------------------------------------------------------------------

namespace KRG
{
    static ConfigSettingString  g_sourceDataPath( "SourceDataPath", "Paths" );
    static ConfigSettingString  g_compiledDataPath( "CompiledDataPath", "Paths" );

    //-------------------------------------------------------------------------

    EditorSettings::EditorSettings()
        : m_sourceDataDirectory( g_sourceDataPath )
        , m_compiledDataDirectory( g_compiledDataPath )
    {}
}