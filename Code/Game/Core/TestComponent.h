#pragma once

#include "_Module/API.h"
#include "System/TypeSystem/TypeRegistrationMacros.h"
#include "System/Entity/EntityComponent.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Math/Transform.h"
#include "System/Resource/ResourcePtr.h"

//-------------------------------------------------------------------------

namespace KRG
{
    struct KRG_GAME_CORE_API AnotherTestStruct
    {
        KRG_REGISTER_TYPE;

        EXPOSE TVector<F32>                                 m_dynamicArray = { 0.3f, 5.0f, 7.0f };
    };

    struct KRG_GAME_CORE_API TestStruct
    {
        KRG_REGISTER_TYPE;

        EXPOSE U8                                           m_U8 = 8;
        EXPOSE U16                                          m_U16 = 16;
        EXPOSE U32                                          m_U32 = 32;
        EXPOSE U64                                          m_U64 = 64;
        EXPOSE UUID                                         m_UUID;
        EXPOSE EulerAngles                                  m_eulerAngles = EulerAngles( 23, 45, 56 );
        EXPOSE TVector<AnotherTestStruct>                   m_dynamicArray = { AnotherTestStruct(), AnotherTestStruct() };
    };

    //-------------------------------------------------------------------------

    class KRG_GAME_CORE_API TestComponent : public EntityComponent
    {
        KRG_REGISTER_ENTITY_COMPONENT;

    public:

        struct InternalStruct
        {
            KRG_REGISTER_TYPE;

            EXPOSE EulerAngles                                      m_eulerAngles;
            EXPOSE ResourceID                                       m_resourceID;
        };

        enum class InternalEnum : U8
        {
            KRG_REGISTER_ENUM

            foo = 54,
            Bar = 75
        };

        struct Test
        {
            enum class InternalEnum : S16
            {
                KRG_REGISTER_ENUM

                foo = -1,
                Bar
            };
        };

    public:

        TestComponent() = default;

    protected:

        EXPOSE bool                                             m_bool = true;
        EXPOSE U8                                               m_U8 = 8;
        EXPOSE U16                                              m_U16 = 16;
        EXPOSE U32                                              m_U32 = 32;
        EXPOSE U64                                              m_U64 = 64;
        EXPOSE S8                                               m_S8 = -8;
        EXPOSE S16                                              m_S16 = -16;
        EXPOSE S32                                              m_S32 = -32;
        EXPOSE S64                                              m_S64 = -64;
        EXPOSE F32                                              m_F32 = -343.23432432423f;
        EXPOSE F64                                              m_F64 = 343.23432432423;
        EXPOSE Color                                            m_Color = Colors::Pink;
        EXPOSE UUID                                             m_UUID;
        EXPOSE String                                           m_String = "Default Test String";
        EXPOSE StringID                                         m_StringID = StringID( "Default ID" );
        EXPOSE DataPath                                         m_DataPath = "Data://Default.txt";
        EXPOSE Float2                                           m_Float2 = Float2( 1.0f, 2.0f );
        EXPOSE Float3                                           m_Float3 = Float3( 1.0f, 2.0f, 3.0f);
        EXPOSE Float4                                           m_Float4 = Float4( 1.0f, 2.0f, 3.0f, 4.0f );
        EXPOSE Quaternion                                       m_Quaternion = Quaternion( AxisAngle( Vector::WorldRight, Degrees( 35 ) ) );
        EXPOSE Matrix                                           m_Matrix;
        EXPOSE Transform                                        m_AffineTransform;

        EXPOSE InternalEnum                                     m_internalEnum = InternalEnum::Bar;

        EXPOSE Test::InternalEnum                               m_testIinternalEnum = Test::InternalEnum::Bar;

        EXPOSE EulerAngles                                      m_eulerAngles = EulerAngles( 15, 25, 23 );

        EXPOSE ResourceID                                       m_resourceID;

        EXPOSE TestStruct                                       m_struct;

        EXPOSE F32                                              m_staticArray[4];
        EXPOSE TVector<F32>                                     m_dynamicArray;

        EXPOSE StringID                                         m_staticArrayOfIDs[4] = { StringID( "A" ), StringID( "B" ), StringID( "C" ), StringID( "D" ) };
        EXPOSE TVector<TestStruct>                              m_dynamicArrayOfStructs = { TestStruct(), TestStruct(), TestStruct() };

        EXPOSE InternalStruct                                   m_staticArrayOfStructs[2];
        EXPOSE Test::InternalEnum                               m_staticArrayOfEnums[6];
    };


}