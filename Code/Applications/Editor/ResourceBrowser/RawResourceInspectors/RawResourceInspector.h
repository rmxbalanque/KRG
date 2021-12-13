#pragma once

#include "Tools/Core/Helpers/GlobalRegistryBase.h"
#include "Tools/Core/PropertyGrid/PropertyGrid.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Resource/ResourceTypeID.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem { class TypeRegistry; }

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    struct ResourceDescriptor;
    class ResourceDatabase;

    //-------------------------------------------------------------------------

    class RawResourceInspector
    {
    public:

        struct Model
        {
            FileSystem::Path                    m_sourceResourceDirectory;
            FileSystem::Path                    m_compiledResourceDirectory;
            TypeSystem::TypeRegistry const*     m_pTypeRegistry = nullptr;
        };

    public:

        RawResourceInspector( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDatabase, FileSystem::Path const& filePath );
        virtual ~RawResourceInspector();

        // Get the actual file-path for the file
        inline FileSystem::Path const& GetFilePath() const { return m_filePath; }

        // Draw the inspector UI
        bool DrawDialog();

    protected:

        bool CreateNewDescriptor( ResourceTypeID resourceTypeID, Resource::ResourceDescriptor const* pDescriptor ) const;

        virtual char const* GetInspectorTitle() const = 0;

        // Override this to draw the contents of the file
        virtual void DrawFileInfoAndContents() = 0;

        // Override this to draw the asset creator window, this is where you can create new descriptors
        virtual void DrawResourceDescriptorCreator() = 0;

        RawResourceInspector& operator=( RawResourceInspector const& ) = delete;
        RawResourceInspector( RawResourceInspector const& ) = delete;

    protected:

        TypeSystem::TypeRegistry const&     m_typeRegistry;
        FileSystem::Path                    m_rawResourceDirectory;
        FileSystem::Path                    m_filePath;
        ResourceDescriptor*                 m_pDescriptor = nullptr;
        PropertyGrid                        m_propertyGrid;
    };

    //-------------------------------------------------------------------------
    // Resource Workspace Factory
    //-------------------------------------------------------------------------
    // Used to spawn the appropriate factory

    class RawResourceInspectorFactory : public TGlobalRegistryBase<RawResourceInspectorFactory>
    {
        KRG_DECLARE_GLOBAL_REGISTRY( RawResourceInspectorFactory );

    public:

        static bool CanCreateInspector( FileSystem::Path const& filePath );
        static RawResourceInspector* TryCreateInspector( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDatabase, FileSystem::Path const& filePath );

    protected:

        // Get the resource type this factory supports
        virtual bool IsSupportedFile( FileSystem::Path const& filePath ) const = 0;

        // Virtual method that will create a workspace if the resource ID matches the appropriate types
        virtual RawResourceInspector* CreateInspector( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDatabase, FileSystem::Path const& filePath ) const = 0;
    };
}

//-------------------------------------------------------------------------
//  Macro to create a raw file inspector factory
//-------------------------------------------------------------------------
// Use in a CPP to define a factory e.g., KRG_RESOURCE_RAW_FILE_INSPECTOR_FACTORY( FBXInspector, fbx, FBXResourceInspector );
// Note: extension needs to be lowercase

#define KRG_RAW_FILE_INSPECTOR_FACTORY( FactoryName, RawFileExtension, EditorClass )\
class FactoryName final : public RawResourceInspectorFactory\
{\
    virtual bool IsSupportedFile( FileSystem::Path const& filePath) const override { return filePath.GetLowercaseExtensionAsString() == RawFileExtension; }\
    virtual RawResourceInspector* CreateInspector( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDatabase, FileSystem::Path const& filePath ) const override\
    {\
        KRG_ASSERT( IsSupportedFile( filePath ) );\
        return KRG::New<EditorClass>( typeRegistry, resourceDatabase, filePath );\
    }\
};\
static FactoryName g_##FactoryName;
