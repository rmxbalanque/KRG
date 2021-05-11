#pragma once

#include "_Module/API.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Entity/EntityComponent.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Math/Transform.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    struct KRG_GAME_CORE_API ExternalTestSubSubStruct
    {
        KRG_REGISTER_TYPE;

        EXPOSE TVector<float>                               m_dynamicArray = { 1.0f, 2.0f, 3.0f };
    };

    struct KRG_GAME_CORE_API ExternalTestSubStruct
    {
        KRG_REGISTER_TYPE;

        EXPOSE TVector<float>                               m_floats = { 0.3f, 5.0f, 7.0f };
        EXPOSE TVector<ExternalTestSubSubStruct>            m_dynamicArray = { ExternalTestSubSubStruct(), ExternalTestSubSubStruct() };
    };

    struct KRG_GAME_CORE_API ExternalTestStruct
    {
        KRG_REGISTER_TYPE;

        EXPOSE uint8                                        m_uint8 = 8;
        EXPOSE uint16                                       m_uint16 = 16;
        EXPOSE uint32                                       m_uint32 = 32;
        EXPOSE uint64                                       m_U64 = 64;
        EXPOSE UUID                                         m_UUID;
        EXPOSE EulerAngles                                  m_eulerAngles = EulerAngles( 23, 45, 56 );
        EXPOSE TVector<ExternalTestSubStruct>               m_dynamicArray = { ExternalTestSubStruct(), ExternalTestSubStruct() };
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
        KRG_REGISTER_ENTITY_COMPONENT;

    public:

        struct InternalStruct
        {
            KRG_REGISTER_TYPE;

            EXPOSE EulerAngles                                  m_eulerAngles;
            EXPOSE ResourceID                                   m_resourceID;
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

        EXPOSE bool                                             m_bool = true;
        EXPOSE uint8                                            m_U8 = 8;
        EXPOSE uint16                                           m_U16 = 16;
        EXPOSE uint32                                           m_U32 = 32;
        EXPOSE uint64                                           m_U64 = 64;
        EXPOSE int8                                             m_S8 = -8;
        EXPOSE int16                                            m_S16 = -16;
        EXPOSE int32                                            m_S32 = -32;
        EXPOSE int64                                            m_S64 = -64;
        EXPOSE float                                            m_F32 = -343.23432432423f;
        EXPOSE double                                           m_F64 = 343.23432432423;
        EXPOSE UUID                                             m_UUID;
        EXPOSE StringID                                         m_StringID = StringID( "Default ID" );
        EXPOSE String                                           m_String = "Default Test String";
        EXPOSE Color                                            m_Color = Colors::Pink;
        EXPOSE Float2                                           m_Float2 = Float2( 1.0f, 2.0f );
        EXPOSE Float3                                           m_Float3 = Float3( 1.0f, 2.0f, 3.0f);
        EXPOSE Float4                                           m_Float4 = Float4( 1.0f, 2.0f, 3.0f, 4.0f );
        EXPOSE Vector                                           m_vector = Vector( 1.0f, 2.0f, 3.0f, 4.0f );
        EXPOSE Quaternion                                       m_quaternion = Quaternion( AxisAngle( Vector::WorldRight, Degrees( 35 ) ) );
        EXPOSE Matrix                                           m_matrix;
        EXPOSE Transform                                        m_affineTransform;
        EXPOSE Microseconds                                     m_us = 0;
        EXPOSE Milliseconds                                     m_ms = 0;
        EXPOSE Seconds                                          m_s = 0;
        EXPOSE Percentage                                       m_percentage = Percentage( 1.0f );
        EXPOSE Degrees                                          m_degrees;
        EXPOSE Radians                                          m_radians;
        EXPOSE EulerAngles                                      m_eulerAngles = EulerAngles( 15, 25, 23 );
        EXPOSE DataPath                                         m_dataPath = DataPath( "data://Default.txt" );
        EXPOSE BitFlags                                         m_genericFlags;
        EXPOSE TBitFlags<TestFlags>                             m_specificFlags;
        EXPOSE ResourceTypeID                                   m_resourceTypeID;
        EXPOSE ResourceID                                       m_resourceID;
        EXPOSE TResourcePtr<Render::SkeletalMesh>               m_specificResourcePtr;

        // Enums
        EXPOSE InternalEnum                                     m_internalEnum = InternalEnum::Bar;
        EXPOSE InternalTest::InternalEnum                       m_testInternalEnum = InternalTest::InternalEnum::Bar;

        // Types
        EXPOSE ExternalTestStruct                               m_struct0;
        EXPOSE InternalStruct                                   m_struct1;

        // Arrays
        EXPOSE float                                            m_staticArray[4];
        EXPOSE StringID                                         m_staticArrayOfStringIDs[4] = { StringID( "A" ), StringID( "B" ), StringID( "C" ), StringID( "D" ) };
        EXPOSE InternalStruct                                   m_staticArrayOfStructs[2];
        EXPOSE InternalTest::InternalEnum                       m_staticArrayOfEnums[6];
        EXPOSE TVector<float>                                   m_dynamicArray;
        EXPOSE TVector<ExternalTestStruct>                      m_dynamicArrayOfStructs = { ExternalTestStruct(), ExternalTestStruct(), ExternalTestStruct() };
    };
}