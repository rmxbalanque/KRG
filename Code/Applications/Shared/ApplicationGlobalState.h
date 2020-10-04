#pragma once

//-------------------------------------------------------------------------
// Application Global State
//-------------------------------------------------------------------------
// Scoped object managing the lifetime of all KRG global state and systems.
// e.g. Memory Allocators
//      Logging
//      Profiling
//      etc...
//
// An object of ApplicationGlobalState must be the first thing declared on 
// the stack in the entry point function of any KRG application.
//-------------------------------------------------------------------------

namespace KRG
{
    class ApplicationGlobalState
    {
    public:

        ApplicationGlobalState( char const* pMainThreadName = nullptr );
        ~ApplicationGlobalState();

    private:

        bool m_primaryState = false;
    };
}