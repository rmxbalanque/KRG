#pragma once
#include "System/Core/FileSystem/DataPath.h"
#include "System/Resource/ResourceTypeID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    struct DataFilePicker
    {
        // Pick any file in the data folder
        static bool PickFile( FileSystem::Path const& sourceDataPath, DataPath& outPath );

        // Pick only resources of the specific type from the data folder
        static bool PickResourceFile( FileSystem::Path const& sourceDataPath, ResourceTypeID allowedResourceType, DataPath& outPath );

        // Pick only registered resources from the data folder
        static bool PickResourceFile( FileSystem::Path const& sourceDataPath, TVector<ResourceTypeID> const& allowedResourceTypes, DataPath& outPath );
    };
}