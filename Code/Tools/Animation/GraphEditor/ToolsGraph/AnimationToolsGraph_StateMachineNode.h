#pragma once

#include "Tools/Core/GraphEditor/StateMachine/StateMachineGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_StateBaseNode : public GraphEditor::SM::State
    {
        friend class Tools_StateMachineNode;
        KRG_REGISTER_TYPE( Tools_StateBaseNode );

    public:

        struct TimedStateEvent : public IRegisteredType
        {
            KRG_REGISTER_TYPE( TimedStateEvent );

            KRG_EXPOSE StringID                 m_ID;
            KRG_EXPOSE Seconds                  m_timeValue;
        };

    public:

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }

    protected:

        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

    protected:

        KRG_EXPOSE String                       m_name = "State";
        KRG_EXPOSE TVector<StringID>            m_entryEvents;
        KRG_EXPOSE TVector<StringID>            m_executeEvents;
        KRG_EXPOSE TVector<StringID>            m_exitEvents;
        KRG_EXPOSE TVector<TimedStateEvent>     m_timeRemainingEvents;
        KRG_EXPOSE TVector<TimedStateEvent>     m_timeElapsedEvents;
    };
}