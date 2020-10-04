#ifdef _WIN32
#pragma once

#include "../../_Module/API.h"
#include "Tools/Core/Thirdparty/cgltf/cgltf.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Core/Math/Transform.h"

//-------------------------------------------------------------------------
// A GLTF scene context + helpers
//-------------------------------------------------------------------------

// Implements axis system and unit system conversions from the gltf coordinate system ( Right-Handed, Y-Up, -Z as forward ) to the KRG system
// KRG Coordinate system is ( +Z up, -Y forward, Right-Handed i.e. the same as Max/Maya Z-Up/Blender )
// KRG units are meters

// Luckily all that's need to convert gltf scenes to KRG is a -90 rotation on X

//-------------------------------------------------------------------------

namespace KRG
{
    namespace gltf
    {
        class KRG_TOOLS_RESOURCE_API gltfSceneContext
        {
        public:

            gltfSceneContext( FileSystemPath const& filePath );
            ~gltfSceneContext();

            inline bool IsValid() const { return m_pSceneData != nullptr; }
            inline String const& GetErrorMessage() const { return m_error; }

            cgltf_data const* GetSceneData() const { return m_pSceneData; }

            // Conversion Functions
            //-------------------------------------------------------------------------

            inline Transform ConvertMatrix( cgltf_float const gltfMatrix[16] ) const
            {
                Matrix krgMatrix
                (
                    (F32) gltfMatrix[0], (F32) gltfMatrix[1], (F32) gltfMatrix[2], (F32) gltfMatrix[3],
                    (F32) gltfMatrix[4], (F32) gltfMatrix[5], (F32) gltfMatrix[6], (F32) gltfMatrix[7],
                    (F32) gltfMatrix[8], (F32) gltfMatrix[9], (F32) gltfMatrix[10], (F32) gltfMatrix[11],
                    (F32) gltfMatrix[12], (F32) gltfMatrix[13], (F32) gltfMatrix[14], (F32) gltfMatrix[15]
                );

                Transform convertedTransform = Transform( krgMatrix );
                convertedTransform.SanitizeScaleValues();
                return convertedTransform;
            }

            inline Vector ConvertVector( cgltf_float const gltfVector[3] ) const
            {
                Vector krgVector( (F32) gltfVector[0], (F32) gltfVector[1], (F32) gltfVector[2], 1.0f );
                return krgVector;
            }

            // Up Axis Correction
            //-------------------------------------------------------------------------

            inline Transform GetUpAxisCorrectionTransform() const { return m_upAxisCorrectionTransform; }

            inline Transform ApplyUpAxisCorrection( Transform const& matrix ) const
            {
                Transform correctedMatrix = matrix * m_upAxisCorrectionTransform;
                return correctedMatrix;
            }

            inline Vector ApplyUpAxisCorrection( Vector const& point ) const
            {
                Vector correctedVector = m_upAxisCorrectionTransform.TransformPoint( point );
                return correctedVector;
            }

            // Helpers
            //-------------------------------------------------------------------------

            inline Transform GetNodeTransform( cgltf_node* pNode ) const
            {
                KRG_ASSERT( pNode != nullptr );

                Transform t;

                if ( pNode->has_matrix )
                {
                    t = ConvertMatrix( pNode->matrix );
                }
                else
                {
                    Quaternion rotation = Quaternion::Identity;
                    if ( pNode->has_rotation )
                    {
                        rotation = Quaternion( pNode->rotation[0], pNode->rotation[1], pNode->rotation[2], pNode->rotation[3] );
                    }

                    Vector translation = Vector::UnitW;
                    if ( pNode->has_translation )
                    {
                        translation = Vector( pNode->translation[0], pNode->translation[1], pNode->translation[2], 1.0f );
                    }

                    Vector scale = Vector::One;
                    if ( pNode->has_scale )
                    {
                        scale = Vector( pNode->scale[0], pNode->scale[1], pNode->scale[2], 1.0f );
                    }

                    t = Transform( rotation, translation, scale );
                }

                return t;
            }

        private:

            cgltf_data*                 m_pSceneData = nullptr;
            String                      m_error;
            Transform                   m_upAxisCorrectionTransform = Transform( AxisAngle( Float3::UnitX, Degrees(90) ) );
        };
    }
}
#endif