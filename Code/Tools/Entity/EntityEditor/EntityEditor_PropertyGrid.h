#pragma once
#include "Tools/Entity/_Module/API.h"
#include "Tools/Core/PropertyGrid/PropertyGrid.h"

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

    class KRG_TOOLS_ENTITY_API EntityEditorPropertyGrid
    {
    public:

        EntityEditorPropertyGrid( EntityEditorContext& ctx );
        ~EntityEditorPropertyGrid();

        void Draw( UpdateContext const& context );

    private:

        void PreEdit( PropertyEditInfo const& eventInfo );
        void PostEdit( PropertyEditInfo const& eventInfo );

    private:

        EntityEditorContext&            m_context;
        EventBindingID                  m_preEditBindingID;
        EventBindingID                  m_postEditBindingID;
        PropertyGrid                    m_propertyGrid;
    };
}