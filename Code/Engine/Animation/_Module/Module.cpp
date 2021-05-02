#include "Module.h"
#include "Engine/Core/Modules/EngineModuleContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Animation
    {
        bool EngineModule::Initialize( ModuleContext& context )
        {
            m_animationLoader.SetTypeRegistry( context.GetSystem<TypeSystem::TypeRegistry>() );

            context.RegisterResourceLoader( &m_skeletonLoader );
            context.RegisterResourceLoader( &m_animationLoader );

            m_initialized = true;
            return m_initialized;
        }

        void EngineModule::Shutdown( ModuleContext& context )
        {
            if( m_initialized )
            {
                context.UnregisterResourceLoader( &m_animationLoader );
                context.UnregisterResourceLoader( &m_skeletonLoader );
            }

            m_initialized = false;
        }
    }
}