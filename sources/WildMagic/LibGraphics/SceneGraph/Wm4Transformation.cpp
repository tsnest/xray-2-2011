// Wild Magic Source Code
// David Eberly
// http://www.geometrictools.com
// Copyright (c) 1998-2009
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.  The license is available for reading at
// either of the locations:
//     http://www.gnu.org/copyleft/lgpl.html
//     http://www.geometrictools.com/License/WildMagicLicense.pdf
//
// Version: 4.0.2 (2008/08/05)

#include "Wm4GraphicsPCH.h"
#include "Wm4Transformation.h"
using namespace Wm4;

const Transformation Transformation::IDENTITY;

//----------------------------------------------------------------------------
Transformation::Transformation ()
    :
    m_kMatrix(1.0f,0.0f,0.0f,0.0f,1.0f,0.0f,0.0f,0.0f,1.0f),
    m_kTranslate(0.0f,0.0f,0.0f),
    m_kScale(1.0f,1.0f,1.0f)
{
    // default transform is the identity
    m_bIsIdentity = true;
    m_bIsRSMatrix = true;
    m_bIsUniformScale = true;
}
//----------------------------------------------------------------------------
Transformation::~Transformation ()
{
}
//----------------------------------------------------------------------------
void Transformation::MakeIdentity ()
{
    m_kMatrix = Matrix3f::IDENTITY;
    m_kTranslate = Vector3f::ZERO;
    m_kScale = Vector3f(1.0f,1.0f,1.0f);
    m_bIsIdentity = true;
    m_bIsRSMatrix = true;
    m_bIsUniformScale = true;
}
//----------------------------------------------------------------------------
void Transformation::MakeUnitScale ()
{
    assert(m_bIsRSMatrix);

    m_kScale = Vector3f(1.0f,1.0f,1.0f);
    m_bIsUniformScale = true;
}
//----------------------------------------------------------------------------
float Transformation::GetNorm () const
{
    if (m_bIsRSMatrix)
    {
        float fMax = Mathf::FAbs(m_kScale.X());
        if (Mathf::FAbs(m_kScale.Y()) > fMax)
        {
            fMax = Mathf::FAbs(m_kScale.Y());
        }
        if (Mathf::FAbs(m_kScale.Z()) > fMax)
        {
            fMax = Mathf::FAbs(m_kScale.Z());
        }
        return fMax;
    }

    // A general matrix.  Use the max-row-sum matrix norm.  The spectral
    // norm (the maximum absolute value of the eigenvalues) is smaller or
    // equal to this norm.  Therefore, this function returns an approximation
    // to the maximum scale.
    float fMaxRowSum =
        Mathf::FAbs(m_kMatrix[0][0]) +
        Mathf::FAbs(m_kMatrix[0][1]) +
        Mathf::FAbs(m_kMatrix[0][2]);

    float fRowSum =
        Mathf::FAbs(m_kMatrix[1][0]) +
        Mathf::FAbs(m_kMatrix[1][1]) +
        Mathf::FAbs(m_kMatrix[1][2]);

    if ( fRowSum > fMaxRowSum )
        fMaxRowSum = fRowSum;

    fRowSum =
        Mathf::FAbs(m_kMatrix[2][0]) +
        Mathf::FAbs(m_kMatrix[2][1]) +
        Mathf::FAbs(m_kMatrix[2][2]);

    if (fRowSum > fMaxRowSum)
    {
        fMaxRowSum = fRowSum;
    }

    return fMaxRowSum;
}
//----------------------------------------------------------------------------
void Transformation::SetRotate (const Matrix3f& rkRotate)
{
    m_kMatrix = rkRotate;
    m_bIsIdentity = false;
    m_bIsRSMatrix = true;
}
//----------------------------------------------------------------------------
void Transformation::SetTranslate (const Vector3f& rkTranslate)
{
    m_kTranslate = rkTranslate;
    m_bIsIdentity = false;
}
//----------------------------------------------------------------------------
void Transformation::SetScale (const Vector3f& rkScale)
{
    assert(m_bIsRSMatrix && rkScale.X() != 0.0f && rkScale.Y() != 0.0f
        && rkScale.Z() != 0.0f);

    m_kScale = rkScale;
    m_bIsIdentity = false;
    m_bIsUniformScale = false;
}
//----------------------------------------------------------------------------
void Transformation::SetUniformScale (float fScale)
{
    assert(m_bIsRSMatrix && fScale != 0.0f);
    m_kScale = Vector3f(fScale,fScale,fScale);
    m_bIsIdentity = false;
    m_bIsUniformScale = true;
}
//----------------------------------------------------------------------------
void Transformation::SetMatrix (const Matrix3f& rkMatrix)
{
    m_kMatrix = rkMatrix;
    m_bIsIdentity = false;
    m_bIsRSMatrix = false;
    m_bIsUniformScale = false;
}
//----------------------------------------------------------------------------
Vector3f Transformation::ApplyForward (const Vector3f& rkInput) const
{
    if (m_bIsIdentity)
    {
        // Y = X
        return rkInput;
    }

    if (m_bIsRSMatrix)
    {
        // Y = R*S*X + T
        Vector3f kOutput(m_kScale.X()*rkInput.X(),m_kScale.Y()*rkInput.Y(),
            m_kScale.Z()*rkInput.Z());
        kOutput = m_kMatrix*kOutput + m_kTranslate;
        return kOutput;
    }

    // Y = M*X + T
    Vector3f kOutput = m_kMatrix*rkInput + m_kTranslate;
    return kOutput;
}
//----------------------------------------------------------------------------
void Transformation::ApplyForward (int iQuantity, const Vector3f* akInput,
    Vector3f* akOutput) const
{
    if (m_bIsIdentity)
    {
        // Y = X
        size_t uiSize = iQuantity*sizeof(Vector3f);
        System::Memcpy(akOutput,uiSize,akInput,uiSize);
    }
    else
    {
        int i;
        if (m_bIsRSMatrix)
        {
            // Y = R*S*X + T
            for (i = 0; i < iQuantity; i++)
            {
                akOutput[i].X() = m_kScale.X()*akInput[i].X();
                akOutput[i].Y() = m_kScale.Y()*akInput[i].Y();
                akOutput[i].Z() = m_kScale.Z()*akInput[i].Z();
                akOutput[i] = m_kMatrix*akOutput[i] + m_kTranslate;
            }
        }
        else
        {
            // Y = M*X + T
            for (i = 0; i < iQuantity; i++)
            {
                akOutput[i] = m_kMatrix*akInput[i] + m_kTranslate;
            }
        }
    }
}
//----------------------------------------------------------------------------
Vector3f Transformation::ApplyInverse (const Vector3f& rkInput) const
{
    if (m_bIsIdentity)
    {
        // X = Y
        return rkInput;
    }

    Vector3f kOutput = rkInput - m_kTranslate;
    if (m_bIsRSMatrix)
    {
        // X = S^{-1}*R^t*(Y - T)
        kOutput = kOutput*m_kMatrix;
        if (m_bIsUniformScale)
        {
            kOutput /= GetUniformScale();
        }
        else
        {
            // The direct inverse scaling is
            //   kOutput.X() /= m_kScale.X();
            //   kOutput.Y() /= m_kScale.Y();
            //   kOutput.Z() /= m_kScale.Z();
            // When division is much more expensive than multiplication,
            // three divisions are replaced by one division and ten
            // multiplications.
            float fSXY = m_kScale.X()*m_kScale.Y();
            float fSXZ = m_kScale.X()*m_kScale.Z();
            float fSYZ = m_kScale.Y()*m_kScale.Z();
            float fInvDet = 1.0f/(fSXY*m_kScale.Z());
            kOutput.X() *= fInvDet*fSYZ;
            kOutput.Y() *= fInvDet*fSXZ;
            kOutput.Z() *= fInvDet*fSXY;
        }
    }
    else
    {
        // X = M^{-1}*(Y - T)
        kOutput = m_kMatrix.Inverse()*kOutput;
    }

    return kOutput;
}
//----------------------------------------------------------------------------
void Transformation::ApplyInverse (int iQuantity, const Vector3f* akInput,
    Vector3f* akOutput) const
{
    if (m_bIsIdentity)
    {
        // X = Y
        size_t uiSize = iQuantity*sizeof(Vector3f);
        System::Memcpy(akOutput,uiSize,akInput,uiSize);
        return;
    }

    Vector3f kDiff;
    int i;
    if (m_bIsRSMatrix)
    {
        // X = S^{-1}*R^t*(Y - T)
        if (m_bIsUniformScale)
        {
            float fInvScale = 1.0f/GetUniformScale();
            for (i = 0; i < iQuantity; i++)
            {
                kDiff = akInput[i] - m_kTranslate;
                akOutput[i] = fInvScale*(kDiff*m_kMatrix);
            }
        }
        else
        {
            // The direct inverse scaling is
            //   fInvXScale = 1.0f/m_kScale.X();
            //   fInvYScale = 1.0f/m_kScale.Y();
            //   fInvZScale = 1.0f/m_kScale.Z();
            // When division is much more expensive than multiplication, three
            // divisions are replaced by one division and ten multiplications.
            float fSXY = m_kScale.X()*m_kScale.Y();
            float fSXZ = m_kScale.X()*m_kScale.Z();
            float fSYZ = m_kScale.Y()*m_kScale.Z();
            float fInvDet = 1.0f/(fSXY*m_kScale.Z());
            float fInvXScale = fInvDet*fSYZ;
            float fInvYScale = fInvDet*fSXZ;
            float fInvZScale = fInvDet*fSXY;
            for (i = 0; i < iQuantity; i++)
            {
                kDiff = akInput[i] - m_kTranslate;
                akOutput[i] = kDiff*m_kMatrix;
                akOutput[i].X() *= fInvXScale;
                akOutput[i].Y() *= fInvYScale;
                akOutput[i].Z() *= fInvZScale;
            }
        }
    }
    else
    {
        // X = M^{-1}*(Y - T)
        Matrix3f kInverse = m_kMatrix.Inverse();
        for (i = 0; i < iQuantity; i++)
        {
            kDiff = akInput[i] - m_kTranslate;
            akOutput[i] = kInverse*kDiff;
        }
    }
}
//----------------------------------------------------------------------------
Vector3f Transformation::ApplyForwardVector (const Vector3f& rkInput) const
{
    if (m_bIsIdentity)
    {
        // X = Y
        return rkInput;
    }

    if (m_bIsRSMatrix)
    {
        // Y = R*S*X
        Vector3f kOutput(m_kScale.X()*rkInput.X(),m_kScale.Y()*rkInput.Y(),
            m_kScale.Z()*rkInput.Z());
        kOutput = m_kMatrix*kOutput;
        return kOutput;
    }

    // Y = M*X
    Vector3f kOutput = m_kMatrix*rkInput;
    return kOutput;
}
//----------------------------------------------------------------------------
Vector3f Transformation::InvertVector (const Vector3f& rkInput) const
{
    if (m_bIsIdentity)
    {
        // X = Y
        return rkInput;
    }

    Vector3f kOutput;
    if (m_bIsRSMatrix)
    {
        // X = S^{-1}*R^t*Y
        kOutput = rkInput*m_kMatrix;
        if (m_bIsUniformScale)
        {
            kOutput /= GetUniformScale();
        }
        else
        {
            // The direct inverse scaling is
            //   kOutput.X() /= m_kScale.X();
            //   kOutput.Y() /= m_kScale.Y();
            //   kOutput.Z() /= m_kScale.Z();
            // When division is much more expensive than multiplication,
            // three divisions are replaced by one division and ten
            // multiplications.
            float fSXY = m_kScale.X()*m_kScale.Y();
            float fSXZ = m_kScale.X()*m_kScale.Z();
            float fSYZ = m_kScale.Y()*m_kScale.Z();
            float fInvDet = 1.0f/(fSXY*m_kScale.Z());
            kOutput.X() *= fInvDet*fSYZ;
            kOutput.Y() *= fInvDet*fSXZ;
            kOutput.Z() *= fInvDet*fSXY;
        }
    }
    else
    {
        // X = M^{-1}*Y
        kOutput = m_kMatrix.Inverse()*rkInput;
    }

    return kOutput;
}
//----------------------------------------------------------------------------
Plane3f Transformation::ApplyForward (const Plane3f& rkInput) const
{
    if (m_bIsIdentity)
    {
        return rkInput;
    }

    Plane3f kOutput;
    if (m_bIsRSMatrix)
    {
        // Let X represent points in model space and Y = R*S*X+T represent
        // points in world space where S are the world scales, R is the world
        // rotation, and T is the world translation.  The inverse transform is
        // X = S^{-1}*R^t*(Y-T).  The model plane is Dot(N0,X) = C0.
        // Replacing the formula for X in it and applying some algebra leads
        // to the world plane Dot(N1,Y) = C1 where N1 = R*S^{-1}*N0 and
        // C1 = C0+Dot(N1,T).  If S is not the identity, then N1 is not unit
        // length.  We need to normalize N1 and adjust C1:  N1' = N1/|N1| and
        // C1' = C1/|N1|.
        if (m_bIsUniformScale)
        {
            kOutput.Normal = m_kMatrix*rkInput.Normal;
            kOutput.Constant = GetUniformScale()*rkInput.Constant +
                kOutput.Normal.Dot(m_kTranslate);
            return kOutput;
        }

        kOutput.Normal = rkInput.Normal;

        // The direct inverse scaling is
        //   kOutput.X() /= m_kScale.X();
        //   kOutput.Y() /= m_kScale.Y();
        //   kOutput.Z() /= m_kScale.Z();
        // When division is much more expensive than multiplication,
        // three divisions are replaced by one division and ten
        // multiplications.
        float fSXY = m_kScale.X()*m_kScale.Y();
        float fSXZ = m_kScale.X()*m_kScale.Z();
        float fSYZ = m_kScale.Y()*m_kScale.Z();
        float fInvDet = 1.0f/(fSXY*m_kScale.Z());
        kOutput.Normal.X() *= fInvDet*fSYZ;
        kOutput.Normal.Y() *= fInvDet*fSXZ;
        kOutput.Normal.Z() *= fInvDet*fSXY;
        kOutput.Normal = m_kMatrix*kOutput.Normal;
    }
    else
    {
        // Let X represent points in model space and Y = M*X+T represent
        // points in world space.  The inverse transform is X = M^{-1}*(Y-T).
        // The model plane is Dot(N0,X) = C0.  Replacing the formula for X in
        // it and applying some algebra leads to the world plane
        // Dot(N1,Y) = C1 where N1 = M^{-T}*N0/|M^{-1}*N0| (superscript -T
        // denotes the transpose of the inverse) and
        // C1 = C0/|M^{-1}*N0|+Dot(N1,T).
        Matrix3f kInverse = m_kMatrix.Inverse();
        kOutput.Normal = rkInput.Normal*kInverse;
    }

    float fInvLength = 1.0f/kOutput.Normal.Length();
    kOutput.Normal *= fInvLength;
    kOutput.Constant = fInvLength*rkInput.Constant +
        kOutput.Normal.Dot(m_kTranslate);

    return kOutput;
}
//----------------------------------------------------------------------------
void Transformation::Product (const Transformation& rkA,
    const Transformation& rkB)
{
    if (rkA.IsIdentity())
    {
        *this = rkB;
        return;
    }

    if (rkB.IsIdentity())
    {
        *this = rkA;
        return;
    }

    if (rkA.m_bIsRSMatrix && rkB.m_bIsRSMatrix)
    {
        if (rkA.m_bIsUniformScale)
        {
            SetRotate(rkA.m_kMatrix*rkB.m_kMatrix);

            SetTranslate(rkA.GetUniformScale()*(
                rkA.m_kMatrix*rkB.m_kTranslate) + rkA.m_kTranslate);

            if (rkB.IsUniformScale())
            {
                SetUniformScale(rkA.GetUniformScale()*rkB.GetUniformScale());
            }
            else
            {
                SetScale(rkA.GetUniformScale()*rkB.GetScale());
            }

            return;
        }
    }

    // In all remaining cases, the matrix cannot be written as R*S*X+T.
    Matrix3f kMA = (rkA.m_bIsRSMatrix ?
        rkA.m_kMatrix.TimesDiagonal(rkA.m_kScale) :
        rkA.m_kMatrix);

    Matrix3f kMB = (rkB.m_bIsRSMatrix ?
        rkB.m_kMatrix.TimesDiagonal(rkB.m_kScale) :
        rkB.m_kMatrix);

    SetMatrix(kMA*kMB);
    SetTranslate(kMA*rkB.m_kTranslate+rkA.m_kTranslate);
}
//----------------------------------------------------------------------------
void Transformation::Inverse (Transformation& rkInverse) const
{
    if (m_bIsIdentity)
    {
        rkInverse = *this;
        return;
    }

    if (m_bIsRSMatrix)
    {
        if (m_bIsUniformScale)
        {
            rkInverse.m_kMatrix = GetRotate().Transpose()/GetUniformScale();
        }
        else
        {
            Matrix3f kRS = m_kMatrix.TimesDiagonal(m_kScale);
            rkInverse.m_kMatrix = kRS.Inverse();
        }
    }
    else
    {
        rkInverse.m_kMatrix = m_kMatrix.Inverse();
    }

    rkInverse.m_kTranslate = -(rkInverse.m_kMatrix*m_kTranslate);
    rkInverse.m_bIsIdentity = false;
    rkInverse.m_bIsRSMatrix = false;
    rkInverse.m_bIsUniformScale = false;
}
//----------------------------------------------------------------------------
void Transformation::GetHomogeneous (Matrix4f& rkHMatrix) const
{
    if (m_bIsRSMatrix)
    {
        rkHMatrix[0][0] = m_kScale[0]*m_kMatrix[0][0];
        rkHMatrix[0][1] = m_kScale[0]*m_kMatrix[1][0];
        rkHMatrix[0][2] = m_kScale[0]*m_kMatrix[2][0];
        rkHMatrix[0][3] = 0.0f;
        rkHMatrix[1][0] = m_kScale[1]*m_kMatrix[0][1];
        rkHMatrix[1][1] = m_kScale[1]*m_kMatrix[1][1];
        rkHMatrix[1][2] = m_kScale[1]*m_kMatrix[2][1];
        rkHMatrix[1][3] = 0.0f;
        rkHMatrix[2][0] = m_kScale[2]*m_kMatrix[0][2];
        rkHMatrix[2][1] = m_kScale[2]*m_kMatrix[1][2];
        rkHMatrix[2][2] = m_kScale[2]*m_kMatrix[2][2];
        rkHMatrix[2][3] = 0.0f;
    }
    else
    {
        rkHMatrix[0][0] = m_kMatrix[0][0];
        rkHMatrix[0][1] = m_kMatrix[1][0];
        rkHMatrix[0][2] = m_kMatrix[2][0];
        rkHMatrix[0][3] = 0.0f;
        rkHMatrix[1][0] = m_kMatrix[0][1];
        rkHMatrix[1][1] = m_kMatrix[1][1];
        rkHMatrix[1][2] = m_kMatrix[2][1];
        rkHMatrix[1][3] = 0.0f;
        rkHMatrix[2][0] = m_kMatrix[0][2];
        rkHMatrix[2][1] = m_kMatrix[1][2];
        rkHMatrix[2][2] = m_kMatrix[2][2];
        rkHMatrix[2][3] = 0.0f;
    }

    rkHMatrix[3][0] = m_kTranslate[0];
    rkHMatrix[3][1] = m_kTranslate[1];
    rkHMatrix[3][2] = m_kTranslate[2];
    rkHMatrix[3][3] = 1.0f;
}
//----------------------------------------------------------------------------
