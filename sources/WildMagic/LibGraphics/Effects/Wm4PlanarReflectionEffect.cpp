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
// Version: 4.0.2 (2007/06/07)

#include "Wm4GraphicsPCH.h"
#include "Wm4PlanarReflectionEffect.h"
#include "Wm4VisibleSet.h"
#include "Wm4Camera.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,PlanarReflectionEffect,Effect);
WM4_IMPLEMENT_STREAM(PlanarReflectionEffect);

//----------------------------------------------------------------------------
PlanarReflectionEffect::PlanarReflectionEffect (int iQuantity)
{
    assert(iQuantity > 0);
    m_iQuantity = iQuantity;
    m_aspkPlane = WM4_NEW TriMeshPtr[m_iQuantity];
    m_afReflectance = WM4_NEW float[m_iQuantity];

    m_spkAState = WM4_NEW AlphaState;
    m_spkSState = WM4_NEW StencilState;
    m_spkZState = WM4_NEW ZBufferState;
}
//----------------------------------------------------------------------------
PlanarReflectionEffect::PlanarReflectionEffect ()
{
    m_iQuantity = 0;
    m_aspkPlane = 0;
    m_afReflectance = 0;
}
//----------------------------------------------------------------------------
PlanarReflectionEffect::~PlanarReflectionEffect ()
{
    WM4_DELETE[] m_aspkPlane;
    WM4_DELETE[] m_afReflectance;
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::Draw (Renderer* pkRenderer, Spatial*, int iMin,
    int iMax, VisibleObject* akVisible)
{
    // Save the current global state for restoration later.
    StencilStatePtr spkSaveSState = pkRenderer->GetStencilState();
    ZBufferStatePtr spkSaveZState = pkRenderer->GetZBufferState();

    // Enable depth-buffering and stenciling.
    m_spkZState->Enabled = true;
    m_spkZState->Writable = true;
    m_spkZState->Compare = ZBufferState::CF_LEQUAL;
    pkRenderer->SetZBufferState(m_spkZState);

    m_spkSState->Enabled = true;
    pkRenderer->SetStencilState(m_spkSState);

    // The number of planes is limited by the size of a value in the stencil
    // buffer (256 for an 8-bit stencil buffer).
    int iQuantity = m_iQuantity;
    if (iQuantity >= pkRenderer->GetMaxStencilIndices())
    {
        iQuantity = pkRenderer->GetMaxStencilIndices() - 1;
    }

    Camera* pkCamera = pkRenderer->GetCamera();
    float fPortN, fPortF;
    pkCamera->GetDepthRange(fPortN,fPortF);

    int j;
    for (int i = 0; i < iQuantity; i++)
    {
        // Render the mirror into the stencil plane.  All visible mirror
        // pixels will have the stencil value of the mirror.  Make sure that
        // no pixels are written to the depth buffer or color buffer, but use
        // depth buffer testing so that the stencil will not be written where
        // the plane is behind something already in the depth buffer.
        m_spkSState->Compare = StencilState::CF_ALWAYS;
        m_spkSState->Reference = (unsigned int)(i+1);
        m_spkSState->OnFail = StencilState::OT_KEEP;      // irrelevant
        m_spkSState->OnZFail = StencilState::OT_KEEP;     // invisible kept 0
        m_spkSState->OnZPass = StencilState::OT_REPLACE;  // visible to i+1
        pkRenderer->SetStencilState(m_spkSState);

        m_spkZState->Enabled = false;
        pkRenderer->SetZBufferState(m_spkZState);

        pkRenderer->SetColorMask(false,false,false,false);

        pkRenderer->Draw(m_aspkPlane[i]);

        pkRenderer->SetColorMask(true,true,true,true);

        // Render the mirror plane again by only processing pixels where the
        // stencil buffer contains the reference value.  This time there are
        // no changes to the stencil buffer and the depth buffer value is
        // reset to the far clipping plane.  This is done by setting the range
        // of depth values in the viewport volume to be [1,1].  Since the
        // mirror plane cannot also be semi-transparent, we do not care what
        // is behind the mirror plane in the depth buffer.  We need to move
        // the depth buffer values back where the mirror plane will be
        // rendered so that when the reflected object is rendered, it can be
        // depth buffered correctly.  Note that the rendering of the reflected
        // object will cause depth value to be written, which will appear to
        // be behind the mirror plane.  Enable writes to the color buffer.
        // Later when we want to render the reflecting plane and have it blend
        // with the background, which should contain the reflected caster, we
        // want to use the same blending function so that the pixels where the
        // reflected object was not rendered will contain the reflecting plane
        // colors.  In that case, the blending result will have the reflecting
        // plane appear to be opaque when in reality it was blended with
        // blending coefficients adding to one.
        pkCamera->SetDepthRange(1.0f,1.0f);
        m_spkZState->Enabled = true;
        m_spkZState->Compare = ZBufferState::CF_ALWAYS;
        pkRenderer->SetZBufferState(m_spkZState);

        m_spkSState->Compare = StencilState::CF_EQUAL;
        m_spkSState->Reference = (unsigned int)(i+1);
        m_spkSState->OnFail = StencilState::OT_KEEP;
        m_spkSState->OnZFail = StencilState::OT_KEEP;
        m_spkSState->OnZPass = StencilState::OT_KEEP;
        pkRenderer->SetStencilState(m_spkSState);

        pkRenderer->Draw(m_aspkPlane[i]);

        // Restore the depth range and depth testing function.
        m_spkZState->Compare = ZBufferState::CF_LEQUAL;
        pkRenderer->SetZBufferState(m_spkZState);
        pkCamera->SetDepthRange(fPortN,fPortF);

        // Compute the equation for the mirror plane in model coordinates
        // and get the reflection matrix in world coordinates.
        Matrix4f kReflection;
        Plane3f kPlane;
        GetReflectionMatrixAndPlane(i,kReflection,kPlane);

        // Enable a clip plane so that only objects above the mirror plane
        // are reflected.  This occurs before SetTransformation because it
        // needs the current geometric pipeline matrices to compute the clip
        // plane in the correct coordinate system.
        pkRenderer->EnableUserClipPlane(0,kPlane);

        // This temporarily modifies the view matrix, effectively producing
        // H_world = H'_view = H_world*(H_reflection*H_view).
        pkRenderer->SetPostWorldTransformation(kReflection);

        // Reverse the cull direction.  Allow for models that are not
        // necessarily set up with front or back face culling.
        pkRenderer->SetReverseCullFace(true);
        pkRenderer->SetCullState(pkRenderer->GetCullState());

        // Render the reflected object.  Only render where the stencil buffer
        // contains the reference value.
        for (j = iMin; j <= iMax; j++)
        {
            if (akVisible[j].IsDrawable())
            {
                pkRenderer->Draw((Geometry*)akVisible[j].Object);
            }
        }

        pkRenderer->SetReverseCullFace(false);
        pkRenderer->SetCullState(pkRenderer->GetCullState());

        pkRenderer->RestorePostWorldTransformation();
        pkRenderer->DisableUserClipPlane(0);

        // We are about to render the reflecting plane again.  Reset to the
        // global state for the reflecting plane.  We want to blend the
        // reflecting plane with what is already in the color buffer,
        // particularly either the image of the reflected caster or the
        // reflecting plane.  All we want for the reflecting plane at this
        // stage is to force the alpha channel to always be the reflectance
        // value for the reflecting plane.  Render the reflecting plane
        // wherever the stencil buffer is set to the reference value.  This
        // time clear the stencil buffer reference value where it is set.
        // Perform the normal depth buffer testing and writes.  Allow the
        // color buffer to be written to, but this time blend the reflecting
        // plane with the values in the color buffer based on the reflectance
        // value.  Note that where the stencil buffer is set, the color buffer
        // has either color values from the reflecting plane or the reflected
        // object.  Blending will use src=1-alpha (reflecting plane) and
        // dest=alpha background (reflecting plane or reflected object).
        m_spkAState->BlendEnabled = true;
        m_spkAState->SrcBlend = AlphaState::SBF_ONE_MINUS_CONSTANT_ALPHA;
        m_spkAState->DstBlend = AlphaState::DBF_CONSTANT_ALPHA;
        m_spkAState->ConstantColor = ColorRGBA(0.0f,0.0f,0.0f,
            m_afReflectance[i]);
        pkRenderer->SetAlphaState(m_spkAState);

        m_spkSState->Compare = StencilState::CF_EQUAL;
        m_spkSState->Reference = (unsigned int)(i+1);
        m_spkSState->OnFail = StencilState::OT_KEEP;
        m_spkSState->OnZFail = StencilState::OT_KEEP;
        m_spkSState->OnZPass = StencilState::OT_INVERT;
        pkRenderer->SetStencilState(m_spkSState);

        pkRenderer->Draw(m_aspkPlane[i]);

        m_spkAState->BlendEnabled = false;
        pkRenderer->SetAlphaState(m_spkAState);
    }

    // Disable depth-buffering and stenciling (in the effect's states).
    m_spkZState->Enabled = false;
    m_spkSState->Enabled = false;

    // Restore the global state that existed before this function call.
    pkRenderer->SetStencilState(spkSaveSState);
    pkRenderer->SetZBufferState(spkSaveZState);

    // Render the objects as usual, this time drawing only the potentially
    // visible objects.
    for (j = iMin; j <= iMax; j++)
    {
        if (akVisible[j].IsDrawable())
        {
            pkRenderer->Draw((Geometry*)akVisible[j].Object);
        }
    }
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::GetReflectionMatrixAndPlane (int i,
    Matrix4f& rkReflection, Plane3f& rkPlane)
{
    // Compute the equation for the mirror plane in world coordinates.
    Triangle3f kTri;
    m_aspkPlane[i]->GetWorldTriangle(0,kTri);
    rkPlane = Plane3f(kTri.V[0],kTri.V[1],kTri.V[2]);

    // Compute the reflection matrix.
    rkReflection.MakeReflection(rkPlane.Normal,kTri.V[0]);

    m_aspkPlane[i]->GetModelTriangle(0,kTri);
    rkPlane = Plane3f(kTri.V[0],kTri.V[1],kTri.V[2]);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* PlanarReflectionEffect::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = Effect::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_aspkPlane)
    {
        for (int i = 0; i < m_iQuantity; i++)
        {
            if (m_aspkPlane[i])
            {
                pkFound = m_aspkPlane[i]->GetObjectByName(rkName);
                if (pkFound)
                {
                    return pkFound;
                }
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Effect::GetAllObjectsByName(rkName,rkObjects);

    if (m_aspkPlane)
    {
        for (int i = 0; i < m_iQuantity; i++)
        {
            if (m_aspkPlane[i])
            {
                m_aspkPlane[i]->GetAllObjectsByName(rkName,rkObjects);
            }
        }
    }
}
//----------------------------------------------------------------------------
Object* PlanarReflectionEffect::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Effect::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_aspkPlane)
    {
        for (int i = 0; i < m_iQuantity; i++)
        {
            if (m_aspkPlane[i])
            {
                pkFound = m_aspkPlane[i]->GetObjectByID(uiID);
                if (pkFound)
                {
                    return pkFound;
                }
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void PlanarReflectionEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Effect::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iQuantity);

    if (m_iQuantity > 0)
    {
        m_aspkPlane = WM4_NEW TriMeshPtr[m_iQuantity];
        m_afReflectance = WM4_NEW float[m_iQuantity];

        // native data
        rkStream.Read(m_iQuantity,m_afReflectance);

        // link data
        for (int i = 0; i < m_iQuantity; i++)
        {
            Object* pkObject;
            rkStream.Read(pkObject);  // m_aspkPlane[i]
            pkLink->Add(pkObject);
        }
    }

    WM4_END_DEBUG_STREAM_LOAD(PlanarReflectionEffect);
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Effect::Link(rkStream,pkLink);

    for (int i = 0; i < m_iQuantity; i++)
    {
        Object* pkLinkID = pkLink->GetLinkID();
        m_aspkPlane[i] = (TriMesh*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool PlanarReflectionEffect::Register (Stream& rkStream) const
{
    if (!Effect::Register(rkStream))
    {
        return false;
    }

    if (m_aspkPlane)
    {
        for (int i = 0; i < m_iQuantity; i++)
        {
            if (m_aspkPlane[i])
            {
                m_aspkPlane[i]->Register(rkStream);
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void PlanarReflectionEffect::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Effect::Save(rkStream);

    // native data
    rkStream.Write(m_iQuantity);
    if (m_iQuantity > 0)
    {
        rkStream.Write(m_iQuantity,m_afReflectance);

        // link data
        ObjectPtr* aspkArray = (ObjectPtr*)m_aspkPlane;
        //rkStream.Write(m_iQuantity,aspkArray);
        for (int i = 0; i < m_iQuantity; i++)
        {
            rkStream.Write(aspkArray[i]);
        }
    }

    WM4_END_DEBUG_STREAM_SAVE(PlanarReflectionEffect);
}
//----------------------------------------------------------------------------
int PlanarReflectionEffect::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = Effect::GetDiskUsed(rkVersion) + sizeof(m_iQuantity);

    if (m_iQuantity > 0)
    {
        iSize += m_iQuantity*sizeof(m_afReflectance[0]);
        iSize += m_iQuantity*WM4_PTRSIZE(m_aspkPlane[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* PlanarReflectionEffect::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("plane quantity =",m_iQuantity));

    const size_t uiTitleSize = 32;
    char acTitle[uiTitleSize];
    int i;
    for (i = 0; i < m_iQuantity; i++)
    {
        System::Sprintf(acTitle,uiTitleSize,"reflectance[%d] =",i);
        pkTree->Append(Format(acTitle,m_afReflectance[i]));
    }

    // children
    pkTree->Append(Effect::SaveStrings());
    for (i = 0; i < m_iQuantity; i++)
    {
        pkTree->Append(m_aspkPlane[i]->SaveStrings());
    }

    return pkTree;
}
//----------------------------------------------------------------------------
