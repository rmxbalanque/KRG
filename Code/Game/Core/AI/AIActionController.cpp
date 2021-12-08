#include "AIActionController.h"
#include "System/Core/Logging/Log.h"

// Actions

//-------------------------------------------------------------------------

namespace KRG::AI
{
    ActionController::ActionController()
    {

    }

    ActionController::~ActionController()
    {
        for ( auto& pAction : m_overlayActions )
        {
            KRG::Delete( pAction );
        }
        m_overlayActions.clear();

        for ( auto& pAction : m_baseActions )
        {
            KRG::Delete( pAction );
        }

        m_activeBaseActionID = InvalidAction;

        //-------------------------------------------------------------------------

        for ( auto pBaseAction : m_baseActions )
        {
            KRG_ASSERT( pBaseAction == nullptr );
        }

        KRG_ASSERT( m_overlayActions.empty() );
    }

    //-------------------------------------------------------------------------

    void ActionController::Update( ActionContext const& context )
    {
        KRG_ASSERT( context.IsValid() );
    }
}