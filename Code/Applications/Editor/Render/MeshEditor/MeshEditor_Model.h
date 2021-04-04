#pragma once

#include "Tools/Resource/DataBrowser/DataBrowserModel.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Update/UpdateContext.h"
#include "Applications/Editor/EditorSettings.h"

//-------------------------------------------------------------------------

namespace KRG::Render::MeshEditor
{
    class Model
    {

    public:

        Model()
            : m_dataBrowserModel( EditorSettings().m_sourceDataDirectory )
        {}

        inline void Initialize( UpdateContext const& context )
        {
            static char const* const allowedExtensions[] = { ".msh", ".smsh", ".fbx", ".gltf", 0 };
            m_dataBrowserModel.Initialize( allowedExtensions );

            m_pTypeRegistry = context.GetSystem<TypeSystem::TypeRegistry>();
            KRG_ASSERT( m_pTypeRegistry != nullptr );
        }

        inline void Shutdown()
        {
            m_dataBrowserModel.Shutdown();
        }

        inline void Update( UpdateContext const& context )
        {
            m_dataBrowserModel.Update( context );
        }

        //-------------------------------------------------------------------------

        inline DataBrowserModel& GetDataBrowser() { return m_dataBrowserModel; }
        inline TypeSystem::TypeRegistry const& GetTypeRegistry() const { return *m_pTypeRegistry; }

    private:

        TypeSystem::TypeRegistry const*     m_pTypeRegistry = nullptr;
        DataBrowserModel                    m_dataBrowserModel;
    };
}