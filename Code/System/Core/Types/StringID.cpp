#include "StringID.h"
#include "System/Core/Memory/Memory.h"
#include "System/Core/Algorithm/Hash.h"

//-------------------------------------------------------------------------

namespace KRG
{
    THashMap<uint32, char*> const StringID::StringIDMap;

    //-------------------------------------------------------------------------

    namespace
    {
        static char const* const InvalidStringID = "Invalid";

        //-------------------------------------------------------------------------

        class DebugStringAllocator
        {

        public:

            ~DebugStringAllocator()
            {
                auto& nonConstStringMap = const_cast<THashMap<uint32, char*>&>( StringID::StringIDMap );
                for ( auto& debugString : nonConstStringMap )
                {
                    KRG::DeleteArray( debugString.second );
                }

                // Ensure we free the map memory since it was allocated with our custom allocator
                nonConstStringMap.clear( true );
            }

            // TODO: Make threadsafe
            char const* GetString( uint32 ID ) const
            {
                if ( ID == 0 )
                {
                    return InvalidStringID;
                }

                auto iter = StringID::StringIDMap.find( ID );
                if ( iter != StringID::StringIDMap.end() )
                {
                    return iter->second;
                }
                else
                {
                    return nullptr;
                }
            }

            // TODO: Make threadsafe
            void RegisterDebugString( uint32 ID, char const* pStr )
            {
                auto iter = StringID::StringIDMap.find( ID );
                if ( iter == StringID::StringIDMap.end() )
                {
                    auto const size = strlen( pStr ) + 1;
                    auto pNewString = KRG::NewArray<char>( size );
                    memcpy( pNewString, pStr, size );

                    auto& nonConstStringMap = const_cast<THashMap<uint32, char*>&>( StringID::StringIDMap );
                    nonConstStringMap[ID] = pNewString;
                }
            }
        };
    }

    //-------------------------------------------------------------------------

    StringID const StringID::InvalidID;
    DebugStringAllocator* g_pDebugStringAllocator = nullptr;

    //-------------------------------------------------------------------------

    void StringID::Initialize()
    {
        #if KRG_DEVELOPMENT_TOOLS
        g_pDebugStringAllocator = KRG::New<DebugStringAllocator>();
        #endif
    }

    void StringID::Shutdown()
    {
        #if KRG_DEVELOPMENT_TOOLS
        KRG::Delete( g_pDebugStringAllocator );
        #endif
    }

    //-------------------------------------------------------------------------

    StringID::StringID( char const* pStr )
        : m_ID( Hash::GetHash32( pStr ) )
    {
        #if KRG_DEVELOPMENT_TOOLS
        g_pDebugStringAllocator->RegisterDebugString( m_ID, pStr );
        #endif
    }

    StringID::StringID( uint32 ID )
        : m_ID( ID )
    {}

    #if KRG_DEVELOPMENT_TOOLS
    char const* StringID::ToString() const
    {
        return g_pDebugStringAllocator->GetString( m_ID );
    }
    #endif
}