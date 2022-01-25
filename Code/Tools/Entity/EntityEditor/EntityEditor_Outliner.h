#pragma once
#include "Tools/Entity/_Module/API.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class UpdateContext;
}

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    class EntityEditorContext;

    //-------------------------------------------------------------------------

    class KRG_TOOLS_ENTITY_API EntityEditorOutliner
    {
    public:

        EntityEditorOutliner( EntityEditorContext& ctx );

        void Draw( UpdateContext const& context );

    private:

        EntityEditorContext&                m_context;
    };
}