#include "CoreTypeSerializers.h"
#include "TypeRegistry.h"
#include "TypeInfo.h"
#include "System/Resource/ResourcePtr.h"
#include "System/Core/Serialization/BinaryArchive.h"
#include "System/Core/Time/Time.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Types/UUID.h"
#include "System/Core/Types/StringID.h"
#include "System/Core/Types/BitFlags.h"
#include "System/Core/Types/Percentage.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

namespace KRG::TypeSystem
{
    class CoreTypeSerializer
    {
    public:

        // String serialization
        //-------------------------------------------------------------------------

        template<typename T>
        inline static void StringValueToTypeValue( String const& str, T& outValue ) { KRG_UNIMPLEMENTED_FUNCTION(); }

        template<typename T>
        inline static void TypeValueToStringValue( T const& value, String& outStr ) { KRG_UNIMPLEMENTED_FUNCTION(); }

        // Binary serialization
        //-------------------------------------------------------------------------

        template<typename T>
        inline static void BinaryToTypeValue( TVector<Byte> const& binaryData, T& outValue )
        {
            Serialization::BinaryMemoryArchive archive( Serialization::Mode::Read, const_cast<TVector<Byte>&>( binaryData ) );
            archive >> outValue;
        }

        template<typename T>
        inline static void TypeValueToBinary( T const& value, TVector<Byte>& outBinaryData )
        {
            Serialization::BinaryMemoryArchive archive( Serialization::Mode::Write, outBinaryData );
            archive << value;
        }

        // Helpers
        //-------------------------------------------------------------------------

        template<typename T>
        inline static void BinaryToString( TVector<Byte> const& binaryData, String& outStr )
        {
            T value;
            BinaryToTypeValue( binaryData, value );
            CoreTypeSerializer::TypeValueToStringValue( value, outStr );
        }

        template<typename T>
        inline static void StringToBinary( String const& str, TVector<Byte>& outBinaryData )
        {
            T value;
            StringValueToTypeValue( str, value );
            TypeValueToBinary( value, outBinaryData );
        }

    private:

        inline static void StringToFloatArray( String const& str, char const* delim, int32 const numFloats, float* pFloats )
        {
            char substr[128] = { 0 };
            int32 resIdx = 0;
            auto delimLength = strlen( delim );

            bool complete = false;
            size_t startIdx = 0;
            while ( resIdx < numFloats && !complete )
            {
                size_t endIdx = str.find_first_of( delim, startIdx );
                if ( endIdx == String::npos )
                {
                    endIdx = str.length();
                    complete = true;
                }

                size_t sizeToCopy = endIdx - startIdx;
                KRG_ASSERT( sizeToCopy < 128 );
                memcpy( substr, &str.c_str()[startIdx], sizeToCopy );
                substr[sizeToCopy] = '\0';

                pFloats[resIdx++] = std::strtof( substr, nullptr );
                startIdx = endIdx + delimLength;
            }
        }

        inline static void FloatArrayToString( float const* pFloats, char const* delim, int32 const numFloats, String& outStr )
        {
            outStr.clear();

            for ( int32 i = 0; i < numFloats; i++ )
            {
                outStr += eastl::to_string( pFloats[i] );

                if ( i != ( numFloats - 1 ) )
                {
                    outStr += delim;
                }
            }
        }
    };

    //-------------------------------------------------------------------------
    // Core Type Serializers Specializations
    //-------------------------------------------------------------------------

