#pragma once

#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "Tools/Resource/DataBrowser/DataBrowserModel.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Editor Model
    //-------------------------------------------------------------------------
    // Base class for an editor model, this helps to manage global editor state across multiple tools
    // Each editor will create its own model and supply it to its various tools

    class EditorModel
    {

    public:

        virtual ~EditorModel();

        virtual void Initialize( UpdateContext const& context );
        virtual void Shutdown( UpdateContext const& context );
        virtual void Update( UpdateContext const& context );

        inline FileSystem::Path const& GetSourceDataDirectory() const { return m_sourceDataDirectory; }
        inline FileSystem::Path const& GetCompiledDataDirectory() const { return m_compiledDataDirectory; }

        inline TypeSystem::TypeRegistry const& GetTypeRegistry() const { return *m_pTypeRegistry; }
        inline DataBrowserModel& GetDataBrowser() { return *m_pDataBrowserModel; }

    protected:

        FileSystem::Path                    m_sourceDataDirectory;
        FileSystem::Path                    m_compiledDataDirectory;

        // Core editor systems
        TypeSystem::TypeRegistry const*     m_pTypeRegistry = nullptr;

        // Optional: Can be created in the initialize of derived models if required
        DataBrowserModel*                   m_pDataBrowserModel = nullptr;
    };
}