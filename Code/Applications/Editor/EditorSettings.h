#pragma once
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG
{
    struct EditorSettings
    {
        EditorSettings();

        //-------------------------------------------------------------------------

        FileSystem::Path m_sourceDataDirectory;
        FileSystem::Path m_compiledDataDirectory;
    };
}