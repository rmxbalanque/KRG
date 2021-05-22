#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        bool EngineModule::Initialize( ModuleContext& context )
        {
            auto const pTypeRegistry = context.GetSystem<TypeSystem::TypeRegistry>();
            m_animationClipLoader.SetTypeRegistry( pTypeRegistry );
            m_graphLoader.SetTypeRegistry( pTypeRegistry );

            context.RegisterResourceLoader( &m_skeletonLoader );
            context.RegisterResourceLoader( &m_animationClipLoader );
            context.RegisterResourceLoader( &m_graphLoader );
            context.RegisterResourceLoader( &m_graphDataSetLoader );

            m_initialized = true;
            return m_initialized;
        }

        void EngineModule::Shutdown( ModuleContext& context )
        {
            if( m_initialized )
            {
                context.UnregisterResourceLoader( &m_graphDataSetLoader );
                context.UnregisterResourceLoader( &m_graphLoader );
                context.UnregisterResourceLoader( &m_animationClipLoader );
                context.UnregisterResourceLoader( &m_skeletonLoader );
            }

            m_initialized = false;
        }
    }
}