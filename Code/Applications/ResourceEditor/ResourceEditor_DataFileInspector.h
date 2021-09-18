#pragma once

#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "Tools/Core/Resource/RawAssets/RawAssetInfo.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Resource/ResourceID.h"

//-------------------------------------------------------------------------

namespace KRG::Resource { struct ResourceDescriptor; }
namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG
{
    class DataFileInspector
    {
        enum class Mode
        {
            None,
            InspectingRawFile,
            InspectingResourceFile,
        };

    public:

        DataFileInspector( TypeSystem::TypeRegistry const& typeRegistry, FileSystem::Path const& sourceDataPath );
        virtual ~DataFileInspector();

        inline FileSystem::Path const& GetInspectedFilePath() const { return m_inspectedFile; }
        void SetFileToInspect( FileSystem::Path const& inFile );
        void ClearFileToInspect();

        void Draw();

    protected:

        // Raw files
        //-------------------------------------------------------------------------

        void OnStartInspectingRawFile();
        void OnStopInspectingRawFile();
        void DrawRawFileInfo();

        bool CreateNewDescriptor( ResourceTypeID resourceTypeID, Resource::ResourceDescriptor const& descriptor ) const;

        // Resource files
        //-------------------------------------------------------------------------

        void DrawResourceFileInfo();
        bool LoadResourceDescriptor();
        bool SaveLoadedResourceDescriptor();

    protected:

        TypeSystem::TypeRegistry const&             m_typeRegistry;
        FileSystem::Path const                      m_sourceDataPath;
        PropertyGrid                                m_propertyGrid;
        FileSystem::Path                            m_inspectedFile;
        Mode                                        m_mode = Mode::None;

        // Resource files
        ResourceID                                  m_descriptorID;
        FileSystem::Path                            m_descriptorPath;
        Resource::ResourceDescriptor*               m_pDescriptor = nullptr;
        bool                                        m_isDirty = false;

        // Raw assets
        RawAssets::RawAssetInfo                     m_assetInfo;
        bool                                        m_validAssetInfo = false;
    };
}