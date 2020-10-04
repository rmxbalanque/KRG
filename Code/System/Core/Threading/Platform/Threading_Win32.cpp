#if _WIN32
#include "System/Core/Platform/ProcessorInfo.h"
#include "System/Core/Threading/Threading.h"
#include <windows.h>

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Threading
    {
        ThreadID GetCurrentThreadID()
        {
            auto pNativeThreadHandle = GetCurrentThread();
            ThreadID const nativeThreadID = GetThreadId( pNativeThreadHandle );
            return nativeThreadID;
        }

        void SetCurrentThreadName( char const* pName )
        {
            KRG_ASSERT( pName != nullptr );
            wchar_t wThreadName[255];
            auto pNativeThreadHandle = GetCurrentThread();
            DWORD const nativeThreadID = GetThreadId( pNativeThreadHandle );
            MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, pName, -1, wThreadName, 255 );
            SetThreadDescription( pNativeThreadHandle, wThreadName );
        }

        //-------------------------------------------------------------------------

        SyncEvent::SyncEvent()
            : m_pNativeHandle( nullptr )
        {
            m_pNativeHandle = CreateEvent( nullptr, TRUE, FALSE, nullptr );
            KRG_ASSERT( m_pNativeHandle != nullptr );
        }

        void SyncEvent::Signal()
        {
            KRG_ASSERT( m_pNativeHandle != nullptr );
            auto result = SetEvent( m_pNativeHandle );
            KRG_ASSERT( result );
        }

        void SyncEvent::Reset()
        {
            KRG_ASSERT( m_pNativeHandle != nullptr );
            auto result = ResetEvent( m_pNativeHandle );
            KRG_ASSERT( result );
        }

        void SyncEvent::Wait() const
        {
            KRG_ASSERT( m_pNativeHandle != nullptr );
            WaitForSingleObject( m_pNativeHandle, INFINITE );
        }

        void SyncEvent::Wait( Milliseconds maxWaitTime ) const
        {
            KRG_ASSERT( m_pNativeHandle != nullptr );
            WaitForSingleObject( m_pNativeHandle, (U32) maxWaitTime );
        }
    }
}
#endif