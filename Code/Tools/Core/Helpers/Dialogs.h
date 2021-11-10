#pragma once
#include "Tools/Core/_Module/API.h"
#include "System/Resource/ResourceTypeID.h"
#include "System/Core/FileSystem/FileSystemPath.h"

//-------------------------------------------------------------------------

namespace KRG
{
    KRG_TOOLS_CORE_API FileSystem::Path SaveDialog( String const& extension, FileSystem::Path const& startingPath = FileSystem::Path(), String const& friendlyFilterName = "" );
    KRG_TOOLS_CORE_API FileSystem::Path SaveDialog( ResourceTypeID resourceTypeID, FileSystem::Path const& startingPath = FileSystem::Path(), String const& friendlyFilterName = "" );
}