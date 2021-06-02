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
// Version: 4.0.0 (2006/06/28)

#include "Wm4ApplicationPCH.h"
#include "Wm4WindowApplication3.h"
using namespace Wm4;

//----------------------------------------------------------------------------
WindowApplication3::WindowApplication3 (const char* acWindowTitle,
    int iXPosition, int iYPosition, int iWidth, int iHeight,
    const ColorRGBA& rkBackgroundColor)
    :
    WindowApplication(acWindowTitle,iXPosition,iYPosition,iWidth,iHeight,
        rkBackgroundColor)
{
    // camera motion
    m_akWorldAxis[0] = Vector3f::ZERO;
    m_akWorldAxis[1] = Vector3f::ZERO;
    m_akWorldAxis[2] = Vector3f::ZERO;
    m_fTrnSpeed = 0.0f;
    m_fRotSpeed = 0.0f;
    m_bUArrowPressed = false;
    m_bDArrowPressed = false;
    m_bLArrowPressed = false;
    m_bRArrowPressed = false;
    m_bPgUpPressed = false;
    m_bPgDnPressed = false;
    m_bHomePressed = false;
    m_bEndPressed = false;
    m_bInsertPressed = false;
    m_bDeletePressed = false;
    m_bCameraMoveable = false;

    // object motion
    m_iDoRoll = 0;
    m_iDoYaw = 0;
    m_iDoPitch = 0;
    m_spkMotionObject = 0;
    m_fXTrack0 = 0.0f;
    m_fYTrack0 = 0.0f;
    m_fXTrack1 = 0.0f;
    m_fYTrack1 = 0.0f;
    m_bUseTrackBall = true;
    m_bTrackBallDown = false;

    // performance measurements
    m_dLastTime = -1.0;
    m_dAccumulatedTime = 0.0;
    m_dFrameRate = 0.0;
    m_iFrameCount = 0;
    m_iAccumulatedFrameCount = 0;
    m_iTimer = 30;
    m_iMaxTimer = 30;
}
//----------------------------------------------------------------------------
WindowApplication3::~WindowApplication3 ()
{
}
//----------------------------------------------------------------------------
bool WindowApplication3::OnInitialize ()
{
    if (!WindowApplication::OnInitialize())
    {
        return false;
    }

    m_spkCamera = WM4_NEW Camera;
    m_pkRenderer->SetCamera(m_spkCamera);
    m_spkMotionObject = 0;
    return true;
}
//----------------------------------------------------------------------------
void WindowApplication3::OnTerminate ()
{
    m_pkRenderer->SetCamera(0);
    m_spkCamera = 0;
    m_spkMotionObject = 0;
    WindowApplication::OnTerminate();
}
//----------------------------------------------------------------------------
void WindowApplication3::OnDisplay ()
{
    if (m_pkRenderer)
    {
        OnIdle();
    }
}
//----------------------------------------------------------------------------
bool WindowApplication3::OnKeyDown (unsigned char ucKey, int iX, int iY)
{
    if (WindowApplication::OnKeyDown(ucKey,iX,iY))
    {
        return true;
    }

    // standard keys
    switch (ucKey)
    {
    case 't':  // slower camera translation
        if (m_bCameraMoveable)
        {
            m_fTrnSpeed /= m_fTrnSpeedFactor;
        }
        return true;
    case 'T':  // faster camera translation
        if (m_bCameraMoveable)
        {
            m_fTrnSpeed *= m_fTrnSpeedFactor;
        }
        return true;
    case 'r':  // slower camera rotation
        if (m_bCameraMoveable)
        {
            m_fRotSpeed /= m_fRotSpeedFactor;
        }
        return true;
    case 'R':  // faster camera rotation
        if (m_bCameraMoveable)
        {
            m_fRotSpeed *= m_fRotSpeedFactor;
        }
        return true;
    case '?':  // reset the timer
        ResetTime();
        return true;
    };

    return false;
}
//----------------------------------------------------------------------------
bool WindowApplication3::OnSpecialKeyDown (int iKey, int, int)
{
    if (m_bCameraMoveable)
    {
        if (iKey == KEY_LEFT_ARROW)
        {
            m_bLArrowPressed = true;
            return true;
        }
        if (iKey == KEY_RIGHT_ARROW)
        {
            m_bRArrowPressed = true;
            return true;
        }
        if (iKey == KEY_UP_ARROW)
        {
            m_bUArrowPressed = true;
            return true;
        }
        if (iKey == KEY_DOWN_ARROW)
        {
            m_bDArrowPressed = true;
            return true;
        }
        if (iKey == KEY_PAGE_UP)
        {
            m_bPgUpPressed = true;
            return true;
        }
        if (iKey == KEY_PAGE_DOWN)
        {
            m_bPgDnPressed = true;
            return true;
        }
        if (iKey == KEY_HOME)
        {
            m_bHomePressed = true;
            return true;
        }
        if (iKey == KEY_END)
        {
            m_bEndPressed = true;
            return true;
        }
        if (iKey == KEY_INSERT)
        {
            m_bInsertPressed = true;
            return true;
        }
        if (iKey == KEY_DELETE)
        {
            m_bDeletePressed = true;
            return true;
        }
    }

    if (m_spkMotionObject)
    {
        if (iKey == KEY_F1)
        {
            m_iDoRoll = -1;
            return true;
        }
        if (iKey == KEY_F2)
        {
            m_iDoRoll = 1;
            return true;
        }
        if (iKey == KEY_F3)
        {
            m_iDoYaw = -1;
            return true;
        }
        if (iKey == KEY_F4)
        {
            m_iDoYaw = 1;
            return true;
        }
        if (iKey == KEY_F5)
        {
            m_iDoPitch = -1;
            return true;
        }
        if (iKey == KEY_F6)
        {
            m_iDoPitch = 1;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool WindowApplication3::OnSpecialKeyUp (int iKey, int, int)
{
    if (m_bCameraMoveable)
    {
        if (iKey == KEY_LEFT_ARROW)
        {
            m_bLArrowPressed = false;
            return true;
        }
        if (iKey == KEY_RIGHT_ARROW)
        {
            m_bRArrowPressed = false;
            return true;
        }
        if (iKey == KEY_UP_ARROW)
        {
            m_bUArrowPressed = false;
            return true;
        }
        if (iKey == KEY_DOWN_ARROW)
        {
            m_bDArrowPressed = false;
            return true;
        }
        if (iKey == KEY_PAGE_UP)
        {
            m_bPgUpPressed = false;
            return true;
        }
        if (iKey == KEY_PAGE_DOWN)
        {
            m_bPgDnPressed = false;
            return true;
        }
        if (iKey == KEY_HOME)
        {
            m_bHomePressed = false;
            return true;
        }
        if (iKey == KEY_END)
        {
            m_bEndPressed = false;
            return true;
        }
        if (iKey == KEY_INSERT)
        {
            m_bInsertPressed = false;
            return true;
        }
        if (iKey == KEY_DELETE)
        {
            m_bDeletePressed = false;
            return true;
        }
    }

    if (m_spkMotionObject)
    {
        if (iKey == KEY_F1)
        {
            m_iDoRoll = 0;
            return true;
        }
        if (iKey == KEY_F2)
        {
            m_iDoRoll = 0;
            return true;
        }
        if (iKey == KEY_F3)
        {
            m_iDoYaw = 0;
            return true;
        }
        if (iKey == KEY_F4)
        {
            m_iDoYaw = 0;
            return true;
        }
        if (iKey == KEY_F5)
        {
            m_iDoPitch = 0;
            return true;
        }
        if (iKey == KEY_F6)
        {
            m_iDoPitch = 0;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool WindowApplication3::OnMouseClick (int iButton, int iState, int iX,
    int iY, unsigned int)
{
    if (!m_bUseTrackBall
    ||  iButton != MOUSE_LEFT_BUTTON
    ||  !m_spkMotionObject)
    {
        return false;
    }

    float fMult = 1.0f/(m_iWidth >= m_iHeight ? m_iHeight : m_iWidth);

    if (iState == MOUSE_DOWN)
    {
        // get the starting point
        m_bTrackBallDown = true;
        m_kSaveRotate = m_spkMotionObject->Local.GetRotate();
        m_fXTrack0 = (2*iX-m_iWidth)*fMult;
        m_fYTrack0 = (2*(m_iHeight-1-iY)-m_iHeight)*fMult;
    }
    else
    {
        m_bTrackBallDown = false;
    }

    return true;
}
//----------------------------------------------------------------------------
bool WindowApplication3::OnMotion (int iButton, int iX, int iY, unsigned int)
{
    if (!m_bUseTrackBall
    ||  iButton != MOUSE_LEFT_BUTTON
    ||  !m_bTrackBallDown
    ||  !m_spkMotionObject)
    {
        return false;
    }

    // get the ending point
    float fMult = 1.0f/(m_iWidth >= m_iHeight ? m_iHeight : m_iWidth);
    m_fXTrack1 = (2*iX-m_iWidth)*fMult;
    m_fYTrack1 = (2*(m_iHeight-1-iY)-m_iHeight)*fMult;

    // update the object's local rotation
    RotateTrackBall(m_fXTrack0,m_fYTrack0,m_fXTrack1,m_fYTrack1);
    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// camera motion
//----------------------------------------------------------------------------
void WindowApplication3::InitializeCameraMotion (float fTrnSpeed,
    float fRotSpeed, float fTrnSpeedFactor, float fRotSpeedFactor)
{
    m_bCameraMoveable = true;

    m_fTrnSpeed = fTrnSpeed;
    m_fRotSpeed = fRotSpeed;
    m_fTrnSpeedFactor = fTrnSpeedFactor;
    m_fRotSpeedFactor = fRotSpeedFactor;

    m_akWorldAxis[0] = m_spkCamera->GetDVector();
    m_akWorldAxis[1] = m_spkCamera->GetUVector();
    m_akWorldAxis[2] = m_spkCamera->GetRVector();
}
//----------------------------------------------------------------------------
bool WindowApplication3::MoveCamera ()
{
    if (!m_bCameraMoveable)
    {
        return false;
    }

    bool bMoved = false;

    if (m_bUArrowPressed)
    {
        MoveForward();
        bMoved = true;
    }

    if (m_bDArrowPressed)
    {
        MoveBackward();
        bMoved = true;
    }

    if (m_bHomePressed)
    {
        MoveUp();
        bMoved = true;
    }

    if (m_bEndPressed)
    {
        MoveDown();
        bMoved = true;
    }

    if (m_bLArrowPressed)
    {
        TurnLeft();
        bMoved = true;
    }

    if (m_bRArrowPressed)
    {
        TurnRight();
        bMoved = true;
    }

    if (m_bPgUpPressed)
    {
        LookUp();
        bMoved = true;
    }

    if (m_bPgDnPressed)
    {
        LookDown();
        bMoved = true;
    }

    if (m_bInsertPressed)
    {
        MoveRight();
        bMoved = true;
    }

    if (m_bDeletePressed)
    {
        MoveLeft();
        bMoved = true;
    }

    return bMoved;
}
//----------------------------------------------------------------------------
void WindowApplication3::MoveForward ()
{
    Vector3f kLoc = m_spkCamera->GetLocation();
    kLoc += m_fTrnSpeed*m_akWorldAxis[0];
    m_spkCamera->SetLocation(kLoc);
}
//----------------------------------------------------------------------------
void WindowApplication3::MoveBackward ()
{
    Vector3f kLoc = m_spkCamera->GetLocation();
    kLoc -= m_fTrnSpeed*m_akWorldAxis[0];
    m_spkCamera->SetLocation(kLoc);
}
//----------------------------------------------------------------------------
void WindowApplication3::MoveUp ()
{
    Vector3f kLoc = m_spkCamera->GetLocation();
    kLoc += m_fTrnSpeed*m_akWorldAxis[1];
    m_spkCamera->SetLocation(kLoc);
}
//----------------------------------------------------------------------------
void WindowApplication3::MoveDown ()
{
    Vector3f kLoc = m_spkCamera->GetLocation();
    kLoc -= m_fTrnSpeed*m_akWorldAxis[1];
    m_spkCamera->SetLocation(kLoc);
}
//----------------------------------------------------------------------------
void WindowApplication3::TurnLeft ()
{
    Matrix3f kIncr(m_akWorldAxis[1],m_fRotSpeed);
    m_akWorldAxis[0] = kIncr*m_akWorldAxis[0];
    m_akWorldAxis[2] = kIncr*m_akWorldAxis[2];

    Vector3f kDVector = kIncr*m_spkCamera->GetDVector();
    Vector3f kUVector = kIncr*m_spkCamera->GetUVector();
    Vector3f kRVector = kIncr*m_spkCamera->GetRVector();
    m_spkCamera->SetAxes(kDVector,kUVector,kRVector);
}
//----------------------------------------------------------------------------
void WindowApplication3::MoveLeft ()
{
    Vector3f kLoc = m_spkCamera->GetLocation();
    kLoc -= m_fTrnSpeed*m_akWorldAxis[2];
    m_spkCamera->SetLocation(kLoc);
}
//----------------------------------------------------------------------------
void WindowApplication3::MoveRight ()
{
    Vector3f kLoc = m_spkCamera->GetLocation();
    kLoc += m_fTrnSpeed*m_akWorldAxis[2];
    m_spkCamera->SetLocation(kLoc);
}
//----------------------------------------------------------------------------
void WindowApplication3::TurnRight ()
{
    Matrix3f kIncr(m_akWorldAxis[1],-m_fRotSpeed);
    m_akWorldAxis[0] = kIncr*m_akWorldAxis[0];
    m_akWorldAxis[2] = kIncr*m_akWorldAxis[2];

    Vector3f kDVector = kIncr*m_spkCamera->GetDVector();
    Vector3f kUVector = kIncr*m_spkCamera->GetUVector();
    Vector3f kRVector = kIncr*m_spkCamera->GetRVector();
    m_spkCamera->SetAxes(kDVector,kUVector,kRVector);
}
//----------------------------------------------------------------------------
void WindowApplication3::LookUp ()
{
    Matrix3f kIncr(m_akWorldAxis[2],m_fRotSpeed);

    Vector3f kDVector = kIncr*m_spkCamera->GetDVector();
    Vector3f kUVector = kIncr*m_spkCamera->GetUVector();
    Vector3f kRVector = kIncr*m_spkCamera->GetRVector();
    m_spkCamera->SetAxes(kDVector,kUVector,kRVector);
}
//----------------------------------------------------------------------------
void WindowApplication3::LookDown ()
{
    Matrix3f kIncr(m_akWorldAxis[2],-m_fRotSpeed);

    Vector3f kDVector = kIncr*m_spkCamera->GetDVector();
    Vector3f kUVector = kIncr*m_spkCamera->GetUVector();
    Vector3f kRVector = kIncr*m_spkCamera->GetRVector();
    m_spkCamera->SetAxes(kDVector,kUVector,kRVector);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// object motion
//----------------------------------------------------------------------------
void WindowApplication3::InitializeObjectMotion (Spatial* pkMotionObject)
{
    m_spkMotionObject = pkMotionObject;
}
//----------------------------------------------------------------------------
bool WindowApplication3::MoveObject ()
{
    // The coordinate system in which the rotations are applied is that of
    // the object's parent, if it has one.  The parent's world rotation
    // matrix is R, of which the columns are the coordinate axis directions.
    // Column 0 is "direction", column 1 is "up", and column 2 is "right".
    // If the object does not have a parent, the world coordinate axes are
    // used, in which case the rotation matrix is I, the identity.  Column 0
    // is (1,0,0) and is "direction", column 1 is (0,1,0) and is "up", and
    // column 2 is (0,0,1) and is "right".  This choice is consistent with
    // the use of rotations in the Camera and Light classes to store
    // coordinate axes.
    //
    // Roll is about the "direction" axis, yaw is about the "up" axis, and
    // pitch is about the "right" axis.

    if (!m_bCameraMoveable || !m_spkMotionObject)
    {
        return false;
    }

    // Check if the object has been moved by the virtual trackball.
    if (m_bTrackBallDown)
    {
        return true;
    }

    // Check if the object has been moved by the function keys.
    Spatial* pkParent = m_spkMotionObject->GetParent();
    Vector3f kAxis;
    float fAngle;
    Matrix3f kRot, kIncr;

    if (m_iDoRoll)
    {
        kRot = m_spkMotionObject->Local.GetRotate();

        fAngle = m_iDoRoll*m_fRotSpeed;
        if (pkParent)
        {
            kAxis = pkParent->World.GetRotate().GetColumn(0);
        }
        else
        {
            kAxis = Vector3f::UNIT_X;
        }

        kIncr.FromAxisAngle(kAxis,fAngle);
        kRot = kIncr*kRot;
        kRot.Orthonormalize();
        m_spkMotionObject->Local.SetRotate(kRot);
        return true;
    }

    if (m_iDoYaw)
    {
        kRot = m_spkMotionObject->Local.GetRotate();

        fAngle = m_iDoYaw*m_fRotSpeed;
        if (pkParent)
        {
            kAxis = pkParent->World.GetRotate().GetColumn(1);
        }
        else
        {
            kAxis = Vector3f::UNIT_Y;
        }

        kIncr.FromAxisAngle(kAxis,fAngle);
        kRot = kIncr*kRot;
        kRot.Orthonormalize();
        m_spkMotionObject->Local.SetRotate(kRot);
        return true;
    }

    if (m_iDoPitch)
    {
        kRot = m_spkMotionObject->Local.GetRotate();

        fAngle = m_iDoPitch*m_fRotSpeed;
        if (pkParent)
        {
            kAxis = pkParent->World.GetRotate().GetColumn(2);
        }
        else
        {
            kAxis = Vector3f::UNIT_Z;
        }

        kIncr.FromAxisAngle(kAxis,fAngle);
        kRot = kIncr*kRot;
        kRot.Orthonormalize();
        m_spkMotionObject->Local.SetRotate(kRot);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
void WindowApplication3::RotateTrackBall (float fX0, float fY0, float fX1,
    float fY1)
{
    if ((fX0 == fX1 && fY0 == fY1) || !m_spkCamera)  // nothing to rotate
        return;

    // get first vector on sphere
    float fLength = Mathf::Sqrt(fX0*fX0+fY0*fY0), fInvLength, fZ0, fZ1;
    if (fLength > 1.0f)
    {
        // outside unit disk, project onto it
        fInvLength = 1.0f/fLength;
        fX0 *= fInvLength;
        fY0 *= fInvLength;
        fZ0 = 0.0f;
    }
    else
    {
        // compute point (x0,y0,z0) on negative unit hemisphere
        fZ0 = 1.0f - fX0*fX0 - fY0*fY0;
        fZ0 = (fZ0 <= 0.0f ? 0.0f : Mathf::Sqrt(fZ0));
    }
    fZ0 *= -1.0f;

    // use camera world coordinates, order is (D,U,R), so point is (z,y,x)
    Vector3f kVec0(fZ0,fY0,fX0);

    // get second vector on sphere
    fLength = Mathf::Sqrt(fX1*fX1+fY1*fY1);
    if (fLength > 1.0f)
    {
        // outside unit disk, project onto it
        fInvLength = 1.0f/fLength;
        fX1 *= fInvLength;
        fY1 *= fInvLength;
        fZ1 = 0.0f;
    }
    else
    {
        // compute point (x1,y1,z1) on negative unit hemisphere
        fZ1 = 1.0f - fX1*fX1 - fY1*fY1;
        fZ1 = (fZ1 <= 0.0f ? 0.0f : Mathf::Sqrt(fZ1));
    }
    fZ1 *= -1.0f;

    // use camera world coordinates, order is (D,U,R), so point is (z,y,x)
    Vector3f kVec1(fZ1,fY1,fX1);

    // create axis and angle for the rotation
    Vector3f kAxis = kVec0.Cross(kVec1);
    float fDot = kVec0.Dot(kVec1);
    float fAngle;
    if (kAxis.Normalize() > Mathf::ZERO_TOLERANCE)
    {
        fAngle = Mathf::ACos(kVec0.Dot(kVec1)); 
    }
    else  // vectors are parallel
    {
        if (fDot < 0.0f)
        {
            // rotated pi radians
            fInvLength = Mathf::InvSqrt(fX0*fX0+fY0*fY0);
            kAxis.X() = fY0*fInvLength;
            kAxis.Y() = -fX0*fInvLength;
            kAxis.Z() = 0.0f;
            fAngle = Mathf::PI;
        }
        else
        {
            // rotation by zero radians
            kAxis = Vector3f::UNIT_X;
            fAngle = 0.0f;
        }
    }

    // Compute the world rotation matrix implied by trackball motion.  The
    // axis vector was computed in camera coordinates.  It must be converted
    // to world coordinates.  Once again, I use the camera ordering (D,U,R).
    Vector3f kWorldAxis =
        kAxis.X()*m_spkCamera->GetDVector() +
        kAxis.Y()*m_spkCamera->GetUVector() +
        kAxis.Z()*m_spkCamera->GetRVector();

    Matrix3f kTrackRotate(kWorldAxis,fAngle);

    // Compute the new local rotation.  If the object is the root of the
    // scene, the new rotation is simply the *incremental rotation* of the
    // trackball applied *after* the object has been rotated by its old
    // local rotation.  If the object is not the root of the scene, you have
    // to convert the incremental rotation by a change of basis in the
    // parent's coordinate space. 
    const Spatial* pkParent = m_spkMotionObject->GetParent();
    Matrix3f kLocalRot;
    if (pkParent)
    {
        const Matrix3f& rkPRotate = pkParent->World.GetRotate();
        kLocalRot = rkPRotate.TransposeTimes(kTrackRotate) * rkPRotate *
            m_kSaveRotate;
    }
    else
    {
        kLocalRot = kTrackRotate*m_kSaveRotate;
    }
    kLocalRot.Orthonormalize();
    m_spkMotionObject->Local.SetRotate(kLocalRot);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// performance measurements
//----------------------------------------------------------------------------
void WindowApplication3::ResetTime ()
{
    m_dLastTime = -1.0f;
}
//----------------------------------------------------------------------------
void WindowApplication3::MeasureTime ()
{
    // start performance measurements
    if (m_dLastTime == -1.0)
    {
        m_dLastTime = System::GetTime();
        m_dAccumulatedTime = 0.0;
        m_dFrameRate = 0.0;
        m_iFrameCount = 0;
        m_iAccumulatedFrameCount = 0;
        m_iTimer = m_iMaxTimer;
    }

    // accumulate the time only when the miniature time allows it
    if (--m_iTimer == 0)
    {
        double dCurrentTime = System::GetTime();
        double dDelta = dCurrentTime - m_dLastTime;
        m_dLastTime = dCurrentTime;
        m_dAccumulatedTime += dDelta;
        m_iAccumulatedFrameCount += m_iFrameCount;
        m_iFrameCount = 0;
        m_iTimer = m_iMaxTimer;
    }
}
//----------------------------------------------------------------------------
void WindowApplication3::UpdateFrameCount ()
{
    m_iFrameCount++;
}
//----------------------------------------------------------------------------
void WindowApplication3::DrawFrameRate (int iX, int iY,
    const ColorRGBA& rkColor)
{
    if (m_dAccumulatedTime > 0.0)
    {
        m_dFrameRate = m_iAccumulatedFrameCount/m_dAccumulatedTime;
    }
    else
    {
        m_dFrameRate = 0.0;
    }

    const size_t uiSize = 256;
    char acMessage[uiSize];
    System::Sprintf(acMessage,uiSize,"fps: %.1lf",m_dFrameRate);
    m_pkRenderer->Draw(iX,iY,rkColor,acMessage);
}
//----------------------------------------------------------------------------