    // Bool
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, bool& value )
    {
        String lowerString = str;
        lowerString.make_lower();
        value = ( lowerString == "true" );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( bool const& value, String& outStr )
    {
        outStr = value ? "True" : "False";
    }

    // Uint8
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, uint8& value )
    {
        value = (uint8) std::strtoul( str.c_str(), nullptr, 0 );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( uint8 const& value, String& outStr )
    {
        outStr = eastl::to_string( value );
    }

    // Uint16
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, uint16& value )
    {
        value = (uint16) std::strtoul( str.c_str(), nullptr, 0 );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( uint16 const& value, String& outStr )
    {
        outStr = eastl::to_string( value );
    }

    // Uint32
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, uint32& value )
    {
        value = std::strtoul( str.c_str(), nullptr, 0 );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( uint32 const& value, String& outStr )
    {
        outStr = eastl::to_string( value );
    }

    // Uint64
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, uint64& value )
    {
        value = std::strtoull( str.c_str(), nullptr, 0 );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( uint64 const& value, String& outStr )
    {
        outStr = eastl::to_string( value );
    }

    // Int8
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, int8& value )
    {
        value = (int8) std::strtol( str.c_str(), nullptr, 0 );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( int8 const& value, String& outStr )
    {
        outStr = eastl::to_string( value );
    }

    // Int16
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, int16& value )
    {
        value = (int16) std::strtol( str.c_str(), nullptr, 0 );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( int16 const& value, String& outStr )
    {
        outStr = eastl::to_string( value );
    }

    // Int32
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, int32& value )
    {
        value = std::strtol( str.c_str(), nullptr, 0 );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( int32 const& value, String& outStr )
    {
        outStr = eastl::to_string( value );
    }

    // Int64
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, int64& value )
    {
        value = std::strtol( str.c_str(), nullptr, 0 );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( int64 const& value, String& outStr )
    {
        outStr = eastl::to_string( value );
    }

    // Float
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, float& value )
    {
        value = std::strtof( str.c_str(), nullptr );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( float const& value, String& outStr )
    {
        outStr = eastl::to_string( value );
    }

    // Double
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, double& value )
    {
        value = std::strtod( str.c_str(), nullptr );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( double const& value, String& outStr )
    {
        outStr = eastl::to_string( value );
    }

    // String
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, String& value )
    {
        value = str;
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( String const& value, String& outStr )
    {
        outStr = value;
    }

    // String ID
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, StringID& value )
    {
        value = StringID( str.c_str() );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( StringID const& value, String& outStr )
    {
        char const* pStr = value.ToString();
        KRG_ASSERT( pStr != nullptr );
        outStr = pStr;
    }

    // Color
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Color& value )
    {
        uint32 const colorType = std::strtoul( str.c_str(), nullptr, 0 );
        value = Color( colorType );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Color const& value, String& outStr )
    {
        outStr.sprintf( "%02X%02X%02X%02X", value.m_byteColor.m_r, value.m_byteColor.m_g, value.m_byteColor.m_b, value.m_byteColor.m_a );
    }

    // UUID
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, UUID& value )
    {
        value = UUID( str );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( UUID const& value, String& outStr )
    {
        outStr = value.ToString();
    }

    // Float2
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Float2& value )
    {
        StringToFloatArray( str, ",", 2, &value[0] );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Float2 const& value, String& outStr )
    {
        FloatArrayToString( &value[0], ",", 2, outStr );
    }

    // Float3
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Float3& value )
    {
        StringToFloatArray( str, ",", 3, &value[0] );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Float3 const& value, String& outStr )
    {
        FloatArrayToString( &value[0], ",", 3, outStr );
    }

    // Float4
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Float4& value )
    {
        StringToFloatArray( str, ",", 4, &value[0] );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Float4 const& value, String& outStr )
    {
        FloatArrayToString( &value[0], ",", 4, outStr );
    }

    // Matrix
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Matrix& value )
    {
        float floatData[9];
        StringToFloatArray( str, ",", 9, floatData );

        EulerAngles const rotation( floatData[0], floatData[1], floatData[2] );
        Vector const translation = Vector( floatData[3], floatData[4], floatData[5] );
        value = Matrix( Quaternion( rotation ), translation );

        Vector const scale = Vector( floatData[6], floatData[7], floatData[8] );
        value.SetScale( scale );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Matrix const& value, String& outStr )
    {
        // Handle uninitialized matrix
        if ( value.IsOrthonormal() )
        {
            auto eulerAngles = value.ToEulerAngles();

            float floatData[9];
            (Float3&) floatData = Float3( (float) eulerAngles.m_x.ToDegrees(), (float) eulerAngles.m_y.ToDegrees(), (float) eulerAngles.m_z.ToDegrees() );
            (Float3&) floatData[3] = value.GetTranslation().ToFloat3();
            (Float3&) floatData[6] = value.GetScale().ToFloat3();

            FloatArrayToString( floatData, ",", 9, outStr );
        }
        else
        {
            outStr = "0,0,0,0,0,0,0,0,0";
        }
    }

    // Transform
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Transform& value )
    {
        float floatData[9];
        StringToFloatArray( str, ",", 9, floatData );

        // Set rotation
        EulerAngles const rotationEulerAngles( floatData[0], floatData[1], floatData[2] );
        value.SetRotation( Quaternion( rotationEulerAngles ) );

        // Set translation and scale
        value.SetTranslation( Vector( floatData[3], floatData[4], floatData[5], 1.0f ) );
        value.SetScale( Vector( floatData[6], floatData[7], floatData[8], 1.0f ) );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Transform const& value, String& outStr )
    {
        auto eulerAngles = value.GetRotation().ToEulerAngles();

        float floatData[9];

        floatData[0] = (float) eulerAngles.m_x.ToDegrees();
        floatData[1] = (float) eulerAngles.m_y.ToDegrees();
        floatData[2] = (float) eulerAngles.m_z.ToDegrees();

        floatData[3] = value.GetTranslation().m_x;
        floatData[4] = value.GetTranslation().m_y;
        floatData[5] = value.GetTranslation().m_z;

        floatData[6] = value.GetScale().m_x;
        floatData[7] = value.GetScale().m_y;
        floatData[8] = value.GetScale().m_z;

        //-------------------------------------------------------------------------

        String str;
        FloatArrayToString( floatData, ",", 9, str );
        outStr = str;
    }

    // Euler Angles
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, EulerAngles& value )
    {
        StringToFloatArray( str, ",", 3, (float*) &value.m_x );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( EulerAngles const& value, String& outStr )
    {
        FloatArrayToString( (float*) &value.m_x, ",", 3, outStr );
    }

    // Microseconds
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Microseconds& value )
    {
        value = Microseconds( std::strtof( str.c_str(), nullptr ) );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Microseconds const& value, String& outStr )
    {
        outStr = eastl::to_string( value.ToFloat() );
    }

    // Milliseconds
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Milliseconds& value )
    {
        value = Milliseconds( std::strtof( str.c_str(), nullptr ) );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Milliseconds const& value, String& outStr )
    {
        outStr = eastl::to_string( value.ToFloat() );
    }

    // Seconds
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Seconds& value )
    {
        value = Seconds( std::strtof( str.c_str(), nullptr ) );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Seconds const& value, String& outStr )
    {
        outStr = eastl::to_string( value.ToFloat() );
    }

    // Percentage
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Percentage& value )
    {
        value = Percentage( std::strtof( str.c_str(), nullptr ) );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Percentage const& value, String& outStr )
    {
        outStr = eastl::to_string( value.ToFloat() );
    }

    // Degrees
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Degrees& value )
    {
        value = Degrees( std::strtof( str.c_str(), nullptr ) );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Degrees const& value, String& outStr )
    {
        outStr = eastl::to_string( value.GetValue() );
    }

    // Radians
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Radians& value )
    {
        value = Radians( std::strtof( str.c_str(), nullptr ) );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Radians const& value, String& outStr )
    {
        outStr = eastl::to_string( value.GetValue() );
    }
    // Quaternion
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Quaternion& value )
    {
        StringToFloatArray( str, ",", 4, (float*) &value.m_x );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Quaternion const& value, String& outStr )
    {
        FloatArrayToString( (float*) &value.m_x, ",", 4, outStr );
    }

    // ResourceTypeID
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, ResourceTypeID& value )
    {
        value = ResourceTypeID( str );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( ResourceTypeID const& value, String& outStr )
    {
        outStr = value.ToString();
    }

    // DataPath
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, DataPath& value )
    {
        value = DataPath( str );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( DataPath const& value, String& outStr )
    {
        outStr = value.c_str();
    }

    // ResourceID
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, ResourceID& value )
    {
        if ( str.empty() )
        {
            value = ResourceID();
        }
        else
        {
            KRG_ASSERT( ResourceID::IsValidResourceIDString( str ) );
            value = ResourceID( str );
        }
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( ResourceID const& value, String& outStr )
    {
        outStr = value.ToString();
    }

    // ResourcePtr
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, Resource::ResourcePtr& value )
    {
        if ( str.empty() )
        {
            value = Resource::ResourcePtr();
        }
        else
        {
            KRG_ASSERT( ResourceID::IsValidResourceIDString( str ) );
            ResourceID const ID( str );
            value = Resource::ResourcePtr( ID );
        }
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( Resource::ResourcePtr const& value, String& outStr )
    {
        outStr = value.GetResourceID().ToString();
    }

    // BitFlags
    //-------------------------------------------------------------------------

    template<>
    inline static void CoreTypeSerializer::StringValueToTypeValue( String const& str, BitFlags& value )
    {
        value.SetFlags( std::strtol( str.c_str(), nullptr, 0 ) );
    }

    template<>
    inline static void CoreTypeSerializer::TypeValueToStringValue( BitFlags const& value, String& outStr )
    {
        outStr = eastl::to_string( value.GetFlags() );
    }
}

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Conversion
{
    bool ConvertStringToBitFlags( EnumInfo const& enumInfo, String const& str, BitFlags& outFlags )
    {
        outFlags.ClearAllFlags();

        // Handle hex format explicitly
        //-------------------------------------------------------------------------

        if ( str.find_first_of( "0x" ) == 0 || str.find_first_of( "0X" ) == 0 )
        {
            uint32 value = std::strtoul( str.c_str(), nullptr, 16 );
            outFlags.SetFlags( value );
            return true;
        }

        //-------------------------------------------------------------------------

        char buffer[256] = { 0 };
        size_t bufferIdx = 0;
        bool isReadingEnumValue = false;
        bool hasReadWhiteSpace = false;

        //-------------------------------------------------------------------------

        auto ProcessReadValue = [&] ()
        {
            if ( isReadingEnumValue )
            {
                buffer[bufferIdx] = 0;
                bufferIdx = 0;
                isReadingEnumValue = false;

                int64 flag;
                if ( enumInfo.TryGetConstantValue( StringID( buffer ), flag ) )
                {
                    outFlags.SetFlag( (uint8) flag, true );
                    return true;
                }
            }

            return false;
        };

        //-------------------------------------------------------------------------

        size_t const length = str.length();
        for ( auto i = 0u; i < length; i++ )
        {
            if ( str[i] == '|' )
            {
                if ( !ProcessReadValue() )
                {
                    outFlags.ClearAllFlags();
                    return false;
                }
                hasReadWhiteSpace = false;
            }
            else if ( str[i] == ' ' )
            {
                if ( isReadingEnumValue )
                {
                    hasReadWhiteSpace = true;
                }
            }
            else
            {
                // We read a white space while reading an enum value
                if ( hasReadWhiteSpace )
                {
                    outFlags.ClearAllFlags();
                    return false;
                }

                isReadingEnumValue = true;
                buffer[bufferIdx] = str[i];
                bufferIdx++;
                KRG_ASSERT( bufferIdx < 256 );
            }
        }

        //-------------------------------------------------------------------------

        if ( !ProcessReadValue() )
        {
            outFlags.ClearAllFlags();
            return false;
        }

        //-------------------------------------------------------------------------

        return true;
    }

    bool ConvertBitFlagsToString( EnumInfo const& enumInfo, BitFlags const& flags, String& outStr )
    {
        outStr.clear();

        for ( auto i = 0u; i < 32; i++ )
        {
            if ( flags.IsFlagSet( (uint8) i ) )
            {
                StringID label;
                if ( !enumInfo.TryGetConstantLabel( i, label ) )
                {
                    outStr.clear();
                    return false;
                }

                outStr += label.ToString();
                outStr += "|";
            }
        }

        //-------------------------------------------------------------------------

        if ( !outStr.empty() )
        {
            outStr.erase( outStr.end() - 1, outStr.end() );
        }

        return true;
    }
}

