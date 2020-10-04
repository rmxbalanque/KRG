#include "ConfigSettings.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG
{
    ConfigSetting::ConfigSetting( char const* pName, char const* pCategory, Type type, bool isOptional )
        : Setting( pName, pCategory, type )
        , m_isOptional( isOptional )
    {
        // Category Validation
        //-------------------------------------------------------------------------

        size_t const categoryLength = strlen( pCategory );
        for ( auto i = 0; i < categoryLength - 1; i++ )
        {
            KRG_ASSERT( isalnum( pCategory[i] ) || pCategory[i] == ' ' );
        }
    }

    //-------------------------------------------------------------------------

    ConfigSettingBool::ConfigSettingBool( char const* pName, char const* pCategory )
        : ConfigSetting( pName, pCategory, Setting::Type::Bool, false )
    {}

    ConfigSettingBool::ConfigSettingBool( char const* pName, char const* pCategory, bool initialValue )
        : ConfigSetting( pName, pCategory, Setting::Type::Bool, true )
        , m_value( initialValue )
    {}

    //-------------------------------------------------------------------------

    ConfigSettingInt::ConfigSettingInt( char const* pName, char const* pCategory )
        : ConfigSetting( pName, pCategory, Setting::Type::Int, false )
    {}

    ConfigSettingInt::ConfigSettingInt( char const* pName, char const* pCategory, S32 initialValue )
        : ConfigSetting( pName, pCategory, Setting::Type::Int, true )
        , m_value( initialValue )
    {}

    ConfigSettingInt::ConfigSettingInt( char const* pName, char const* pCategory, S32 initialValue, S32 min, S32 max )
        : ConfigSetting( pName, pCategory, Setting::Type::Int, true )
        , m_value( initialValue )
        , m_min( min )
        , m_max( max )
    {}

    //-------------------------------------------------------------------------

    ConfigSettingFloat::ConfigSettingFloat( char const* pName, char const* pCategory )
        : ConfigSetting( pName, pCategory, Setting::Type::Float, false )
    {}

    ConfigSettingFloat::ConfigSettingFloat( char const* pName, char const* pCategory, F32 initialValue )
        : ConfigSetting( pName, pCategory, Setting::Type::Float, true )
        , m_value( initialValue )
    {}

    ConfigSettingFloat::ConfigSettingFloat( char const* pName, char const* pCategory, F32 initialValue, F32 min, F32 max )
        : ConfigSetting( pName, pCategory, Setting::Type::Float, true )
        , m_value( initialValue )
        , m_min( min )
        , m_max( max )
    {}

    //-------------------------------------------------------------------------

    ConfigSettingString::ConfigSettingString( char const* pName, char const* pCategory )
        : ConfigSetting( pName, pCategory, Setting::Type::String, false )
    {}

    ConfigSettingString::ConfigSettingString( char const* pName, char const* pCategory, char const* pInitialValue )
        : ConfigSetting( pName, pCategory, Setting::Type::String, true )
    {
        KRG_ASSERT( pInitialValue != nullptr );

        // We cant log a warning since these settings are statically defined so the logging system is not instantiated yet.
        // We we will crash in this case since this a invalid CODE DEFINED default value.
        size_t const length = strlen( pInitialValue );
        KRG_ASSERT( length < BufferSize );
        strncpy( const_cast<char*>( m_value ), pInitialValue, BufferSize );
    }

    ConfigSettingString& ConfigSettingString::operator=( char const* pValue )
    {
        // Only allocate if we have bound users
        String oldValue;
        if( m_onValueChangedEvent.HasBoundUsers() )
        {
            oldValue = m_value;
        }

        size_t const length = strlen( pValue );
        if( length > BufferSize )
        {
            KRG_LOG_WARNING( "System", "String value for configuration parameter (%s/%s) exceeds max buffer size of %d", m_category, m_name, BufferSize );
        }

        strncpy( const_cast<char*>( m_value ), pValue, BufferSize );
        m_onValueChangedEvent.Execute( oldValue, m_value );
        return *this;
    }
}