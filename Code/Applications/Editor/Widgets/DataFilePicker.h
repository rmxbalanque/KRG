#pragma once
#include "System/Core/FileSystem/DataPath.h"
#include "System/Resource/ResourceTypeID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    struct DataFilePicker
    {
        // Pick any file in the data folder
        static DataPath PickFile( FileSystem::Path const& sourceDataPath );

        // Pick only resources of the specific type from the data folder
        static DataPath PickResourceFile( FileSystem::Path const& sourceDataPath, ResourceTypeID allowedResourceType );

        // Pick only registered resources from the data folder
        static DataPath PickResourceFile( FileSystem::Path const& sourceDataPath, TVector<ResourceTypeID> const& allowedResourceTypes );
    };
}