//-------------------------------------------------------------------------

namespace KRG::TypeSystem::Conversion
{
    void ConvertStringValueToTypeValue( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue, void* pValue )
    {
        // Special cases: Enums and TBitFlags
        if ( typeID == CoreTypes::TBitFlags )
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( templateArgumentTypeID );
            if ( pEnumInfo == nullptr )
            {
                KRG_LOG_WARNING( "TypeSystem", "Unknown enum class (%s) for TBitFlags", templateArgumentTypeID.GetAsStringID().c_str() );
                return;
            }

            if ( !ConvertStringToBitFlags( *pEnumInfo, strValue, *(BitFlags*) pValue ) )
            {
                KRG_LOG_WARNING( "TypeSystem", "Failed to convert string (%s) into valid TBitFlags<%s>", strValue.c_str(), templateArgumentTypeID.GetAsStringID().c_str() );
            }
        }
        else if( !IsCoreType( typeID ) ) // Enums has unique typeIDs
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( typeID );
            KRG_ASSERT( pEnumInfo != nullptr );
            
            StringID const enumID( strValue );
            int64 const enumValue = pEnumInfo->GetConstantValue( enumID );

            // We only support up to 32 bit enum types...
            switch ( pEnumInfo->m_underlyingType )
            {
                case CoreTypes::Uint8:
                {
                    *( (uint8*) pValue ) = (uint8) enumValue;
                }
                break;

                case CoreTypes::Int8:
                {
                    *( (int8*) pValue ) = (int8) enumValue;
                }
                break;

                case CoreTypes::Uint16:
                {
                    *( (uint16*) pValue ) = (uint16) enumValue;
                }
                break;

                case CoreTypes::Int16:
                {
                    *( (int16*) pValue ) = (int16) enumValue;
                }
                break;

                case CoreTypes::Uint32:
                {
                    *( (uint32*) pValue ) = (uint32) enumValue;
                }
                break;

                case CoreTypes::Int32:
                {
                    *( (int32*) pValue ) = (int32) enumValue;
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
        else // Real core types
        {
            if ( typeID == CoreTypes::Bool )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(bool*) pValue );
            }
            else if ( typeID == CoreTypes::Uint8 )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(uint8*) pValue );
            }
            else if ( typeID == CoreTypes::Int8 )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(int8*) pValue );
            }
            else if ( typeID == CoreTypes::Uint16 )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(uint16*) pValue );
            }
            else if ( typeID == CoreTypes::Int16 )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(int16*) pValue );
            }
            else if ( typeID == CoreTypes::Uint32 )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(uint32*) pValue );
            }
            else if ( typeID == CoreTypes::Int32 )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(int32*) pValue );
            }
            else if ( typeID == CoreTypes::Uint64 )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(uint64*) pValue );
            }
            else if ( typeID == CoreTypes::Int64 )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(int64*) pValue );
            }
            else if ( typeID == CoreTypes::Float )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(float*) pValue );
            }
            else if ( typeID == CoreTypes::Double )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(double*) pValue );
            }
            else if ( typeID == CoreTypes::String )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(String*) pValue );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(StringID*) pValue );
            }
            else if ( typeID == CoreTypes::Color )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Color*) pValue );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Float2*) pValue );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Float3*) pValue );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Float4*) pValue );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Transform*) pValue );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(EulerAngles*) pValue );
            }
            else if ( typeID == CoreTypes::Microseconds )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Microseconds*) pValue );
            }
            else if ( typeID == CoreTypes::Milliseconds )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Milliseconds*) pValue );
            }
            else if ( typeID == CoreTypes::Seconds )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Seconds*) pValue );
            }
            else if ( typeID == CoreTypes::Percentage )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Percentage*) pValue );
            }
            else  if ( typeID == CoreTypes::Degrees )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Degrees*) pValue );
            }
            else if ( typeID == CoreTypes::Radians )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Radians*) pValue );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Quaternion*) pValue );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Matrix*) pValue );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(DataPath*) pValue );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(ResourceTypeID*) pValue );
            }
            else if ( typeID == CoreTypes::ResourcePtr || typeID == CoreTypes::TResourcePtr )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(Resource::ResourcePtr*) pValue );
            }
            else if ( typeID == CoreTypes::ResourceID )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(ResourceID*) pValue );
            }
            else if ( typeID == CoreTypes::BitFlags )
            {
                CoreTypeSerializer::StringValueToTypeValue( strValue, *(BitFlags*) pValue );
            }
            else
            {
                KRG_UNREACHABLE_CODE();
            }
        }
    }

    void ConvertTypeValueToStringValue( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, void const* pValue, String & strValue )
    {
        // Special cases: Enums and TBitFlags
        if ( typeID == CoreTypes::TBitFlags )
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( templateArgumentTypeID );
            if ( pEnumInfo == nullptr )
            {
                KRG_LOG_WARNING( "TypeSystem", "Unknown enum class (%s) for TBitFlags", templateArgumentTypeID.GetAsStringID().c_str() );
                return;
            }

            if ( !ConvertBitFlagsToString( *pEnumInfo, *(BitFlags*) pValue, strValue ) )
            {
                KRG_LOG_WARNING( "TypeSystem", "Failed to convert string (%s) into valid TBitFlags<%s>", strValue.c_str(), templateArgumentTypeID.GetAsStringID().c_str() );
            }
        }
        else if ( !IsCoreType( typeID ) ) // Enums has unique typeIDs
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( typeID );
            KRG_ASSERT( pEnumInfo != nullptr );

            StringID const enumID( strValue );
            int64 const enumValue = pEnumInfo->GetConstantValue( enumID );

            // We only support up to 32 bit enum types...
            switch ( pEnumInfo->m_underlyingType )
            {
                case CoreTypes::Uint8:
                {
                    strValue = pEnumInfo->GetConstantLabel( *( (uint8*) pValue ) ).c_str();
                }
                break;

                case CoreTypes::Int8:
                {
                    strValue = pEnumInfo->GetConstantLabel( *( (int8*) pValue ) ).c_str();
                }
                break;

                case CoreTypes::Uint16:
                {
                    strValue = pEnumInfo->GetConstantLabel( *( (uint16*) pValue ) ).c_str();
                }
                break;

                case CoreTypes::Int16:
                {
                    strValue = pEnumInfo->GetConstantLabel( *( (int16*) pValue ) ).c_str();
                }
                break;

                case CoreTypes::Uint32:
                {
                    strValue = pEnumInfo->GetConstantLabel( *( (uint32*) pValue ) ).c_str();
                }
                break;

                case CoreTypes::Int32:
                {
                    strValue = pEnumInfo->GetConstantLabel( *( (int32*) pValue ) ).c_str();
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
        else // Real core types
        {
            KRG_ASSERT( IsCoreType( typeID ) && pValue != nullptr );
            strValue.clear();

            if ( typeID == CoreTypes::Bool )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(bool*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Uint8 )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(uint8*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Int8 )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(int8*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Uint16 )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(uint16*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Int16 )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(int16*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Uint32 )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(uint32*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Int32 )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(int32*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Uint64 )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(uint64*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Int64 )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(int64*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Float )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(float*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Double )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(double*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::String )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(String*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(StringID*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::UUID )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(UUID*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Color )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Color*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                return  CoreTypeSerializer::TypeValueToStringValue( *(Float2*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Float3*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Float4*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Transform*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(EulerAngles*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Microseconds )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Microseconds*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Milliseconds )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Milliseconds*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Seconds )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Seconds*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Percentage )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Percentage*) pValue, strValue );
            }
            else  if ( typeID == CoreTypes::Degrees )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Degrees*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Radians )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Radians*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Quaternion*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Matrix*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(DataPath*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(ResourceTypeID*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::ResourcePtr || typeID == CoreTypes::TResourcePtr )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(Resource::ResourcePtr*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::ResourceID )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(ResourceID*) pValue, strValue );
            }
            else if ( typeID == CoreTypes::BitFlags )
            {
                CoreTypeSerializer::TypeValueToStringValue( *(BitFlags*) pValue, strValue );
            }
        }
    }

    void ConvertTypeValueToBinary( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, void const* pValue, TVector<Byte>& byteArray )
    {
        KRG_ASSERT( !byteArray.empty() && pValue != nullptr );

        if ( !IsCoreType( typeID ) ) // Enums have unique typeIDs
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( typeID );
            KRG_ASSERT( pEnumInfo != nullptr );

            // We only support up to 32 bit enum types...
            switch ( pEnumInfo->m_underlyingType )
            {
                case CoreTypes::Uint8:
                {
                    CoreTypeSerializer::TypeValueToBinary( *(uint8*) pValue, byteArray );
                }
                break;

                case CoreTypes::Int8:
                {
                    CoreTypeSerializer::TypeValueToBinary( *(int8*) pValue, byteArray );
                }
                break;

                case CoreTypes::Uint16:
                {
                    CoreTypeSerializer::TypeValueToBinary( *(uint16*) pValue, byteArray );
                }
                break;

                case CoreTypes::Int16:
                {
                    CoreTypeSerializer::TypeValueToBinary( *(int16*) pValue, byteArray );
                }
                break;

                case CoreTypes::Uint32:
                {
                    CoreTypeSerializer::TypeValueToBinary( *(uint32*) pValue, byteArray );
                }
                break;

                case CoreTypes::Int32:
                {
                    CoreTypeSerializer::TypeValueToBinary( *(int32*) pValue, byteArray );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
        else // Real core types
        {
            if ( typeID == CoreTypes::Bool )
            {
                CoreTypeSerializer::TypeValueToBinary( *(bool*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Uint8 )
            {
                CoreTypeSerializer::TypeValueToBinary( *(uint8*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Int8 )
            {
                CoreTypeSerializer::TypeValueToBinary( *(int8*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Uint16 )
            {
                CoreTypeSerializer::TypeValueToBinary( *(uint16*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Int16 )
            {
                CoreTypeSerializer::TypeValueToBinary( *(int16*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Uint32 )
            {
                CoreTypeSerializer::TypeValueToBinary( *(uint32*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Int32 )
            {
                CoreTypeSerializer::TypeValueToBinary( *(int32*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Uint64 )
            {
                CoreTypeSerializer::TypeValueToBinary( *(uint64*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Int64 )
            {
                CoreTypeSerializer::TypeValueToBinary( *(int64*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float )
            {
                CoreTypeSerializer::TypeValueToBinary( *(float*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Double )
            {
                CoreTypeSerializer::TypeValueToBinary( *(double*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::String )
            {
                CoreTypeSerializer::TypeValueToBinary( *(String*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                CoreTypeSerializer::TypeValueToBinary( *(StringID*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::UUID )
            {
                CoreTypeSerializer::TypeValueToBinary( *(UUID*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Color )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Color*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Float2*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Float3*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Float4*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Transform*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                CoreTypeSerializer::TypeValueToBinary( *(EulerAngles*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Microseconds )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Microseconds*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Milliseconds )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Milliseconds*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Seconds )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Seconds*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Percentage )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Percentage*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Degrees )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Degrees*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Radians )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Radians*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Quaternion*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Matrix*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                CoreTypeSerializer::TypeValueToBinary( *(DataPath*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                CoreTypeSerializer::TypeValueToBinary( *(ResourceTypeID*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourcePtr )
            {
                CoreTypeSerializer::TypeValueToBinary( *(Resource::ResourcePtr*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::TResourcePtr )
            {
                CoreTypeSerializer::TypeValueToBinary( *( TResourcePtr<Resource::IResource>* ) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourceID )
            {
                CoreTypeSerializer::TypeValueToBinary( *(ResourceID*) pValue, byteArray );
            }
            else if ( typeID == CoreTypes::BitFlags || typeID == CoreTypes::TBitFlags )
            {
                CoreTypeSerializer::TypeValueToBinary( *(BitFlags*) pValue, byteArray );
            }
            else
            {
                KRG_UNREACHABLE_CODE();
            }
        }
    }

    void ConvertBinaryToTypeValue( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, TVector<Byte> const& byteArray, void* pValue )
    {
        KRG_ASSERT( !byteArray.empty() && pValue != nullptr );

        if ( !IsCoreType( typeID ) ) // Enums have unique typeIDs
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( typeID );
            KRG_ASSERT( pEnumInfo != nullptr );

            // We only support up to 32 bit enum types...
            switch ( pEnumInfo->m_underlyingType )
            {
                case CoreTypes::Uint8:
                {
                    CoreTypeSerializer::BinaryToTypeValue( byteArray, *( (uint8*) pValue ) );
                }
                break;

                case CoreTypes::Int8:
                {
                    CoreTypeSerializer::BinaryToTypeValue( byteArray, *( (uint8*) pValue ) );
                }
                break;

                case CoreTypes::Uint16:
                {
                    CoreTypeSerializer::BinaryToTypeValue( byteArray, *( (uint8*) pValue ) );
                }
                break;

                case CoreTypes::Int16:
                {
                    CoreTypeSerializer::BinaryToTypeValue( byteArray, *( (uint8*) pValue ) );
                }
                break;

                case CoreTypes::Uint32:
                {
                    CoreTypeSerializer::BinaryToTypeValue( byteArray, *( (uint8*) pValue ) );
                }
                break;

                case CoreTypes::Int32:
                {
                    CoreTypeSerializer::BinaryToTypeValue( byteArray, *( (uint8*) pValue ) );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
        else // Real core types
        {
            if ( typeID == CoreTypes::Bool )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(bool*) pValue );
            }
            else if ( typeID == CoreTypes::Uint8 )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(uint8*) pValue );
            }
            else if ( typeID == CoreTypes::Int8 )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(int8*) pValue );
            }
            else if ( typeID == CoreTypes::Uint16 )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(uint16*) pValue );
            }
            else if ( typeID == CoreTypes::Int16 )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(int16*) pValue );
            }
            else if ( typeID == CoreTypes::Uint32 )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(uint32*) pValue );
            }
            else if ( typeID == CoreTypes::Int32 )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(int32*) pValue );
            }
            else if ( typeID == CoreTypes::Uint64 )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(uint64*) pValue );
            }
            else if ( typeID == CoreTypes::Int64 )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(int64*) pValue );
            }
            else if ( typeID == CoreTypes::Float )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(float*) pValue );
            }
            else if ( typeID == CoreTypes::Double )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(double*) pValue );
            }
            else if ( typeID == CoreTypes::String )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(String*) pValue );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(StringID*) pValue );
            }
            else if ( typeID == CoreTypes::UUID )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(UUID*) pValue );
            }
            else if ( typeID == CoreTypes::Color )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Color*) pValue );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Float2*) pValue );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Float3*) pValue );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Float4*) pValue );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Transform*) pValue );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(EulerAngles*) pValue );
            }
            else if ( typeID == CoreTypes::Microseconds )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Microseconds*) pValue );
            }
            else if ( typeID == CoreTypes::Milliseconds )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Milliseconds*) pValue );
            }
            else if ( typeID == CoreTypes::Seconds )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Seconds*) pValue );
            }
            else if ( typeID == CoreTypes::Percentage )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Percentage*) pValue );
            }
            else if ( typeID == CoreTypes::Degrees )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Degrees*) pValue );
            }
            else if ( typeID == CoreTypes::Radians )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Radians*) pValue );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Quaternion*) pValue );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Matrix*) pValue );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(DataPath*) pValue );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(ResourceTypeID*) pValue );
            }
            else if ( typeID == CoreTypes::ResourcePtr )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(Resource::ResourcePtr*) pValue );
            }
            else if ( typeID == CoreTypes::TResourcePtr )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *( TResourcePtr<Resource::IResource>* ) pValue );
            }
            else if ( typeID == CoreTypes::ResourceID )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(ResourceID*) pValue );
            }
            else if ( typeID == CoreTypes::BitFlags || typeID == CoreTypes::TBitFlags )
            {
                CoreTypeSerializer::BinaryToTypeValue( byteArray, *(BitFlags*) pValue );
            }
            else
            {
                KRG_UNREACHABLE_CODE();
            }
        }
    }

    void ConvertStringValueToBinary( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue, TVector<Byte>& byteArray )
    {
        KRG_ASSERT( !strValue.empty() );
        byteArray.clear();

        // Special cases: Enums and TBitFlags
        if ( typeID == CoreTypes::TBitFlags )
        {
            BitFlags flags;
            ConvertStringValueToTypeValue( typeRegistry, typeID, templateArgumentTypeID, strValue, &flags );
            CoreTypeSerializer::TypeValueToBinary( flags, byteArray );
        }
        else if ( !IsCoreType( typeID ) ) // Enums have unique typeIDs
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( typeID );
            KRG_ASSERT( pEnumInfo != nullptr );

            StringID const enumID( strValue );
            int64 const enumValue = pEnumInfo->GetConstantValue( enumID );

            // We only support up to 32 bit enum types...
            switch ( pEnumInfo->m_underlyingType )
            {
                case CoreTypes::Uint8:
                {
                    CoreTypeSerializer::TypeValueToBinary( (uint8) enumValue, byteArray );
                }
                break;

                case CoreTypes::Int8:
                {
                    CoreTypeSerializer::TypeValueToBinary( (int8) enumValue, byteArray );
                }
                break;

                case CoreTypes::Uint16:
                {
                    CoreTypeSerializer::TypeValueToBinary( (uint16) enumValue, byteArray );
                }
                break;

                case CoreTypes::Int16:
                {
                    CoreTypeSerializer::TypeValueToBinary( (int16) enumValue, byteArray );
                }
                break;

                case CoreTypes::Uint32:
                {
                    CoreTypeSerializer::TypeValueToBinary( (uint32) enumValue, byteArray );
                }
                break;

                case CoreTypes::Int32:
                {
                    CoreTypeSerializer::TypeValueToBinary( (int32) enumValue, byteArray );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
        else // Real core types
        {
            if ( typeID == CoreTypes::Bool )
            {
                CoreTypeSerializer::StringToBinary<bool>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Uint8 )
            {
                CoreTypeSerializer::StringToBinary<uint8>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Int8 )
            {
                CoreTypeSerializer::StringToBinary<int8>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Uint16 )
            {
                CoreTypeSerializer::StringToBinary<uint16>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Int16 )
            {
                CoreTypeSerializer::StringToBinary<int16>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Uint32 )
            {
                CoreTypeSerializer::StringToBinary<uint32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Int32 )
            {
                CoreTypeSerializer::StringToBinary<int32>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Uint64 )
            {
                CoreTypeSerializer::StringToBinary<uint64>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Int64 )
            {
                CoreTypeSerializer::StringToBinary<int64>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float )
            {
                CoreTypeSerializer::StringToBinary<float>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Double )
            {
                CoreTypeSerializer::StringToBinary<double>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::String )
            {
                CoreTypeSerializer::StringToBinary<String>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::StringID )
            {
                CoreTypeSerializer::StringToBinary<StringID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::UUID )
            {
                CoreTypeSerializer::StringToBinary<UUID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Color )
            {
                CoreTypeSerializer::StringToBinary<Color>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float2 )
            {
                CoreTypeSerializer::StringToBinary<Float2>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float3 )
            {
                CoreTypeSerializer::StringToBinary<Float3>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Float4 )
            {
                CoreTypeSerializer::StringToBinary<Float4>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Transform )
            {
                CoreTypeSerializer::StringToBinary<Transform>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::EulerAngles )
            {
                CoreTypeSerializer::StringToBinary<EulerAngles>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Microseconds )
            {
                CoreTypeSerializer::StringToBinary<Microseconds>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Milliseconds )
            {
                CoreTypeSerializer::StringToBinary<Milliseconds>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Seconds )
            {
                CoreTypeSerializer::StringToBinary<Seconds>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Percentage )
            {
                CoreTypeSerializer::StringToBinary<Percentage>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Degrees )
            {
                CoreTypeSerializer::StringToBinary<Degrees>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Radians )
            {
                CoreTypeSerializer::StringToBinary<Radians>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Quaternion )
            {
                CoreTypeSerializer::StringToBinary<Quaternion>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::Matrix )
            {
                CoreTypeSerializer::StringToBinary<Matrix>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::DataPath )
            {
                CoreTypeSerializer::StringToBinary<DataPath>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourceTypeID )
            {
                CoreTypeSerializer::StringToBinary<ResourceTypeID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourcePtr || typeID == CoreTypes::TResourcePtr )
            {
                CoreTypeSerializer::StringToBinary<Resource::ResourcePtr>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::ResourceID )
            {
                CoreTypeSerializer::StringToBinary<ResourceID>( strValue, byteArray );
            }
            else if ( typeID == CoreTypes::BitFlags )
            {
                CoreTypeSerializer::StringToBinary<BitFlags>( strValue, byteArray );
            }
            else
            {
                KRG_UNREACHABLE_CODE();
            }
        }
    }

    bool IsValidStringValueForType( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue )
    {
        // Special cases: Enums and TBitFlags
        if ( typeID == CoreTypes::TBitFlags )
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( templateArgumentTypeID );
            KRG_ASSERT( pEnumInfo != nullptr );
            BitFlags flags;
            return ConvertStringToBitFlags( *pEnumInfo, strValue, flags );
        }
        else if ( !IsCoreType( typeID ) ) // Enums has unique typeIDs
        {
            auto const pEnumInfo = typeRegistry.GetEnumInfo( typeID );
            KRG_ASSERT( pEnumInfo != nullptr );
            return pEnumInfo->IsValidValue( StringID( strValue ) );
        }
        else // Real core types
        {
            // TODO
            return true;
        }
    }
}