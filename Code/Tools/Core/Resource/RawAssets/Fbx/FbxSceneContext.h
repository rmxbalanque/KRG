#pragma once

#include "Tools/Core/_Module/API.h"
#include "System/Core/Math/Transform.h"
#include "System/Core/Types/String.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Types/StringID.h"
#include <fbxsdk.h>

//-------------------------------------------------------------------------

using namespace fbxsdk;

//-------------------------------------------------------------------------
// An FBX scene context + helpers
//-------------------------------------------------------------------------

// This helper allows us to import an FBX file and perform all necessary coordinate conversion and scale adjustments
// NB! This helper need all implementations to be inlined, as the FBXSDK doesnt work out of the box across DLL boundaries.

// Implements axis system and unit system conversions from any FBX system to the KRG system
// KRG Coordinate system is ( +Z up, -Y forward, Right Handed i.e. the same as Max/Maya Z-Up/Blender )
// KRG units are meters

// Unit conversion is handled when we convert from FBX to KRG types, since FBX bakes the conversion into the FBX transforms and we have to remove it afterwards

//-------------------------------------------------------------------------

namespace KRG::Fbx
{
    class KRG_TOOLS_CORE_API FbxSceneContext
    {

    public:

        FbxSceneContext( FileSystem::Path const& filePath );
        ~FbxSceneContext();

        inline bool IsValid() const { return m_pManager != nullptr && m_pScene != nullptr && m_error.empty(); }
        inline String const& GetErrorMessage() const { return m_error; }

        // Scene helpers
        //-------------------------------------------------------------------------

        void FindAllNodesOfType( FbxNodeAttribute::EType nodeType, TVector<FbxNode*>& results ) const;
        FbxNode* FindNodeOfTypeByName( FbxNodeAttribute::EType nodeType, char const* pNodeName ) const;

        void FindAllAnimStacks( TVector<FbxAnimStack*>& results ) const;
        FbxAnimStack* FindAnimStack( char const* pTakeName ) const;

        inline FbxAMatrix GetNodeGlobalTransform( FbxNode* pNode ) const
        {
            KRG_ASSERT( pNode != nullptr );

            FbxAMatrix geometricTransform;
            FbxVector4 const gT = pNode->GetGeometricTranslation( FbxNode::eSourcePivot );
            FbxVector4 const gR = pNode->GetGeometricRotation( FbxNode::eSourcePivot );
            FbxVector4 const gS = pNode->GetGeometricScaling( FbxNode::eSourcePivot );
            geometricTransform.SetT( gT );
            geometricTransform.SetR( gR );
            geometricTransform.SetS( gS );

            geometricTransform = pNode->EvaluateGlobalTransform() * geometricTransform;
            return geometricTransform;
        }

        // Conversion Functions
        //-------------------------------------------------------------------------

        inline Transform ConvertMatrixToTransform( FbxAMatrix const& fbxMatrix ) const
        {
            auto const Q = fbxMatrix.GetQ();
            auto const T = fbxMatrix.GetT();
            auto const S = fbxMatrix.GetS();

            auto const rotation = Quaternion( (float) Q[0], (float) Q[1], (float) Q[2], (float) Q[3] ).GetNormalized();
            auto const translation = Vector( (float) T[0], (float) T[1], (float) T[2], 1.0f );
            auto const scale = Vector( (float) S[0], (float) S[1], (float) S[2], 0.0f );

            Transform transform( rotation, translation, scale );
            transform.SanitizeScaleValues();
            return transform;
        }

        inline Vector ConvertVector( FbxVector4 const& fbxVector ) const
        {
            Vector krgVector( (float) fbxVector[0], (float) fbxVector[1], (float) fbxVector[2], 1.0f );
            return krgVector;
        }

        // Up Axis Correction
        //-------------------------------------------------------------------------
        // This is not universally needed since the FBX scene deep convert corrects certain transforms

        inline Transform ApplyUpAxisCorrection( Transform const& transform ) const
        {
            Transform correctedMatrix = m_upAxisCorrectionTransform * transform;
            return correctedMatrix;
        }

        inline Vector ApplyUpAxisCorrection( Vector const& point ) const
        {
            Vector correctedVector = m_upAxisCorrectionTransform.TransformPoint( point );
            return correctedVector;
        }

        // Scale correction
        //-------------------------------------------------------------------------

        inline float GetScaleConversionMultiplier() const { return m_scaleConversionMultiplier; }

        inline Transform const& GetScaleConversionTransform() const { return m_scaleConversionTransform; }

        inline Vector ApplyScaleCorrection( Vector const& vector ) const
        {
            return Vector( vector.m_x * m_scaleConversionMultiplier, vector.m_y * m_scaleConversionMultiplier, vector.m_z * m_scaleConversionMultiplier, vector.m_w );
        }

    private:

        FbxSceneContext() = delete;
        FbxSceneContext( FbxSceneContext const& ) = delete;
        FbxSceneContext& operator=( FbxSceneContext const& ) = delete;

    public:

        FbxManager*                 m_pManager = nullptr;
        FbxScene*                   m_pScene = nullptr;

    private:

        String                      m_error;
        float                         m_scaleConversionMultiplier = 1.0f;
        Transform                   m_scaleConversionTransform;
        Transform                   m_upAxisCorrectionTransform;
    };
}