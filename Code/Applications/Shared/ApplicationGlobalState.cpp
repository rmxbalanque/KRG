#include "ApplicationGlobalState.h"
#include "System/TypeSystem/CoreTypeIDs.h"
#include "System/Core/Memory/Memory.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Profiling/Profiling.h"
#include "System/Core/Threading/Threading.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace
    {
        static bool g_platformInitialized = false;
    }

    //-------------------------------------------------------------------------

    ApplicationGlobalState::ApplicationGlobalState( char const* pMainThreadName )
    {
        KRG_ASSERT( !g_platformInitialized );

        // Initialize memory and threading subsystems
        //-------------------------------------------------------------------------

        Memory::Initialize();
        Threading::Initialize( ( pMainThreadName != nullptr ) ? pMainThreadName : "Kruger Main Thread" );

        StringID::Initialize();
        Log::Initialize();

        TypeSystem::Initialize();

        g_platformInitialized = true;
        m_primaryState = true;
    }

    ApplicationGlobalState::~ApplicationGlobalState()
    {
        if ( m_primaryState )
        {
            KRG_ASSERT( g_platformInitialized );
            g_platformInitialized = false;
            m_primaryState = false;

            TypeSystem::Shutdown();

            Log::Shutdown();
            StringID::Shutdown();

            Threading::Shutdown();
            Memory::Shutdown();
        }
    }
}