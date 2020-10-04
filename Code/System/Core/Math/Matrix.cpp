#include "Matrix.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Matrix const Matrix::Identity( 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 );

    //-------------------------------------------------------------------------

    Matrix::Matrix( F32 v00, F32 v01, F32 v02, F32 v03, F32 v10, F32 v11, F32 v12, F32 v13, F32 v20, F32 v21, F32 v22, F32 v23, F32 v30, F32 v31, F32 v32, F32 v33 )
    {
        m_rows[0] = Vector( v00, v01, v02, v03 );
        m_rows[1] = Vector( v10, v11, v12, v13 );
        m_rows[2] = Vector( v20, v21, v22, v23 );
        m_rows[3] = Vector( v30, v31, v32, v33 );
    }

    Matrix::Matrix( Vector const xAxis, Vector const yAxis, Vector const zAxis )
    {
        m_rows[0] = xAxis;
        m_rows[1] = yAxis;
        m_rows[2] = zAxis;
        m_rows[3] = Vector::Zero;
    }

    Matrix::Matrix( Vector const xAxis, Vector const yAxis, Vector const zAxis, Vector const translation )
    {
        m_rows[0] = xAxis;
        m_rows[1] = yAxis;
        m_rows[2] = zAxis;
        m_rows[3] = translation;
    }

    Matrix::Matrix( EulerAngles const& eulerAngles, Vector const translation )
    {
        float cx, cy, cz, sx, sy, sz, czsx, cxcz, sysz;

        sx = sinf( eulerAngles.x ); cx = cosf( eulerAngles.x );
        sy = sinf( eulerAngles.y ); cy = cosf( eulerAngles.y );
        sz = sinf( eulerAngles.z ); cz = cosf( eulerAngles.z );

        czsx = cz * sx;
        cxcz = cx * cz;
        sysz = sy * sz;

        // Order is XYZ
        m_values[0][0] = cy * cz;
        m_values[0][1] = cy * sz;
        m_values[0][2] = -sy;
        m_values[1][0] = czsx * sy - cx * sz;
        m_values[1][1] = cxcz + sx * sysz;
        m_values[1][2] = cy * sx;
        m_values[2][0] = cxcz * sy + sx * sz;
        m_values[2][1] = -czsx + cx * sysz;
        m_values[2][2] = cx * cy;
        m_values[0][3] = 0.0f;
        m_values[1][3] = 0.0f;
        m_values[2][3] = 0.0f;

        // Translation
        m_rows[3] = translation;
    }

    EulerAngles Matrix::ToEulerAngles() const
    {
        KRG_ASSERT( IsOrthonormal() );

        EulerAngles result;

        result.x = Math::ATan2( m_values[1][2], m_values[2][2] );

        F32 const c2 = Math::Sqrt( ( m_values[0][0] * m_values[0][0] ) + ( m_values[0][1] * m_values[0][1] ) );
        result.y = Math::ATan2( -m_values[0][2], c2 );

        F32 const s1 = Math::Sin( result.x );
        F32 const c1 = Math::Cos( result.x );
        result.z = Math::ATan2( ( s1 * m_values[2][0] ) - ( c1 * m_values[1][0] ), ( c1 * m_values[1][1] ) - ( s1 * m_values[2][1] ) );

        return result;
    }

    //-------------------------------------------------------------------------
    // Matrix Decomposition
    //-------------------------------------------------------------------------

    static bool CheckForZeroScaleInRow( F32 scale, Vector const& row )
    {
        F32 const absScale = Math::Abs( scale );

        for ( int i = 0; i < 3; i++ )
        {
            if ( absScale < 1 && Math::Abs( row[i] ) >= FLT_MAX * absScale )
            {
                return false;
            }
        }

        return true;
    }

    // Copied from the IlmBase math library and modified for KRG
    static bool ExtractAndRemoveScalingAndShear( Matrix& matrix, Vector& scale, Vector& shear )
    {
        // This implementation follows the technique described in the paper by
        // Spencer W. Thomas in the Graphics Gems II article: "Decomposing a 
        // Matrix into Simple Transformations", p. 320.

        Vector row[3];
        row[0] = Vector( matrix[0][0], matrix[0][1], matrix[0][2] );
        row[1] = Vector( matrix[1][0], matrix[1][1], matrix[1][2] );
        row[2] = Vector( matrix[2][0], matrix[2][1], matrix[2][2] );

        F32 maxVal = 0;
        for ( int i = 0; i < 3; i++ )
        {
            for ( int j = 0; j < 3; j++ )
            {
                if ( Math::Abs( row[i][j] ) > maxVal )
                {
                    maxVal = Math::Abs( row[i][j] );
                }
            }
        }

        // We normalize the 3x3 matrix here.
        // It was noticed that this can improve numerical stability significantly,
        // especially when many of the upper 3x3 matrix's coefficients are very
        // close to zero; we correct for this step at the end by multiplying the 
        // scaling factors by maxVal at the end (shear and rotation are not 
        // affected by the normalization).

        if ( maxVal != 0 )
        {
            for ( int i = 0; i < 3; i++ )
            {
                if ( !CheckForZeroScaleInRow( maxVal, row[i] ) )
                {
                    return false;
                }
                else
                {
                    row[i] /= maxVal;
                }
            }
        }

        // Compute X scale factor.
        scale.x = row[0].Length3().GetX();
        if ( !CheckForZeroScaleInRow( scale.x, row[0] ) )
        {
            return false;
        }

        // Normalize first row.
        row[0] /= scale.x;

        // An XY shear factor will shear the X coord. as the Y coord. changes.
        // There are 6 combinations (XY, XZ, YZ, YX, ZX, ZY), although we only
        // extract the first 3 because we can effect the last 3 by shearing in
        // XY, XZ, YZ combined rotations and scales.
        //
        // shear matrix <   1,  YX,  ZX,  0,
        //                 XY,   1,  ZY,  0,
        //                 XZ,  YZ,   1,  0,
        //                  0,   0,   0,  1 >

        // Compute XY shear factor and make 2nd row orthogonal to 1st.
        shear[0] = Vector::Dot3( row[0], row[1] ).ToFloat();
        row[1] -= row[0] * shear[0];

        // Now, compute Y scale.
        scale.y = row[1].Length3().GetX();
        if ( !CheckForZeroScaleInRow( scale.y, row[1] ) )
        {
            return false;
        }

        // Normalize 2nd row and correct the XY shear factor for Y scaling.
        row[1] /= scale.y;
        shear[0] /= scale.y;

        // Compute XZ and YZ shears, orthogonalize 3rd row.
        shear[1] = Vector::Dot3( row[0], row[2] ).ToFloat();
        row[2] -= row[0] * shear[1];
        shear[2] = Vector::Dot3( row[1], row[2] ).ToFloat();
        row[2] -= row[1] * shear[2];

        // Next, get Z scale.
        scale.z = row[2].Length3().ToFloat();
        if ( !CheckForZeroScaleInRow( scale.z, row[2] ) )
        {
            return false;
        }

        // Normalize 3rd row and correct the XZ and YZ shear factors for Z scaling.
        row[2] /= scale.z;
        shear[1] /= scale.z;
        shear[2] /= scale.z;

        // At this point, the upper 3x3 matrix in mat is orthonormal.
        // Check for a coordinate system flip. If the determinant
        // is less than zero, then negate the matrix and the scaling factors.
        if ( Vector::Dot3( row[0], Vector::Cross3( row[1], row[2] ) ).ToFloat() < 0 )
        {
            for ( int i = 0; i < 3; i++ )
            {
                scale[i] *= -1;
                row[i] *= -1;
            }
        }

        // Copy over the orthonormal rows into the returned matrix.
        // The upper 3x3 matrix in mat is now a rotation matrix.
        for ( int i = 0; i < 3; i++ )
        {
            matrix[i][0] = row[i][0];
            matrix[i][1] = row[i][1];
            matrix[i][2] = row[i][2];
        }

        // Correct the scaling factors for the normalization step that we 
        // performed above; shear and rotation are not affected by the 
        // normalization.
        scale *= maxVal;

        return true;
    }

    bool Matrix::Decompose( Quaternion& outRotation, Vector& outTranslation, Vector& outScale ) const
    {
        Matrix copy = *this;
        Vector shr = Vector::Zero;
        outScale = Vector::Zero;

        // Extract and remove scale and shear from matrix
        if ( ExtractAndRemoveScalingAndShear( copy, outScale, shr ) )
        {
            // Extract rotation and translation from unscaled matrix
            outRotation = copy.GetRotation();
            outTranslation = copy.GetTranslation();
            return true;
        }

        return false;
    }

    //-------------------------------------------------------------------------

    Vector Matrix::GetScale() const
    {
        Matrix copy = *this;
        Vector scale = Vector::Zero, shear;
        if ( !ExtractAndRemoveScalingAndShear( copy, scale, shear ) )
        {
            F32 const lengthX = m_rows[0].Length3().ToFloat();
            F32 const lengthY = m_rows[1].Length3().ToFloat();
            F32 const lengthZ = m_rows[2].Length3().ToFloat();
            scale = Vector( lengthX, lengthY, lengthZ, 0.0f );
        }

        return scale;
    }

    Matrix& Matrix::SetScale( Vector const& newScale )
    {
        Vector scale, shear;
        bool result = ExtractAndRemoveScalingAndShear( *this, scale, shear );
        KRG_ASSERT( result ); // Cannot set scale on matrix that contains zero-scale

        //-------------------------------------------------------------------------

        m_rows[0] = m_rows[0] * newScale.x;
        m_rows[1] = m_rows[1] * newScale.y;
        m_rows[2] = m_rows[2] * newScale.z;
        return *this;
    }

    Matrix& Matrix::RemoveScale()
    {
        Vector scale, shear;
        bool result = ExtractAndRemoveScalingAndShear( *this, scale, shear );
        KRG_ASSERT( result ); // Cannot remove zero scale from matrix
        return *this;
    }
}