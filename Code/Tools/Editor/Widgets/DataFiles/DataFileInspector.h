#pragma once

#include "Tools/Editor/Widgets/PropertyGrid/PropertyGrid.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Resource/ResourceID.h"

//-------------------------------------------------------------------------

namespace KRG { class EditorModel; }
namespace KRG::Resource { struct ResourceDescriptor; }

//-------------------------------------------------------------------------

namespace KRG::Editor
{
    class KRG_TOOLS_EDITOR_API DataFileInspector
    {
        enum class Mode
        {
            None,
            InspectingRawFile,
            InspectingResourceFile,
        };

    public:

        DataFileInspector( EditorModel* pModel );
        virtual ~DataFileInspector();

        inline FileSystem::Path const& GetInspectedFilePath() const { return m_inspectedFile; }
        void SetFileToInspect( FileSystem::Path const& inFile );
        void ClearFileToInspect();

        void Draw();

    protected:

        // Raw files
        //-------------------------------------------------------------------------

        virtual void OnStartInspectingRawFile() {}
        virtual void OnStopInspectingRawFile() {}
        virtual void DrawRawFileInfo() {}

        bool CreateNewDescriptor( ResourceTypeID resourceTypeID, Resource::ResourceDescriptor const& descriptor ) const;

        // Resource files
        //-------------------------------------------------------------------------

        void DrawResourceFileInfo();
        bool LoadResourceDescriptor();
        bool SaveLoadedResourceDescriptor();

    protected:

        EditorModel*                                m_pModel = nullptr;
        PropertyGrid                                m_propertyGrid;
        FileSystem::Path                            m_inspectedFile;
        Mode                                        m_mode = Mode::None;

        // Resource files
        ResourceID                                  m_descriptorID;
        FileSystem::Path                            m_descriptorPath;
        Resource::ResourceDescriptor*               m_pDescriptor = nullptr;
        bool                                        m_isDirty = false;
    };
}