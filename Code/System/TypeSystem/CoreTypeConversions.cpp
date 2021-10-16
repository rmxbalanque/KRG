#include "CoreTypeConversions.h"
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

namespace KRG::TypeSystem::Conversion
{
    void StringToFloatArray( String const& str, int32 const numFloats, float* pFloats )
    {
        char substr[128] = { 0 };
        int32 resIdx = 0;

        bool complete = false;
        size_t startIdx = 0;
        while ( resIdx < numFloats && !complete )
        {
            size_t endIdx = str.find_first_of( ',', startIdx );
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
            startIdx = endIdx + 1;
        }
    }

    void FloatArrayToString( float const* pFloats, int32 const numFloats, String& strValue )
    {
        strValue.clear();

        for ( int32 i = 0; i < numFloats; i++ )
        {
            strValue += eastl::to_string( pFloats[i] );

            if ( i != ( numFloats - 1 ) )
            {
                strValue += ',';
            }
        }
    }

    void StringToIntArray( String const& str, int32 const numInts, int32* pInts )
    {
        char substr[128] = { 0 };
        int32 resIdx = 0;

        bool complete = false;
        size_t startIdx = 0;
        while ( resIdx < numInts && !complete )
        {
            size_t endIdx = str.find_first_of( ',', startIdx );
            if ( endIdx == String::npos )
            {
                endIdx = str.length();
                complete = true;
            }

            size_t sizeToCopy = endIdx - startIdx;
            KRG_ASSERT( sizeToCopy < 128 );
            memcpy( substr, &str.c_str()[startIdx], sizeToCopy );
            substr[sizeToCopy] = '\0';

            pInts[resIdx++] = std::strtoul( substr, nullptr, 0 );
            startIdx = endIdx + 1;
        }
    }

    void IntArrayToString( int32 const* pInts, int32 const numInts, String& strValue )
    {
        strValue.clear();

        for ( int32 i = 0; i < numInts; i++ )
        {
            strValue += eastl::to_string( pInts[i] );

            if ( i != ( numInts - 1 ) )
            {
                strValue += ',';
            }
        }
    }

