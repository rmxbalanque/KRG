#pragma once

#include "Tools/Core/Helpers/GlobalRegistryBase.h"
#include "Tools/Core/Editors/EditorModel.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource { struct ResourceDescriptor; }

    //-------------------------------------------------------------------------

    class RawFileInspector
    {

    public:

        RawFileInspector( EditorModel* pModel, FileSystem::Path const& filePath );
        virtual ~RawFileInspector() = default;

        // Get the actual file-path for the file
        inline FileSystem::Path const& GetFilePath() const { return m_filePath; }

        // Draw the inspector UI
        virtual void Draw() = 0;

    protected:

        bool CreateNewDescriptor( ResourceTypeID resourceTypeID, Resource::ResourceDescriptor const& descriptor ) const;

        RawFileInspector& operator=( RawFileInspector const& ) = delete;
        RawFileInspector( RawFileInspector const& ) = delete;

    protected:

        FileSystem::Path                    m_filePath;
        EditorModel*                        m_pModel = nullptr;
    };

    //-------------------------------------------------------------------------
    // Resource Workspace Factory
    //-------------------------------------------------------------------------
    // Used to spawn the appropriate factory

    class RawFileInspectorFactory : public TGlobalRegistryBase<RawFileInspectorFactory>
    {
        KRG_DECLARE_GLOBAL_REGISTRY( RawFileInspectorFactory );

    public:

        static bool CanCreateInspector( FileSystem::Path const& filePath );
        static RawFileInspector* TryCreateInspector( EditorModel* pModel, FileSystem::Path const& filePath );

    protected:

        // Get the resource type this factory supports
        virtual bool IsSupportedFile( FileSystem::Path const& filePath ) const = 0;

        // Virtual method that will create a workspace if the resource ID matches the appropriate types
        virtual RawFileInspector* CreateInspector( EditorModel* pModel, FileSystem::Path const& filePath ) const = 0;
    };
}

//-------------------------------------------------------------------------
//  Macro to create a raw file inspector factory
//-------------------------------------------------------------------------
// Use in a CPP to define a factory e.g., KRG_RESOURCE_RAW_FILE_INSPECTOR_FACTORY( FBXInspector, fbx, FBXFileInspector );
// Note: extension needs to be lowercase

#define KRG_RAW_FILE_INSPECTOR_FACTORY( FactoryName, RawFileExtension, EditorClass )\
class FactoryName final : public RawFileInspectorFactory\
{\
    virtual bool IsSupportedFile( FileSystem::Path const& filePath) const override { return filePath.GetLowercaseExtensionAsString() == RawFileExtension; }\
    virtual RawFileInspector* CreateInspector( EditorModel* pModel, FileSystem::Path const& filePath ) const override\
    {\
        KRG_ASSERT( IsSupportedFile( filePath ) );\
        return KRG::New<EditorClass>( pModel, filePath );\
    }\
};\
static FactoryName g_##FactoryName;
