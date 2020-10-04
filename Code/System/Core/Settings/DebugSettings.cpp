#include "DebugSettings.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    DebugSetting::DebugSetting( char const* pName, char const* pCategory, char const* pDescription, Type type )
        : DebugSetting( pName, pCategory, type )
    {
        // Description Validation
        //-------------------------------------------------------------------------

        size_t const descriptionLength = ( pDescription == nullptr ) ? 0 : strlen( pDescription );
        KRG_ASSERT( descriptionLength < 256 );

        if ( pDescription != nullptr )
        {
            strcpy( const_cast<char*>( m_description ), pDescription );
        }
    }

    DebugSetting::DebugSetting( char const* pName, char const* pCategory, Type type )
        : Setting( pName, pCategory, type )
    {
        // Category Validation
        //-------------------------------------------------------------------------

        size_t const categoryLength = strlen( pCategory );
        for ( auto i = 0; i < categoryLength - 1; i++ )
        {
            KRG_ASSERT( isalnum( pCategory[i] ) || pCategory[i] == ' ' || pCategory[i] == CategorySeperatorToken );
        }
    }

    //-------------------------------------------------------------------------

    DebugSettingBool::DebugSettingBool( char const* pName, char const* pCategory, char const* pDescription, bool initialValue )
        : DebugSetting( pName, pCategory, pDescription, Setting::Type::Bool )
        , m_value( initialValue )
    {}

    //-------------------------------------------------------------------------

    DebugSettingInt::DebugSettingInt( char const* pName, char const* pCategory, char const* pDescription, S32 initialValue )
        : DebugSetting( pName, pCategory, pDescription, Setting::Type::Int )
        , m_value( initialValue )
    {}

    DebugSettingInt::DebugSettingInt( char const* pName, char const* pCategory, char const* pDescription, S32 initialValue, S32 min, S32 max )
        : DebugSetting( pName, pCategory, pDescription, Setting::Type::Int )
        , m_value( initialValue )
        , m_min( min )
        , m_max( max )
    {}

    //-------------------------------------------------------------------------

    DebugSettingFloat::DebugSettingFloat( char const* pName, char const* pCategory, char const* pDescription, F32 initialValue )
        : DebugSetting( pName, pCategory, pDescription, Setting::Type::Float )
        , m_value( initialValue )
    {}

    DebugSettingFloat::DebugSettingFloat( char const* pName, char const* pCategory, char const* pDescription, F32 initialValue, F32 min, F32 max )
        : DebugSetting( pName, pCategory, pDescription, Setting::Type::Float )
        , m_value( initialValue )
        , m_min( min )
        , m_max( max )
    {}
}
#endif