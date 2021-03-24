#pragma once

#include "../_Module/API.h"
#include "System/Core/Types/UUID.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class UpdateContext;

    //-------------------------------------------------------------------------

    namespace Debug
    {
        //-------------------------------------------------------------------------
        // Debug Menu Drawing Callback
        //-------------------------------------------------------------------------
        // The draw function will be called once the debug UI menu is shown and the appropriate category menu has been opened
        // The name is not displayed, but is used for sorting the various callbacks in the menu (e.g. if we have multiple callback within the render category)
        // Categories are treated as paths and so are separated using '/' (e.g. "Input/Controller Utils")

        class DebugMenuCallback
        {
        public:

            DebugMenuCallback( String const& name, String const& category, TFunction<void( UpdateContext const& )>&& drawFunction )
                : m_name( name )
                , m_category( category )
                , m_drawFunction( drawFunction )
            {
                KRG_ASSERT( !m_name.empty() && !m_category.empty() && m_drawFunction != nullptr );
            }

            inline String const& GetName() const { return m_name; }
            inline String const& GetCategory() const { return m_category; }
            inline void Execute( UpdateContext const& ctx ) const { m_drawFunction( ctx ); }

        private:

            String                                              m_name;
            String                                              m_category;
            TFunction<void( UpdateContext const& )>             m_drawFunction;
        };

        //-------------------------------------------------------------------------
        // Debug View
        //-------------------------------------------------------------------------
        // This is responsible for registering debug menu callbacks and for drawing any open debug windows

        class KRG_ENGINE_CORE_API DebugView
        {
            friend class DebugTools;

        public:

            virtual ~DebugView() {}

        protected:

            inline TVector<DebugMenuCallback> const& GetMenuCallbacks() const { return m_menuCallbacks; }
            virtual void DrawWindows( UpdateContext const& context ) = 0;

        protected:

            TVector<DebugMenuCallback>  m_menuCallbacks;
        };
    }
}
#endif