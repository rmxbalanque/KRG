#pragma once

#include "UUID.h"
#include "System/Core/Logging/Log.h"
#include <EASTL/functional.h>

//-------------------------------------------------------------------------
// Generic Events
//-------------------------------------------------------------------------
//
// Single user events also allow return types
//
// Use the TSingleUserEvent/TMultiUserEvent type to allows users to register for the event
// Use the TSingleUserEventImpl/TMultiUserEventImpl type internally to allow the event to be fired.
//
// e.g.
//
// class Foo
// {
// public:
// 
//     inline TSingleUserEvent<void(int,int)>& OnEvent() { return m_onEventA; }
//     inline TMultiUserEvent<int,int>& OnEvent() { return m_onEventB; }
// 
// private:
// 
//     TSingleUserEventInternal<void(int,int)>          m_onEventA;
//     TMultiUserEventInternal<int,int>                 m_onEventB;
// };

namespace KRG
{
    template<typename T> using TFunction = eastl::function<T>;

    //-------------------------------------------------------------------------

    class EventBindingID
    {
        template<typename R, typename... Args> friend class TSingleUserEventInternal;
        template<typename... Args> friend class TMultiUserEventInternal;

    public:

        EventBindingID() = default;
        inline bool IsValid() const { return m_ID.IsValid(); }
        inline String ToString() const { return eastl::move( m_ID.ToString() ); }

    private:

        EventBindingID( UUID& newID ) : m_ID( newID ) {}
        EventBindingID( UUID&& newID ) : m_ID( newID ) {}
        inline void Reset() { m_ID.Reset(); }

    private:

        UUID    m_ID;
    };

    //-------------------------------------------------------------------------
    // Single User Event
    //-------------------------------------------------------------------------

    template<typename R, typename... Args>
    class TSingleUserEvent;

    template<typename R, typename... Args>
    class TSingleUserEventInternal
    {
        friend TSingleUserEvent<R, Args...>;

    public:

        TSingleUserEventInternal()
        {
            KRG_ASSERT( m_function == nullptr );
        }

        ~TSingleUserEventInternal()
        {
            // We need the log here since the assert is inlined and we dont know where it came from
            if ( HasBoundUser() )
            {
                KRG_LOG_ERROR( "Core", "Event still has bound users at destruction" );
                KRG_HALT();
            }
        }

        inline bool HasBoundUser() const { return m_function != nullptr; }

        inline R Execute( Args... args ) const
        {
            KRG_ASSERT( m_function != nullptr );
            return m_function( std::forward<Args>( args )... );
        }

    private:

        inline EventBindingID Bind( eastl::function<R( Args... )>&& function )
        {
            KRG_ASSERT( !HasBoundUser() );
            m_bindingID = UUID::GenerateID();
            m_function = function;
            return EventBindingID( m_bindingID );
        }

        inline void Unbind( EventBindingID& handle )
        {
            KRG_ASSERT( HasBoundUser() && m_bindingID == handle.m_ID );
            m_function = nullptr;
            m_bindingID.Reset();
            handle.Reset();
        }

    private:

        UUID                                m_bindingID;
        eastl::function<R( Args... )>       m_function = nullptr;
    };

    //-------------------------------------------------------------------------

    template<typename R, typename... Args>
    class TSingleUserEvent
    {
    public:

        TSingleUserEvent( TSingleUserEventInternal<R, Args...>& event ) : m_pEvent( &event ) {}
        TSingleUserEvent( TSingleUserEventInternal<R, Args...>* event ) : m_pEvent( event ) {}

        [[nodiscard]] inline EventBindingID Bind( eastl::function<R( Args... )>&& function )
        {
            return m_pEvent->Bind( eastl::move( function ) );
        }

        inline void Unbind( EventBindingID& handle )
        {
            m_pEvent->Unbind( handle );
        }

    private:

        TSingleUserEventInternal<R, Args...>* m_pEvent = nullptr;
    };

    //-------------------------------------------------------------------------
    // Multi User Event
    //-------------------------------------------------------------------------

    template<typename... Args> 
    class TMultiUserEvent;

    template<typename... Args>
    class TMultiUserEventInternal
    {
        friend TMultiUserEvent<Args...>;

        struct BoundUser
        {
            BoundUser( eastl::function<void( Args... )>&& function )
                : m_function( function ) 
            {}

            void Reset()
            {
                m_ID.Reset();
                m_function = nullptr;
            }

        public:

            UUID                            m_ID = UUID::GenerateID();
            TFunction<void( Args... )>      m_function = nullptr;
        };

    public:

        ~TMultiUserEventInternal()
        {
            if ( HasBoundUsers() )
            {
                KRG_LOG_ERROR( "Core", "Event still has bound users at destruction" );
                KRG_HALT();
            }
        }

        inline bool HasBoundUsers() const { return !m_boundUsers.empty(); }

        inline void Execute( Args... args ) const
        {
            for ( auto& boundUser : m_boundUsers )
            {
                KRG_ASSERT( boundUser.m_function != nullptr );
                boundUser.m_function( std::forward<Args>( args )... );
            }
        }

    private:

        inline EventBindingID Bind( eastl::function<void( Args... )>&& function )
        {
            auto& boundUser = m_boundUsers.emplace_back( BoundUser( eastl::forward<eastl::function<void( Args... )>&&>( function ) ) );
            boundUser.m_function = function;
            return EventBindingID( boundUser.m_ID );
        }

        inline void Unbind( EventBindingID bindingID )
        {
            auto searchPredicate = [] ( BoundUser const& boundUser, EventBindingID const& bindingID ) { return boundUser.m_ID == bindingID.m_ID; };
            auto foundIter = eastl::find( m_boundUsers.begin(), m_boundUsers.end(), bindingID, searchPredicate );

            KRG_ASSERT( foundIter != m_boundUsers.end() );
            m_boundUsers.erase( foundIter );
            bindingID.Reset();

            // Always free memory when we completely empty the array (this is need for statically created global events since the allocators are released before the events are destroyed)
            if ( m_boundUsers.empty() )
            {
                m_boundUsers.shrink_to_fit();
            }
        }

    private:

        TVector<BoundUser>              m_boundUsers;
    };

    template<typename... Args>
    class TMultiUserEvent
    {
    public:

        TMultiUserEvent( TMultiUserEventInternal<Args...>& event ) : m_pEvent( &event ) {}
        TMultiUserEvent( TMultiUserEventInternal<Args...>* event ) : m_pEvent( event ) {}

        [[nodiscard]] inline EventBindingID Bind( eastl::function<void( Args... )>&& function )
        {
            return m_pEvent->Bind( eastl::move( function ) );
        }

        inline void Unbind( EventBindingID& handle )
        {
            m_pEvent->Unbind( handle );
        }

    private:

        TMultiUserEventInternal<Args...>* m_pEvent = nullptr;
    };
}