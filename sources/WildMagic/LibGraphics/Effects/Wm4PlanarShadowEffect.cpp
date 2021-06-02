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
#include "Wm4PlanarShadowEffect.h"
#include "Wm4VisibleSet.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,PlanarShadowEffect,Effect);
WM4_IMPLEMENT_STREAM(PlanarShadowEffect);

//----------------------------------------------------------------------------
PlanarShadowEffect::PlanarShadowEffect (int iQuantity)
{
    assert(iQuantity > 0);
    m_iQuantity = iQuantity;
    m_aspkPlane = WM4_NEW TriMeshPtr[m_iQuantity];
    m_aspkProjector = WM4_NEW LightPtr[m_iQuantity];
    m_akShadowColor = WM4_NEW ColorRGBA[m_iQuantity];

    m_spkAState = WM4_NEW AlphaState;
    m_spkMState = WM4_NEW MaterialState;
    m_spkSState = WM4_NEW StencilState;
    m_spkZState = WM4_NEW ZBufferState;
    m_spkMEffect = WM4_NEW MaterialEffect;
}
//----------------------------------------------------------------------------
PlanarShadowEffect::PlanarShadowEffect ()
{
    m_iQuantity = 0;
    m_aspkPlane = 0;
    m_aspkProjector = 0;
    m_akShadowColor = 0;

    m_spkAState = WM4_NEW AlphaState;
    m_spkMState = WM4_NEW MaterialState;
    m_spkSState = WM4_NEW StencilState;
    m_spkZState = WM4_NEW ZBufferState;
    m_spkMEffect = WM4_NEW MaterialEffect;
}
//----------------------------------------------------------------------------
PlanarShadowEffect::~PlanarShadowEffect ()
{
    WM4_DELETE[] m_aspkPlane;
    WM4_DELETE[] m_aspkProjector;
    WM4_DELETE[] m_akShadowColor;
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::Draw (Renderer* pkRenderer, Spatial* pkGlobalObject,
    int iMin, int iMax, VisibleObject* akVisible)
{
    // Draw the potentially visible portions of the shadow caster.
    int j;
    for (j = iMin; j <= iMax; j++)
    {
        if (akVisible[j].IsDrawable())
        {
            pkRenderer->Draw((Geometry*)akVisible[j].Object);
        }
    }

    // The number of planes is limited by the size of a value in the stencil
    // buffer (256 for an 8-bit stencil buffer).
    int iQuantity = m_iQuantity;
    if (iQuantity >= pkRenderer->GetMaxStencilIndices())
    {
        iQuantity = pkRenderer->GetMaxStencilIndices() - 1;
    }

    // Save the current global state for restoration later.
    AlphaStatePtr spkSaveAState = pkRenderer->GetAlphaState();
    MaterialStatePtr spkSaveMState = pkRenderer->GetMaterialState();
    StencilStatePtr spkSaveSState = pkRenderer->GetStencilState();
    ZBufferStatePtr spkSaveZState = pkRenderer->GetZBufferState();

    for (int i = 0; i < iQuantity; i++)
    {
        // Enable depth buffering.  NOTE: The plane object should not have a
        // ZBufferState object that changes the current settings.
        m_spkZState->Enabled = true;
        pkRenderer->SetZBufferState(m_spkZState);

        // Enable the stencil buffer so that the shadow can be clipped by the
        // plane.  The stencil values are set whenever the corresponding
        // plane pixels are visible.
        m_spkSState->Enabled = true;
        m_spkSState->Compare = StencilState::CF_ALWAYS;
        m_spkSState->Reference = (unsigned int)(i+1);
        m_spkSState->OnFail = StencilState::OT_KEEP;      // irrelevant
        m_spkSState->OnZFail = StencilState::OT_KEEP;     // invisible kept 0
        m_spkSState->OnZPass = StencilState::OT_REPLACE;  // visible to i+1
        pkRenderer->SetStencilState(m_spkSState);

        // Draw the plane.
        pkRenderer->Draw(m_aspkPlane[i]);

        // Get the projection matrix relative to the projector (light).
        Matrix4f kProjection;
        if (!GetProjectionMatrix(i,pkGlobalObject->WorldBound,kProjection))
        {
            continue;
        }

        pkRenderer->SetPostWorldTransformation(kProjection);

        // Blend the shadow color with the pixels drawn on the projection
        // plane.  The blending equation is
        //   (rf,gf,bf) = as*(rs,gs,bs) + (1-as)*(rd,gd,bd)
        // where (rf,gf,bf) is the final color to be written to the frame
        // buffer, (rs,gs,bs,as) is the shadow color, and (rd,gd,bd) is the
        // current color of the frame buffer.
        m_spkAState->BlendEnabled = true;
        m_spkAState->SrcBlend = AlphaState::SBF_SRC_ALPHA;
        m_spkAState->DstBlend = AlphaState::DBF_ONE_MINUS_SRC_ALPHA;
        pkRenderer->SetAlphaState(m_spkAState);
        m_spkMState->Diffuse[0] = m_akShadowColor[i][0];
        m_spkMState->Diffuse[1] = m_akShadowColor[i][1];
        m_spkMState->Diffuse[2] = m_akShadowColor[i][2];
        m_spkMState->Alpha = m_akShadowColor[i][3];
        pkRenderer->SetMaterialState(m_spkMState);

        // Disable the depth buffer reading so that no depth-buffer fighting
        // occurs.  The drawing of pixels is controlled solely by the stencil
        // value.
        m_spkZState->Enabled = false;
        pkRenderer->SetZBufferState(m_spkZState);

        // Only draw where the plane has been drawn.
        m_spkSState->Enabled = true;
        m_spkSState->Compare = StencilState::CF_EQUAL;
        m_spkSState->Reference = (unsigned int)(i+1);
        m_spkSState->OnFail = StencilState::OT_KEEP;   // invisible kept 0
        m_spkSState->OnZFail = StencilState::OT_KEEP;  // irrelevant
        m_spkSState->OnZPass = StencilState::OT_ZERO;  // visible set to 0
        pkRenderer->SetStencilState(m_spkSState);

        // Draw the caster again, but temporarily use a material effect so
        // that the shadow color is blended onto the plane.  TO DO:  This
        // drawing pass should use a VisibleSet relative to the projector so
        // that objects that are out of view (i.e. culled relative to the
        // camera and not in the camera's VisibleSet) can cast shadows.
        for (j = iMin; j <= iMax; j++)
        {
            if (akVisible[j].IsDrawable())
            {
                Geometry* pkGeom = (Geometry*)akVisible[j].Object;

                GlobalStatePtr spkSaveMaterialState =
                    pkGeom->States[GlobalState::MATERIAL];
                pkGeom->States[GlobalState::MATERIAL] = 0;

                pkGeom->AttachEffect(m_spkMEffect);
                pkGeom->SetStartEffect(pkGeom->GetEffectQuantity()-1);

                pkRenderer->Draw((Geometry*)akVisible[j].Object);

                pkGeom->SetStartEffect(0);
                pkGeom->DetachEffect(m_spkMEffect);

                pkGeom->States[GlobalState::MATERIAL] = spkSaveMaterialState;
            }
        }

        // Disable the stencil buffer and alpha blending.
        m_spkSState->Enabled = false;
        pkRenderer->SetStencilState(m_spkSState);
        m_spkAState->BlendEnabled = false;
        pkRenderer->SetAlphaState(m_spkAState);

        pkRenderer->RestorePostWorldTransformation();
    }

    // Restore the global state that existed before this function call.
    pkRenderer->SetAlphaState(spkSaveAState);
    pkRenderer->SetMaterialState(spkSaveMState);
    pkRenderer->SetStencilState(spkSaveSState);
    pkRenderer->SetZBufferState(spkSaveZState);
}
//----------------------------------------------------------------------------
bool PlanarShadowEffect::GetProjectionMatrix (int i,
    const BoundingVolume* pkGlobalObjectWorldBound, Matrix4f& rkProjection)
{
    // Compute the equation for the shadow plane in world coordinates.
    Triangle3f kTri;
    m_aspkPlane[i]->GetWorldTriangle(0,kTri);
    Plane3f kPlane(kTri.V[0],kTri.V[1],kTri.V[2]);

    // This is a conservative test to see if a shadow should be cast.  This
    // can/ cause incorrect results if the caster is large and intersects the
    // plane, but ordinarily we are not trying to cast shadows in such
    // situations.
    if (pkGlobalObjectWorldBound->WhichSide(kPlane) < 0)
    {
        // The shadow caster is on the far side of plane, so it cannot cast
        // a shadow.
        return false;
    }

    // Compute the projection matrix for the light source.
    Light* pkProjector = m_aspkProjector[i];
    if (pkProjector->Type == Light::LT_DIRECTIONAL)
    {
        float fNdD = kPlane.Normal.Dot(pkProjector->DVector);
        if (fNdD >= 0.0f)
        {
            // The projection must be onto the "positive side" of the plane.
            return false;
        }

        rkProjection.MakeObliqueProjection(kPlane.Normal,kTri.V[0],
            pkProjector->DVector);
    }
    else if (pkProjector->Type == Light::LT_POINT
    || pkProjector->Type == Light::LT_SPOT )
    {
        float fNdE = kPlane.Normal.Dot(pkProjector->Position);
        if (fNdE <= 0.0f)
        {
            // The projection must be onto the "positive side" of the plane.
            return false;
        }

        rkProjection.MakePerspectiveProjection(kPlane.Normal,kTri.V[0],
            pkProjector->Position);
    }
    else
    {
        assert(false);
        return false;
    }

    return true;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* PlanarShadowEffect::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = Effect::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    int i;

    if (m_aspkPlane)
    {
        for (i = 0; i < m_iQuantity; i++)
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

    if (m_aspkProjector)
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if (m_aspkProjector[i])
            {
                pkFound = m_aspkProjector[i]->GetObjectByName(rkName);
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
void PlanarShadowEffect::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    Effect::GetAllObjectsByName(rkName,rkObjects);

    int i;

    if (m_aspkPlane)
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if (m_aspkPlane[i])
            {
                m_aspkPlane[i]->GetAllObjectsByName(rkName,rkObjects);
            }
        }
    }

    if (m_aspkProjector)
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if (m_aspkProjector[i])
            {
                m_aspkProjector[i]->GetAllObjectsByName(rkName,rkObjects);
            }
        }
    }
}
//----------------------------------------------------------------------------
Object* PlanarShadowEffect::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Effect::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    int i;

    if (m_aspkPlane)
    {
        for (i = 0; i < m_iQuantity; i++)
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

    if (m_aspkProjector)
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if (m_aspkProjector[i])
            {
                pkFound = m_aspkProjector[i]->GetObjectByID(uiID);
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
void PlanarShadowEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Effect::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iQuantity);

    if (m_iQuantity > 0)
    {
        m_aspkPlane = WM4_NEW TriMeshPtr[m_iQuantity];
        m_aspkProjector = WM4_NEW LightPtr[m_iQuantity];
        m_akShadowColor = WM4_NEW ColorRGBA[m_iQuantity];

        // native data
        rkStream.Read(m_iQuantity,m_akShadowColor);

        // link data
        Object* pkObject;
        int i;
        for (i = 0; i < m_iQuantity; i++)
        {
            rkStream.Read(pkObject);  // m_aspkPlane[i]
            pkLink->Add(pkObject);
        }
        for (i = 0; i < m_iQuantity; i++)
        {
            rkStream.Read(pkObject);  // m_aspkProjector[i]
            pkLink->Add(pkObject);
        }
    }

    WM4_END_DEBUG_STREAM_LOAD(PlanarShadowEffect);
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Effect::Link(rkStream,pkLink);

    int i;
    for (i = 0; i < m_iQuantity; i++)
    {
        Object* pkLinkID = pkLink->GetLinkID();
        m_aspkPlane[i] = (TriMesh*)rkStream.GetFromMap(pkLinkID);
    }
    for (i = 0; i < m_iQuantity; i++)
    {
        Object* pkLinkID = pkLink->GetLinkID();
        m_aspkProjector[i] = (Light*)rkStream.GetFromMap(pkLinkID);
    }
}
//----------------------------------------------------------------------------
bool PlanarShadowEffect::Register (Stream& rkStream) const
{
    if (!Effect::Register(rkStream))
    {
        return false;
    }

    int i;

    if (m_aspkPlane)
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if (m_aspkPlane[i])
            {
                m_aspkPlane[i]->Register(rkStream);
            }
        }
    }

    if (m_aspkProjector)
    {
        for (i = 0; i < m_iQuantity; i++)
        {
            if (m_aspkProjector[i])
            {
                m_aspkProjector[i]->Register(rkStream);
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void PlanarShadowEffect::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Effect::Save(rkStream);

    // native data
    rkStream.Write(m_iQuantity);
    if (m_iQuantity > 0)
    {
        rkStream.Write(m_iQuantity,m_akShadowColor);

        // link data
        ObjectPtr* aspkArray = (ObjectPtr*)m_aspkPlane;
        //rkStream.Write(m_iQuantity,aspkArray);
        int i;
        for (i = 0; i < m_iQuantity; i++)
        {
            rkStream.Write(aspkArray[i]);
        }

        aspkArray = (ObjectPtr*)m_aspkProjector;
        //rkStream.Write(m_iQuantity,aspkArray);
        for (i = 0; i < m_iQuantity; i++)
        {
            rkStream.Write(aspkArray[i]);
        }
    }

    WM4_END_DEBUG_STREAM_SAVE(PlanarShadowEffect);
}
//----------------------------------------------------------------------------
int PlanarShadowEffect::GetDiskUsed (const StreamVersion& rkVersion) const
{
    int iSize = Effect::GetDiskUsed(rkVersion) + sizeof(m_iQuantity);

    if (m_iQuantity > 0)
    {
        iSize += m_iQuantity*sizeof(m_akShadowColor[0]);
        iSize += m_iQuantity*WM4_PTRSIZE(m_aspkPlane[0]);
        iSize += m_iQuantity*WM4_PTRSIZE(m_aspkProjector[0]);
    }

    return iSize;
}
//----------------------------------------------------------------------------
StringTree* PlanarShadowEffect::SaveStrings (const char*)
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
        System::Sprintf(acTitle,uiTitleSize,"shadow color[%d] =",i);
        pkTree->Append(Format(acTitle,m_akShadowColor[i]));
    }

    // children
    pkTree->Append(Effect::SaveStrings());
    for (i = 0; i < m_iQuantity; i++)
    {
        pkTree->Append(m_aspkPlane[i]->SaveStrings());
        pkTree->Append(m_aspkProjector[i]->SaveStrings());
    }

    return pkTree;
}
//----------------------------------------------------------------------------
