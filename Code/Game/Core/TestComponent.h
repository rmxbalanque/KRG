#pragma once

#include "_Module/API.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "Engine/Core/Entity/EntityComponent.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Math/Transform.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    struct KRG_GAME_CORE_API ExternalTestSubSubStruct : public IRegisteredType
    {
        KRG_REGISTER_TYPE( ExternalTestSubSubStruct );

        KRG_EXPOSE TVector<float>                               m_dynamicArray = { 1.0f, 2.0f, 3.0f };
    };

    struct KRG_GAME_CORE_API ExternalTestSubStruct : public IRegisteredType
    {
        KRG_REGISTER_TYPE( ExternalTestSubStruct );

        KRG_EXPOSE TVector<float>                               m_floats = { 0.3f, 5.0f, 7.0f };
        KRG_EXPOSE TVector<ExternalTestSubSubStruct>            m_dynamicArray = { ExternalTestSubSubStruct(), ExternalTestSubSubStruct() };
    };

    struct KRG_GAME_CORE_API ExternalTestStruct : public IRegisteredType
    {
        KRG_REGISTER_TYPE( ExternalTestStruct );

        KRG_EXPOSE uint8                                        m_uint8 = 8;
        KRG_EXPOSE uint16                                       m_uint16 = 16;
        KRG_EXPOSE uint32                                       m_uint32 = 32;
        KRG_EXPOSE uint64                                       m_U64 = 64;
        KRG_EXPOSE UUID                                         m_UUID;
        KRG_EXPOSE EulerAngles                                  m_eulerAngles = EulerAngles( 23, 45, 56 );
        KRG_EXPOSE TVector<ExternalTestSubStruct>               m_dynamicArray = { ExternalTestSubStruct(), ExternalTestSubStruct() };
    };

    //-------------------------------------------------------------------------

    enum class TestFlags
    {
        KRG_REGISTER_ENUM

        a = 1,
        B = 2,
        c = 3,
        Duplicate = 5,
        D = 4,
        A = 5,
    };

    //-------------------------------------------------------------------------

    class KRG_GAME_CORE_API TestComponent : public EntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT( TestComponent );

    public:

        struct InternalStruct : public IRegisteredType
        {
            KRG_REGISTER_TYPE( InternalStruct );

            KRG_EXPOSE EulerAngles                                  m_eulerAngles;
            KRG_EXPOSE ResourceID                                   m_resourceID;
        };

        enum class InternalEnum : uint8
        {
            KRG_REGISTER_ENUM

            foo = 54,
            Bar = 75
        };

        struct InternalTest
        {
            enum class InternalEnum : int16
            {
                KRG_REGISTER_ENUM

                foo = -1,
                Bar
            };
        };

    public:

        TestComponent() = default;
        virtual void Initialize() override;

    protected:

        KRG_EXPOSE bool                                             m_bool = true;
        KRG_EXPOSE uint8                                            m_U8 = 8;
        KRG_EXPOSE uint16                                           m_U16 = 16;
        KRG_EXPOSE uint32                                           m_U32 = 32;
        KRG_EXPOSE uint64                                           m_U64 = 64;
        KRG_EXPOSE int8                                             m_S8 = -8;
        KRG_EXPOSE int16                                            m_S16 = -16;
        KRG_EXPOSE int32                                            m_S32 = -32;
        KRG_EXPOSE int64                                            m_S64 = -64;
        KRG_EXPOSE float                                            m_F32 = -343.23432432423f;
        KRG_EXPOSE double                                           m_F64 = 343.23432432423;
        KRG_EXPOSE UUID                                             m_UUID;
        KRG_EXPOSE StringID                                         m_StringID = StringID( "Default ID" );
        KRG_EXPOSE String                                           m_String = "Default Test String";
        KRG_EXPOSE Color                                            m_Color = Colors::Pink;
        KRG_EXPOSE Float2                                           m_Float2 = Float2( 1.0f, 2.0f );
        KRG_EXPOSE Float3                                           m_Float3 = Float3( 1.0f, 2.0f, 3.0f);
        KRG_EXPOSE Float4                                           m_Float4 = Float4( 1.0f, 2.0f, 3.0f, 4.0f );
        KRG_EXPOSE Vector                                           m_vector = Vector( 1.0f, 2.0f, 3.0f, 4.0f );
        KRG_EXPOSE Quaternion                                       m_quaternion = Quaternion( AxisAngle( Vector::WorldRight, Degrees( 35 ) ) );
        KRG_EXPOSE Matrix                                           m_matrix;
        KRG_EXPOSE Transform                                        m_affineTransform;
        KRG_EXPOSE Microseconds                                     m_us = 0;
        KRG_EXPOSE Milliseconds                                     m_ms = 0;
        KRG_EXPOSE Seconds                                          m_s = 0;
        KRG_EXPOSE Percentage                                       m_percentage = Percentage( 1.0f );
        KRG_EXPOSE Degrees                                          m_degrees;
        KRG_EXPOSE Radians                                          m_radians;
        KRG_EXPOSE EulerAngles                                      m_eulerAngles = EulerAngles( 15, 25, 23 );
        KRG_EXPOSE ResourcePath                                     m_resourcePath = ResourcePath( "data://Default.txt" );
        KRG_EXPOSE BitFlags                                         m_genericFlags;
        KRG_EXPOSE TBitFlags<TestFlags>                             m_specificFlags;
        KRG_EXPOSE ResourceTypeID                                   m_resourceTypeID;
        KRG_EXPOSE ResourceID                                       m_resourceID;
        KRG_EXPOSE TResourcePtr<Render::SkeletalMesh>               m_specificResourcePtr;

        KRG_EXPOSE IntRange                                         m_intRange;
        KRG_EXPOSE FloatRange                                       m_floatRange;

        // Enums
        KRG_EXPOSE InternalEnum                                     m_internalEnum = InternalEnum::Bar;
        KRG_EXPOSE InternalTest::InternalEnum                       m_testInternalEnum = InternalTest::InternalEnum::Bar;

        // Types
        KRG_EXPOSE ExternalTestStruct                               m_struct0;
        KRG_EXPOSE InternalStruct                                   m_struct1;

        // Arrays
        KRG_EXPOSE float                                            m_staticArray[4];
        KRG_EXPOSE StringID                                         m_staticArrayOfStringIDs[4] = { StringID( "A" ), StringID( "B" ), StringID( "C" ), StringID( "D" ) };
        KRG_EXPOSE InternalStruct                                   m_staticArrayOfStructs[2];
        KRG_EXPOSE InternalTest::InternalEnum                       m_staticArrayOfEnums[6];
        KRG_EXPOSE TVector<float>                                   m_dynamicArray;
        KRG_EXPOSE TVector<ExternalTestStruct>                      m_dynamicArrayOfStructs = { ExternalTestStruct(), ExternalTestStruct(), ExternalTestStruct() };
    };
}