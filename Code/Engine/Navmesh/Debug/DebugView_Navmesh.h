#pragma once

#include "../_Module/API.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Core/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Navmesh
{
    class NavmeshSystem;
    class NavmeshWorldSystem;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_NAVMESH_API NavmeshDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( NavmeshDebugView );

    public:

        NavmeshDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawMenu( EntityUpdateContext const& context );

    private:

        NavmeshSystem*          m_pNavmeshSystem = nullptr;
        NavmeshWorldSystem*     m_pNavmeshWorldSystem = nullptr;
    };
}
#endif