    inline static bool ConvertStringToBitFlags( EnumInfo const& enumInfo, String const& str, BitFlags& outFlags )
    {
        outFlags.ClearAllFlags();

        if ( str.empty() )
        {
            return true;
        }

        // Handle hex format explicitly
        //-------------------------------------------------------------------------

        if ( str.find_first_of( "0x" ) == 0 || str.find_first_of( "0X" ) == 0 )
        {
            uint32 value = std::strtoul( str.c_str(), nullptr, 16 );
            outFlags.Set( value );
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

    inline static bool ConvertBitFlagsToString( EnumInfo const& enumInfo, BitFlags const& flags, String& strValue )
    {
        strValue.clear();

        for ( auto i = 0u; i < 32; i++ )
        {
            if ( flags.IsFlagSet( (uint8) i ) )
            {
                StringID label;
                if ( !enumInfo.TryGetConstantLabel( i, label ) )
                {
                    strValue.clear();
                    return false;
                }

                strValue += label.ToString();
                strValue += "|";
            }
        }

        //-------------------------------------------------------------------------

        if ( !strValue.empty() )
        {
            strValue.erase( strValue.end() - 1, strValue.end() );
        }

        return true;
    }

    //-------------------------------------------------------------------------

    void ConvertStringToNativeType( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& str, void* pValue )
    {
        // Enums
        if( !IsCoreType( typeID ) ) 
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( typeID );
            KRG_ASSERT( pEnumInfo != nullptr );
            
            StringID const enumID( str );
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
            CoreTypes const typeToConvert = GetCoreType( typeID );
            switch ( typeToConvert )
            {
                case CoreTypes::Bool :
                {
                    String lowerString = str;
                    lowerString.make_lower();
                    *reinterpret_cast<bool*>( pValue ) = ( lowerString == "true" );
                }
                break;

                case CoreTypes::Uint8 :
                {
                    *reinterpret_cast<uint8*>( pValue ) = (uint8) std::strtoul( str.c_str(), nullptr, 0 );
                }
                break;

                case CoreTypes::Int8 :
                {
                    *reinterpret_cast<int8*>( pValue ) = (int8) std::strtol( str.c_str(), nullptr, 0 );
                }
                break;

                case CoreTypes::Uint16 :
                {
                    *reinterpret_cast<uint16*>( pValue ) = (uint16) std::strtoul( str.c_str(), nullptr, 0 );
                }
                break;

                case CoreTypes::Int16 :
                {
                    *reinterpret_cast<int16*>( pValue ) = (int16) std::strtol( str.c_str(), nullptr, 0 );
                }
                break;

                case CoreTypes::Uint32 :
                {
                    *reinterpret_cast<uint32*>( pValue ) = std::strtoul( str.c_str(), nullptr, 0 );
                }
                break;

                case CoreTypes::Int32 :
                {
                    *reinterpret_cast<int32*>( pValue ) = std::strtol( str.c_str(), nullptr, 0 );
                }
                break;

                case CoreTypes::Uint64 :
                {
                    *reinterpret_cast<uint64*>( pValue ) = std::strtoull( str.c_str(), nullptr, 0 );
                }
                break;

                case CoreTypes::Int64:
                {
                    *reinterpret_cast<int64*>( pValue ) = std::strtol( str.c_str(), nullptr, 0 );
                }
                break;

                case CoreTypes::Float :
                {
                    *reinterpret_cast<float*>( pValue ) = std::strtof( str.c_str(), nullptr );
                }
                break;

                case CoreTypes::Double :
                {
                    *reinterpret_cast<double*>( pValue ) = std::strtod( str.c_str(), nullptr );
                }
                break;

                case CoreTypes::String :
                {
                    *reinterpret_cast<String*>( pValue ) = str;
                }
                break;

                case CoreTypes::StringID :
                {
                    *reinterpret_cast<StringID*>( pValue ) = StringID( str.c_str() );
                }
                break;

                case CoreTypes::TypeID:
                {
                    *reinterpret_cast<TypeID*>( pValue ) = TypeID( str.c_str() );
                }
                break;

                case CoreTypes::UUID :
                {
                    *reinterpret_cast<UUID*>( pValue ) = UUID( str );
                }
                break;

                case CoreTypes::Color :
                {
                    uint32 const colorType = std::strtoul( str.c_str(), nullptr, 16 );
                    *reinterpret_cast<Color*>( pValue ) = Color( colorType );
                }
                break;

                case CoreTypes::Float2 :
                {
                    StringToFloatArray( str, 2, &reinterpret_cast<Float2*>( pValue )->m_x );
                }
                break;

                case CoreTypes::Float3:
                {
                    StringToFloatArray( str, 3, &reinterpret_cast<Float3*>( pValue )->m_x );
                }
                break;

                case CoreTypes::Float4:
                {
                    StringToFloatArray( str, 4, &reinterpret_cast<Float4*>( pValue )->m_x );
                }
                break;

                case CoreTypes::Vector:
                {
                    StringToFloatArray( str, 4, &reinterpret_cast<Vector*>( pValue )->m_x );
                }
                break;

                case CoreTypes::Quaternion:
                {
                    StringToFloatArray( str, 4, &reinterpret_cast<Quaternion*>( pValue )->m_x );
                }
                break;

                case CoreTypes::Matrix:
                {
                    float floatData[9];
                    StringToFloatArray( str, 9, floatData );

                    EulerAngles const rotation( floatData[0], floatData[1], floatData[2] );
                    Vector const translation = Vector( floatData[3], floatData[4], floatData[5] );
                    Vector const scale = Vector( floatData[6], floatData[7], floatData[8] );
                    *reinterpret_cast<Matrix*>( pValue ) = Matrix( Quaternion( rotation ), translation, scale );
                }
                break;

                case CoreTypes::Transform:
                {
                    float floatData[9];
                    StringToFloatArray( str, 9, floatData );

                    EulerAngles const rotation( floatData[0], floatData[1], floatData[2] );
                    Vector const translation( floatData[3], floatData[4], floatData[5] );
                    Vector const scale( floatData[6], floatData[7], floatData[8] );
                    *reinterpret_cast<Transform*>( pValue ) = Transform( Quaternion( rotation ), translation, scale );
                }
                break;

                case CoreTypes::EulerAngles:
                {
                    Float3 angles;
                    StringToFloatArray( str, 3, &angles.m_x );
                    *reinterpret_cast<EulerAngles*>( pValue ) = EulerAngles( angles.m_x, angles.m_y, angles.m_z );
                }
                break;

                case CoreTypes::Microseconds:
                {
                    *reinterpret_cast<Microseconds*>( pValue ) = Microseconds( std::strtof( str.c_str(), nullptr ) );
                }
                break;

                case CoreTypes::Milliseconds:
                {
                    *reinterpret_cast<Milliseconds*>( pValue ) = Milliseconds( std::strtof( str.c_str(), nullptr ) );
                }
                break;

                case CoreTypes::Seconds:
                {
                    *reinterpret_cast<Seconds*>( pValue ) = Seconds( std::strtof( str.c_str(), nullptr ) );
                }
                break;

                case CoreTypes::Percentage:
                {
                    *reinterpret_cast<Percentage*>( pValue ) = Percentage( std::strtof( str.c_str(), nullptr ) );
                }
                break;

                case CoreTypes::Degrees:
                {
                    *reinterpret_cast<Degrees*>( pValue ) = Degrees( std::strtof( str.c_str(), nullptr ) );
                }
                break;

                case CoreTypes::Radians:
                {
                    *reinterpret_cast<Radians*>( pValue ) = Radians( std::strtof( str.c_str(), nullptr ) );
                }
                break;

                case CoreTypes::ResourcePath:
                {
                    *reinterpret_cast<ResourcePath*>( pValue ) = ResourcePath( str );
                }
                break;

                case CoreTypes::IntRange:
                {
                    int32 intData[2];
                    StringToIntArray( str, 2, intData );
                    *reinterpret_cast<IntRange*>( pValue ) = IntRange( intData[0], intData[1] );
                }
                break;

                case CoreTypes::FloatRange:
                {
                    float floatData[2];
                    StringToFloatArray( str, 2, floatData );
                    *reinterpret_cast<FloatRange*>( pValue ) = FloatRange( floatData[0], floatData[1] );
                }
                break;

                case CoreTypes::ResourceTypeID:
                {
                    *reinterpret_cast<ResourceTypeID*>( pValue ) = ResourceTypeID( str );
                }
                break;

                case CoreTypes::ResourcePtr:
                case CoreTypes::TResourcePtr:
                {
                    Resource::ResourcePtr& resourcePtr = *reinterpret_cast<Resource::ResourcePtr*>( pValue );
                    if ( str.empty() )
                    {
                        resourcePtr = Resource::ResourcePtr();
                    }
                    else
                    {
                        KRG_ASSERT( ResourceID::IsValidResourceIDString( str ) );
                        ResourceID const ID( str );
                        resourcePtr = Resource::ResourcePtr( ID );
                    }
                }
                break;

                case CoreTypes::ResourceID:
                {
                    if ( str.empty() )
                    {
                        *reinterpret_cast<ResourceID*>( pValue ) = ResourceID();
                    }
                    else
                    {
                        KRG_ASSERT( ResourceID::IsValidResourceIDString( str ) );
                        *reinterpret_cast<ResourceID*>( pValue ) = ResourceID( str );
                    }
                }
                break;

                case CoreTypes::BitFlags:
                {
                    reinterpret_cast<BitFlags*>( pValue )->Set( std::strtol( str.c_str(), nullptr, 0 ) );
                }
                break;

                case CoreTypes::TBitFlags:
                {
                    EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( templateArgumentTypeID );
                    if ( pEnumInfo == nullptr )
                    {
                        KRG_LOG_WARNING( "TypeSystem", "Unknown enum class (%s) for TBitFlags", templateArgumentTypeID.ToStringID().c_str() );
                        return;
                    }

                    if ( !ConvertStringToBitFlags( *pEnumInfo, str, *reinterpret_cast<BitFlags*>( pValue ) ) )
                    {
                        KRG_LOG_WARNING( "TypeSystem", "Failed to convert string (%s) into valid TBitFlags<%s>", str.c_str(), templateArgumentTypeID.ToStringID().c_str() );
                    }
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
    }

    void ConvertNativeTypeToString( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, void const* pValue, String & strValue )
    {
        // Enums
        if ( !IsCoreType( typeID ) )
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( typeID );
            KRG_ASSERT( pEnumInfo != nullptr );

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
        else  // Real core types
        {
            CoreTypes const typeToConvert = GetCoreType( typeID );
            switch ( typeToConvert )
            {
                case CoreTypes::Bool:
                {
                    strValue = *reinterpret_cast<bool const*>( pValue ) ? "True" : "False";
                }
                break;

                case CoreTypes::Uint8:
                {
                    strValue = eastl::to_string( *reinterpret_cast<uint8 const*>( pValue ) );
                }
                break;

                case CoreTypes::Int8:
                {
                    strValue = eastl::to_string( *reinterpret_cast<int8 const*>( pValue ) );
                }
                break;

                case CoreTypes::Uint16:
                {
                    strValue = eastl::to_string( *reinterpret_cast<uint16 const*>( pValue ) );
                }
                break;

                case CoreTypes::Int16:
                {
                    strValue = eastl::to_string( *reinterpret_cast<int16 const*>( pValue ) );
                }
                break;

                case CoreTypes::Uint32:
                {
                    strValue = eastl::to_string( *reinterpret_cast<uint32 const*>( pValue ) );
                }
                break;

                case CoreTypes::Int32:
                {
                    strValue = eastl::to_string( *reinterpret_cast<int32 const*>( pValue ) );
                }
                break;

                case CoreTypes::Uint64:
                {
                    strValue = eastl::to_string( *reinterpret_cast<uint64 const*>( pValue ) );
                }
                break;

                case CoreTypes::Int64:
                {
                    strValue = eastl::to_string( *reinterpret_cast<int64 const*>( pValue ) );
                }
                break;

                case CoreTypes::Float:
                {
                    strValue = eastl::to_string( *reinterpret_cast<float const*>( pValue ) );
                }
                break;

                case CoreTypes::Double:
                {
                    strValue = eastl::to_string( *reinterpret_cast<double const*>( pValue ) );
                }
                break;

                case CoreTypes::String:
                {
                    strValue = *reinterpret_cast<String const*>( pValue );
                }
                break;

                case CoreTypes::StringID:
                {
                    char const* pStr = reinterpret_cast<StringID const*>( pValue )->ToString();
                    KRG_ASSERT( pStr != nullptr );
                    strValue = pStr;
                }
                break;

                case CoreTypes::TypeID:
                {
                    char const* pStr = reinterpret_cast<TypeID const*>( pValue )->ToStringID().c_str();
                    KRG_ASSERT( pStr != nullptr );
                    strValue = pStr;
                }
                break;

                case CoreTypes::UUID:
                {
                    strValue = reinterpret_cast<UUID const*>( pValue )->ToString();
                }
                break;

                case CoreTypes::Color:
                {
                    Color const& value = *reinterpret_cast<Color const*>( pValue );
                    strValue.sprintf( "%02X%02X%02X%02X", value.m_byteColor.m_r, value.m_byteColor.m_g, value.m_byteColor.m_b, value.m_byteColor.m_a );
                }
                break;

                case CoreTypes::Float2:
                {
                    FloatArrayToString( &reinterpret_cast<Float2 const*>( pValue )->m_x, 2, strValue );
                }
                break;

                case CoreTypes::Float3:
                {
                    FloatArrayToString( &reinterpret_cast<Float3 const*>( pValue )->m_x, 3, strValue );
                }
                break;

                case CoreTypes::Float4:
                {
                    FloatArrayToString( &reinterpret_cast<Float4 const*>( pValue )->m_x, 4, strValue );
                }
                break;

                case CoreTypes::Vector:
                {
                    FloatArrayToString( &reinterpret_cast<Vector const*>( pValue )->m_x, 4, strValue );
                }
                break;

                case CoreTypes::Quaternion:
                {
                    FloatArrayToString( &reinterpret_cast<Quaternion const*>( pValue )->m_x, 4, strValue );
                }
                break;

                case CoreTypes::Matrix:
                {
                    Matrix const& value = *reinterpret_cast<Matrix const*>( pValue );

                    // Handle uninitialized matrix
                    if ( value.IsOrthonormal() )
                    {
                        auto eulerAngles = value.ToEulerAngles();

                        float floatData[9];
                        (Float3&) floatData = Float3( (float) eulerAngles.m_x.ToDegrees(), (float) eulerAngles.m_y.ToDegrees(), (float) eulerAngles.m_z.ToDegrees() );
                        (Float3&) floatData[3] = value.GetTranslation().ToFloat3();
                        (Float3&) floatData[6] = value.GetScale().ToFloat3();

                        FloatArrayToString( floatData, 9, strValue );
                    }
                    else
                    {
                        strValue = "0,0,0,0,0,0,0,0,0";
                    }
                }
                break;

                case CoreTypes::Transform:
                {
                    Transform const& transform = *reinterpret_cast<Transform const*>( pValue );
                    auto eulerAngles = transform.GetRotation().ToEulerAngles();

                    float floatData[9];

                    floatData[0] = (float) eulerAngles.m_x.ToDegrees();
                    floatData[1] = (float) eulerAngles.m_y.ToDegrees();
                    floatData[2] = (float) eulerAngles.m_z.ToDegrees();

                    floatData[3] = transform.GetTranslation().m_x;
                    floatData[4] = transform.GetTranslation().m_y;
                    floatData[5] = transform.GetTranslation().m_z;

                    floatData[6] = transform.GetScale().m_x;
                    floatData[7] = transform.GetScale().m_y;
                    floatData[8] = transform.GetScale().m_z;

                    //-------------------------------------------------------------------------

                    FloatArrayToString( floatData, 9, strValue );
                }
                break;

                case CoreTypes::EulerAngles:
                {
                    Float3 const angles = reinterpret_cast<EulerAngles const*>( pValue )->GetAsDegrees();
                    FloatArrayToString( &angles.m_x, 3, strValue );
                }
                break;

                case CoreTypes::Microseconds:
                {
                    strValue = eastl::to_string( reinterpret_cast<Microseconds const*>( pValue )->ToFloat() );
                }
                break;

                case CoreTypes::Milliseconds:
                {
                    strValue = eastl::to_string( reinterpret_cast<Milliseconds const*>( pValue )->ToFloat() );
                }
                break;

                case CoreTypes::Seconds:
                {
                    strValue = eastl::to_string( reinterpret_cast<Seconds const*>( pValue )->ToFloat() );
                }
                break;

                case CoreTypes::Percentage:
                {
                    strValue = eastl::to_string( reinterpret_cast<Percentage const*>( pValue )->ToFloat() );
                }
                break;

                case CoreTypes::Degrees:
                {
                    strValue = eastl::to_string( reinterpret_cast<Degrees const*>( pValue )->ToFloat() );
                }
                break;

                case CoreTypes::Radians:
                {
                    strValue = eastl::to_string( reinterpret_cast<Radians const*>( pValue )->ToFloat() );
                }
                break;

                case CoreTypes::ResourcePath:
                {
                    strValue = reinterpret_cast<ResourcePath const*>( pValue )->c_str();
                }
                break;

                case CoreTypes::IntRange:
                {
                    IntRange const* pRange = reinterpret_cast<IntRange const*>( pValue );
                    IntArrayToString( &pRange->m_start, 3, strValue );
                }
                break;

                case CoreTypes::FloatRange:
                {
                    FloatRange const* pRange = reinterpret_cast<FloatRange const*>( pValue );
                    FloatArrayToString( &pRange->m_start, 3, strValue );
                }
                break;

                case CoreTypes::ResourceTypeID:
                {
                    strValue = reinterpret_cast<ResourceTypeID const*>( pValue )->ToString();
                }
                break;

                case CoreTypes::ResourcePtr:
                case CoreTypes::TResourcePtr:
                {
                    strValue = reinterpret_cast<Resource::ResourcePtr const*>( pValue )->GetResourceID().ToString();
                }
                break;

                case CoreTypes::ResourceID:
                {
                    strValue = reinterpret_cast<ResourceID const*>( pValue )->ToString();
                }
                break;

                case CoreTypes::BitFlags:
                {
                    strValue = eastl::to_string( reinterpret_cast<BitFlags const*>( pValue )->Get() );
                }
                break;

                case CoreTypes::TBitFlags:
                {
                    EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( templateArgumentTypeID );
                    if ( pEnumInfo == nullptr )
                    {
                        KRG_LOG_WARNING( "TypeSystem", "Unknown enum class (%s) for TBitFlags", templateArgumentTypeID.ToStringID().c_str() );
                        return;
                    }

                    if ( !ConvertBitFlagsToString( *pEnumInfo, *reinterpret_cast<BitFlags const*>( pValue ), strValue ) )
                    {
                        KRG_LOG_WARNING( "TypeSystem", "Failed to convert string (%s) into valid TBitFlags<%s>", strValue.c_str(), templateArgumentTypeID.ToStringID().c_str() );
                    }
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
    }

    void ConvertNativeTypeToBinary( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, void const* pValue, TVector<Byte>& byteArray )
    {
        Serialization::BinaryMemoryArchive archive( Serialization::Mode::Write, byteArray );

        // Enums
        if ( !IsCoreType( typeID ) )
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( typeID );
            KRG_ASSERT( pEnumInfo != nullptr );

            // We only support up to 32 bit enum types...
            switch ( pEnumInfo->m_underlyingType )
            {
                case CoreTypes::Uint8:
                {
                    archive << *reinterpret_cast<uint8 const*>( pValue );
                }
                break;

                case CoreTypes::Int8:
                {
                    archive << *reinterpret_cast<int8 const*>( pValue );
                }
                break;

                case CoreTypes::Uint16:
                {
                    archive << *reinterpret_cast<uint16 const*>( pValue );
                }
                break;

                case CoreTypes::Int16:
                {
                    archive << *reinterpret_cast<int16 const*>( pValue );
                }
                break;

                case CoreTypes::Uint32:
                {
                    archive << *reinterpret_cast<uint32 const*>( pValue );
                }
                break;

                case CoreTypes::Int32:
                {
                    archive << *reinterpret_cast<int32 const*>( pValue );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
        else  // Real core types
        {
            CoreTypes const typeToConvert = GetCoreType( typeID );
            switch ( typeToConvert )
            {
                case CoreTypes::Bool:
                {
                    archive << *reinterpret_cast<bool const*>( pValue );
                }
                break;

                case CoreTypes::Uint8:
                {
                    archive << *reinterpret_cast<uint8 const*>( pValue );
                }
                break;

                case CoreTypes::Int8:
                {
                    archive << *reinterpret_cast<int8 const*>( pValue );
                }
                break;

                case CoreTypes::Uint16:
                {
                    archive << *reinterpret_cast<uint16 const*>( pValue );
                }
                break;

                case CoreTypes::Int16:
                {
                    archive << *reinterpret_cast<int16 const*>( pValue );
                }
                break;

                case CoreTypes::Uint32:
                {
                    archive << *reinterpret_cast<uint32 const*>( pValue );
                }
                break;

                case CoreTypes::Int32:
                {
                    archive << *reinterpret_cast<int32 const*>( pValue );
                }
                break;

                case CoreTypes::Uint64:
                {
                    archive << *reinterpret_cast<uint64 const*>( pValue );
                }
                break;

                case CoreTypes::Int64:
                {
                    archive << *reinterpret_cast<int64 const*>( pValue );
                }
                break;

                case CoreTypes::Float:
                {
                    archive << *reinterpret_cast<float const*>( pValue );
                }
                break;

                case CoreTypes::Double:
                {
                    archive << *reinterpret_cast<double const*>( pValue );
                }
                break;

                case CoreTypes::String:
                {
                    archive << *reinterpret_cast<String const*>( pValue );
                }
                break;

                case CoreTypes::StringID:
                {
                    archive << *reinterpret_cast<StringID const*>( pValue );
                }
                break;

                case CoreTypes::TypeID:
                {
                    archive << reinterpret_cast<TypeID const*>( pValue )->ToStringID();
                }
                break;

                case CoreTypes::UUID:
                {
                    archive << *reinterpret_cast<UUID const*>( pValue );
                }
                break;

                case CoreTypes::Color:
                {
                    archive << *reinterpret_cast<Color const*>( pValue );
                }
                break;

                case CoreTypes::Float2:
                {
                    archive << *reinterpret_cast<Float2 const*>( pValue );
                }
                break;

                case CoreTypes::Float3:
                {
                    archive << *reinterpret_cast<Float3 const*>( pValue );
                }
                break;

                case CoreTypes::Float4:
                {
                    archive << *reinterpret_cast<Float4 const*>( pValue );
                }
                break;

                case CoreTypes::Vector:
                {
                    archive << *reinterpret_cast<Vector const*>( pValue );
                }
                break;

                case CoreTypes::Quaternion:
                {
                    archive << *reinterpret_cast<Quaternion const*>( pValue );
                }
                break;

                case CoreTypes::Matrix:
                {
                    archive << *reinterpret_cast<Matrix const*>( pValue );
                }
                break;

                case CoreTypes::Transform:
                {
                    archive << *reinterpret_cast<Transform const*>( pValue );
                }
                break;

                case CoreTypes::EulerAngles:
                {
                    archive << *reinterpret_cast<EulerAngles const*>( pValue );
                }
                break;

                case CoreTypes::Microseconds:
                {
                    archive << *reinterpret_cast<Microseconds const*>( pValue );
                }
                break;

                case CoreTypes::Milliseconds:
                {
                    archive << *reinterpret_cast<Milliseconds const*>( pValue );
                }
                break;

                case CoreTypes::Seconds:
                {
                    archive << *reinterpret_cast<Seconds const*>( pValue );
                }
                break;

                case CoreTypes::Percentage:
                {
                    archive << *reinterpret_cast<Percentage const*>( pValue );
                }
                break;

                case CoreTypes::Degrees:
                {
                    archive << *reinterpret_cast<Degrees const*>( pValue );
                }
                break;

                case CoreTypes::Radians:
                {
                    archive << *reinterpret_cast<Radians const*>( pValue );
                }
                break;

                case CoreTypes::ResourcePath:
                {
                    archive << *reinterpret_cast<ResourcePath const*>( pValue );
                }
                break;

                case CoreTypes::IntRange:
                {
                    archive << *reinterpret_cast<IntRange const*>( pValue );
                }
                break;

                case CoreTypes::FloatRange:
                {
                    archive << *reinterpret_cast<FloatRange const*>( pValue );
                }
                break;

                case CoreTypes::ResourceTypeID:
                {
                    archive << *reinterpret_cast<ResourceTypeID const*>( pValue );
                }
                break;

                case CoreTypes::ResourcePtr:
                case CoreTypes::TResourcePtr:
                {
                    archive << *reinterpret_cast<Resource::ResourcePtr const*>( pValue );
                }
                break;

                case CoreTypes::ResourceID:
                {
                    archive << *reinterpret_cast<ResourceID const*>( pValue );
                }
                break;

                case CoreTypes::BitFlags:
                case CoreTypes::TBitFlags:
                {
                    archive << *reinterpret_cast<BitFlags const*>( pValue );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
    }

    void ConvertBinaryToNativeType( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, TVector<Byte> const& byteArray, void* pValue )
    {
        Serialization::BinaryMemoryArchive archive( Serialization::Mode::Read, const_cast<TVector<Byte>&>( byteArray ) );

        // Enums
        if ( !IsCoreType( typeID ) )
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( typeID );
            KRG_ASSERT( pEnumInfo != nullptr );

            // We only support up to 32 bit enum types...
            switch ( pEnumInfo->m_underlyingType )
            {
                case CoreTypes::Uint8:
                {
                    archive >> *reinterpret_cast<uint8*>( pValue );
                }
                break;

                case CoreTypes::Int8:
                {
                    archive >> *reinterpret_cast<int8*>( pValue );
                }
                break;

                case CoreTypes::Uint16:
                {
                    archive >> *reinterpret_cast<uint16*>( pValue );
                }
                break;

                case CoreTypes::Int16:
                {
                    archive >> *reinterpret_cast<int16*>( pValue );
                }
                break;

                case CoreTypes::Uint32:
                {
                    archive >> *reinterpret_cast<uint32*>( pValue );
                }
                break;

                case CoreTypes::Int32:
                {
                    archive >> *reinterpret_cast<int32*>( pValue );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
        else  // Real core types
        {
            CoreTypes const typeToConvert = GetCoreType( typeID );
            switch ( typeToConvert )
            {
                case CoreTypes::Bool:
                {
                    archive >> *reinterpret_cast<bool*>( pValue );
                }
                break;

                case CoreTypes::Uint8:
                {
                    archive >> *reinterpret_cast<uint8*>( pValue );
                }
                break;

                case CoreTypes::Int8:
                {
                    archive >> *reinterpret_cast<int8*>( pValue );
                }
                break;

                case CoreTypes::Uint16:
                {
                    archive >> *reinterpret_cast<uint16*>( pValue );
                }
                break;

                case CoreTypes::Int16:
                {
                    archive >> *reinterpret_cast<int16*>( pValue );
                }
                break;

                case CoreTypes::Uint32:
                {
                    archive >> *reinterpret_cast<uint32*>( pValue );
                }
                break;

                case CoreTypes::Int32:
                {
                    archive >> *reinterpret_cast<int32*>( pValue );
                }
                break;

                case CoreTypes::Uint64:
                {
                    archive >> *reinterpret_cast<uint64*>( pValue );
                }
                break;

                case CoreTypes::Int64:
                {
                    archive >> *reinterpret_cast<int64*>( pValue );
                }
                break;

                case CoreTypes::Float:
                {
                    archive >> *reinterpret_cast<float*>( pValue );
                }
                break;

                case CoreTypes::Double:
                {
                    archive >> *reinterpret_cast<double*>( pValue );
                }
                break;

                case CoreTypes::String:
                {
                    archive >> *reinterpret_cast<String*>( pValue );
                }
                break;

                case CoreTypes::StringID:
                {
                    archive >> *reinterpret_cast<StringID*>( pValue );
                }
                break;

                case CoreTypes::TypeID:
                {
                    StringID ID;
                    archive >> ID;
                    *reinterpret_cast<TypeID*>( pValue ) = TypeID( ID );
                }
                break;

                case CoreTypes::UUID:
                {
                    archive >> *reinterpret_cast<UUID*>( pValue );
                }
                break;

                case CoreTypes::Color:
                {
                    archive >> *reinterpret_cast<Color*>( pValue );
                }
                break;

                case CoreTypes::Float2:
                {
                    archive >> *reinterpret_cast<Float2*>( pValue );
                }
                break;

                case CoreTypes::Float3:
                {
                    archive >> *reinterpret_cast<Float3*>( pValue );
                }
                break;

                case CoreTypes::Float4:
                {
                    archive >> *reinterpret_cast<Float4*>( pValue );
                }
                break;

                case CoreTypes::Vector:
                {
                    archive >> *reinterpret_cast<Vector*>( pValue );
                }
                break;

                case CoreTypes::Quaternion:
                {
                    archive >> *reinterpret_cast<Quaternion*>( pValue );
                }
                break;

                case CoreTypes::Matrix:
                {
                    archive >> *reinterpret_cast<Matrix*>( pValue );
                }
                break;

                case CoreTypes::Transform:
                {
                    archive >> *reinterpret_cast<Transform*>( pValue );
                }
                break;

                case CoreTypes::EulerAngles:
                {
                    archive >> *reinterpret_cast<EulerAngles*>( pValue );
                }
                break;

                case CoreTypes::Microseconds:
                {
                    archive >> *reinterpret_cast<Microseconds*>( pValue );
                }
                break;

                case CoreTypes::Milliseconds:
                {
                    archive >> *reinterpret_cast<Milliseconds*>( pValue );
                }
                break;

                case CoreTypes::Seconds:
                {
                    archive >> *reinterpret_cast<Seconds*>( pValue );
                }
                break;

                case CoreTypes::Percentage:
                {
                    archive >> *reinterpret_cast<Percentage*>( pValue );
                }
                break;

                case CoreTypes::Degrees:
                {
                    archive >> *reinterpret_cast<Degrees*>( pValue );
                }
                break;

                case CoreTypes::Radians:
                {
                    archive >> *reinterpret_cast<Radians*>( pValue );
                }
                break;

                case CoreTypes::ResourcePath:
                {
                    archive >> *reinterpret_cast<ResourcePath*>( pValue );
                }
                break;

                case CoreTypes::IntRange:
                {
                    archive >> *reinterpret_cast<IntRange*>( pValue );
                }
                break;

                case CoreTypes::FloatRange:
                {
                    archive >> *reinterpret_cast<FloatRange*>( pValue );
                }
                break;

                case CoreTypes::ResourceTypeID:
                {
                    archive >> *reinterpret_cast<ResourceTypeID*>( pValue );
                }
                break;

                case CoreTypes::ResourcePtr:
                case CoreTypes::TResourcePtr:
                {
                    archive >> *reinterpret_cast<Resource::ResourcePtr*>( pValue );
                }
                break;

                case CoreTypes::ResourceID:
                {
                    archive >> *reinterpret_cast<ResourceID*>( pValue );
                }
                break;

                case CoreTypes::BitFlags:
                case CoreTypes::TBitFlags:
                {
                    archive >> *reinterpret_cast<BitFlags*>( pValue );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
    }

    void ConvertStringToBinary( TypeRegistry const& typeRegistry, TypeID typeID, TypeID templateArgumentTypeID, String const& strValue, TVector<Byte>& byteArray )
    {
        byteArray.clear();

        //-------------------------------------------------------------------------

        // Enums
        if ( !IsCoreType( typeID ) )
        {
            EnumInfo const* pEnumInfo = typeRegistry.GetEnumInfo( typeID );
            KRG_ASSERT( pEnumInfo != nullptr );

            // We only support up to 32 bit enum types...
            switch ( pEnumInfo->m_underlyingType )
            {
                case CoreTypes::Uint8:
                {
                    uint8 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Int8:
                {
                    int8 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Uint16:
                {
                    uint16 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Int16:
                {
                    int16 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Uint32:
                {
                    uint32 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Int32:
                {
                    int32 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
        else  // Real core types
        {
            CoreTypes const typeToConvert = GetCoreType( typeID );
            switch ( typeToConvert )
            {
                case CoreTypes::Bool:
                {
                    bool value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Uint8:
                {
                    uint8 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Int8:
                {
                    int8 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Uint16:
                {
                    uint16 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Int16:
                {
                    int16 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Uint32:
                {
                    uint32 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Int32:
                {
                    int32 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Uint64:
                {
                    uint64 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Int64:
                {
                    int64 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Float:
                {
                    float value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Double:
                {
                    double value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::String:
                {
                    String value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::StringID:
                {
                    StringID value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::TypeID:
                {
                    TypeID value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::UUID:
                {
                    UUID value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Color:
                {
                    Color value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Float2:
                {
                    Float2 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Float3:
                {
                    Float3 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Float4:
                {
                    Float4 value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Vector:
                {
                    Vector value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Quaternion:
                {
                    Quaternion value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Matrix:
                {
                    Matrix value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Transform:
                {
                    Transform value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::EulerAngles:
                {
                    EulerAngles value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Microseconds:
                {
                    Microseconds value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Milliseconds:
                {
                    Milliseconds value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Seconds:
                {
                    Seconds value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Percentage:
                {
                    Percentage value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Degrees:
                {
                    Degrees value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::Radians:
                {
                    Radians value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::ResourcePath:
                {
                    ResourcePath value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::IntRange:
                {
                    IntRange value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::FloatRange:
                {
                    FloatRange value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::ResourceTypeID:
                {
                    ResourceTypeID value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::ResourcePtr:
                case CoreTypes::TResourcePtr:
                {
                    Resource::ResourcePtr value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::ResourceID:
                {
                    ResourceID value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                case CoreTypes::BitFlags:
                case CoreTypes::TBitFlags:
                {
                    BitFlags value;
                    ConvertStringToNativeType( typeRegistry, typeID, templateArgumentTypeID, strValue, &value );
                    ConvertNativeTypeToBinary( typeRegistry, typeID, templateArgumentTypeID, &value, byteArray );
                }
                break;

                default:
                {
                    KRG_UNREACHABLE_CODE();
                }
                break;
            }
        }
    }

    //-------------------------------------------------------------------------

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