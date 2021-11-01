#pragma once

#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Resource/ResourceID.h"

namespace KRG {}

//-------------------------------------------------------------------------

namespace KRG
{
    class ResourceEditorModel;
    class RawFileInspector;
    namespace Resource { struct ResourceDescriptor; }

    //-------------------------------------------------------------------------

    class DataFileInspector
    {
        enum class Mode
        {
            None,
            InspectingRawFile,
            InspectingResourceFile,
        };

    public:

        DataFileInspector( ResourceEditorModel* pModel );
        virtual ~DataFileInspector();

        inline FileSystem::Path const& GetInspectedFilePath() const { return m_inspectedFile; }
        void SetFileToInspect( FileSystem::Path const& inFile );
        void ClearFileToInspect();

        void Draw();

    protected:

        void DrawResourceFileInfo();
        bool LoadResourceDescriptor();
        bool SaveLoadedResourceDescriptor();

    protected:

        ResourceEditorModel*                        m_pModel = nullptr;
        PropertyGrid                                m_propertyGrid;
        FileSystem::Path                            m_inspectedFile;
        Mode                                        m_mode = Mode::None;

        // Resource files
        ResourceID                                  m_descriptorID;
        FileSystem::Path                            m_descriptorPath;
        Resource::ResourceDescriptor*               m_pDescriptor = nullptr;
        bool                                        m_isDirty = false;

        // Raw files
        RawFileInspector*                           m_pRawFileInspector = nullptr;
    };
}