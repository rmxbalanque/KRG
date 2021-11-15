#pragma once

#include "Engine/Core/_Module/API.h"
#include "System/Core/Types/UUID.h"
#include "System/Core/Types/Event.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"

//-------------------------------------------------------------------------

struct ImGuiWindowClass;

//-------------------------------------------------------------------------
// Runtime Debug View
//-------------------------------------------------------------------------
// Provides debug info and tools for a given world
// We create an instance of each of these views per world
// They are drawn at the end of each frame

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class SystemRegistry;
    class EntityWorld;
    class EntityUpdateContext;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityWorldDebugView : public IRegisteredType
    {
        KRG_REGISTER_TYPE( EntityWorldDebugView );

        friend class EntityWorldDebugger;

    public:

        //-------------------------------------------------------------------------
        // Debug Menu
        //-------------------------------------------------------------------------
        // The draw function will be called once the debug UI menu is shown and the appropriate category menu has been opened
        // The name is not displayed, but is used for sorting the various callbacks in the menu (e.g. if we have multiple callback within the render category)
        // Categories are treated as paths and so are separated using '/' (e.g. "Input/Controller Utils")

        class DebugMenu
        {
        public:

            DebugMenu( String const& name, String const& category, TFunction<void( EntityUpdateContext const& )>&& drawFunction )
                : m_name( name )
                , m_category( category )
                , m_drawFunction( drawFunction )
            {
                KRG_ASSERT( !m_name.empty() && !m_category.empty() && m_drawFunction != nullptr );
            }

            inline String const& GetName() const { return m_name; }
            inline String const& GetCategory() const { return m_category; }
            inline void Execute( EntityUpdateContext const& ctx ) const { m_drawFunction( ctx ); }

        private:

            String                                              m_name;
            String                                              m_category;
            TFunction<void( EntityUpdateContext const& )>       m_drawFunction;
        };

    public:

        virtual ~EntityWorldDebugView() {}

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) {};
        virtual void Shutdown() {};

    protected:

        // Called to draw all imgui windows
        virtual void DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass ) = 0;

        // Called within the context of a large overlay window allowing you to draw helpers and widgets over a viewport
        virtual void DrawOverlayElements( EntityUpdateContext const& context ) {}

    protected:

        TVector<DebugMenu>      m_menus;
    };
}
#endif