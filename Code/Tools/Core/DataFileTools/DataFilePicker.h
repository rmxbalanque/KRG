#pragma once
#include "Tools/Core/_Module/API.h"
#include "System/Resource/ResourceID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    struct KRG_TOOLS_CORE_API DataFilePicker
    {
        // Draw an imgui picker control, returns true if the type was edited
        static bool DrawPickerControl( FileSystem::Path const& sourceDataPath, ResourceTypeID allowedResourceTypeID, ResourceID* pResourceID );

        // Pick any file in the data folder
        static bool PickFile( FileSystem::Path const& sourceDataPath, DataPath& outPath );

        // Pick only resources of the specific type from the data folder
        static bool PickResourceFile( FileSystem::Path const& sourceDataPath, ResourceTypeID allowedResourceType, DataPath& outPath );

        // Pick only registered resources from the data folder
        static bool PickResourceFile( FileSystem::Path const& sourceDataPath, TVector<ResourceTypeID> const& allowedResourceTypes, DataPath& outPath );
    };
}