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
// Version: 4.0.4 (2008/02/18)

#include "CubeMapEffect.h"
#include "Wm4Culler.h"
#include "Wm4FrameBuffer.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,CubeMapEffect,ShaderEffect);
WM4_IMPLEMENT_STREAM(CubeMapEffect);

//----------------------------------------------------------------------------
CubeMapEffect::CubeMapEffect (Image* pkXpImage, Image* pkXmImage,
    Image* pkYpImage, Image* pkYmImage, Image* pkZpImage, Image* pkZmImage,
    float fReflectivity, bool bAllowDynamicUpdates)
    :
    ShaderEffect(1)
{
    // Assertions:
    // 1. All input pointers are not null.
    // 2. All input images have format Image::IT_RGB888.
    // 3. All input images are 2D, square, and all have the same common
    //    bound.

    m_afReflectivity[0] = fReflectivity;

    m_kVShader[0] = WM4_NEW VertexShader("CubeMap");
    m_kPShader[0] = WM4_NEW PixelShader("CubeMap");

    // Allocate the cube map image to store the inputs.  The images must be
    // square, all of the same size, and all 24-bit RGB.  TO DO:  Describe
    // how the input images relate to a cube with the camera at center.  Also
    // describe why the x and y reflections must be made to the cube map
    // image.
    int iSize = pkXpImage->GetBound(0);
    int iBytesPerPixel = pkXpImage->GetBytesPerPixel();
    int iQuantity = iBytesPerPixel*iSize*iSize;
    unsigned char* aucData = WM4_NEW unsigned char[6*iQuantity];

    std::string kHexaName =
        std::string(pkXpImage->GetName()) +
        std::string(pkXmImage->GetName()) +
        std::string(pkYpImage->GetName()) +
        std::string(pkYmImage->GetName()) +
        std::string(pkZpImage->GetName()) +
        std::string(pkZmImage->GetName());

    m_spkHexaImage = WM4_NEW Image(Image::IT_CUBE_RGB888,iSize,iSize,aucData,
        kHexaName.c_str());

    size_t uiSize = iQuantity*sizeof(unsigned char);
    System::Memcpy(&aucData[0*iQuantity],uiSize,pkXpImage->GetData(),uiSize);
    System::Memcpy(&aucData[1*iQuantity],uiSize,pkXmImage->GetData(),uiSize);
    System::Memcpy(&aucData[2*iQuantity],uiSize,pkYpImage->GetData(),uiSize);
    System::Memcpy(&aucData[3*iQuantity],uiSize,pkYmImage->GetData(),uiSize);
    System::Memcpy(&aucData[4*iQuantity],uiSize,pkZpImage->GetData(),uiSize);
    System::Memcpy(&aucData[5*iQuantity],uiSize,pkZmImage->GetData(),uiSize);

    ReflectXY(iSize,iSize,iBytesPerPixel,&aucData[0*iQuantity]);
    ReflectXY(iSize,iSize,iBytesPerPixel,&aucData[1*iQuantity]);
    ReflectXY(iSize,iSize,iBytesPerPixel,&aucData[4*iQuantity]);
    ReflectXY(iSize,iSize,iBytesPerPixel,&aucData[5*iQuantity]);

    // The texture of the pixel shader will manage the cube map.
    m_kPShader[0]->SetTexture(0,kHexaName.c_str());
    GetPTexture(0,0)->SetFilterType(Texture::LINEAR_LINEAR);

    if (bAllowDynamicUpdates)
    {
        m_spkCamera = WM4_NEW Camera;
        m_pkCuller = WM4_NEW Culler;
        m_pkCuller->SetCamera(m_spkCamera);

        // Create an image to store an updated face of the cube map.
        aucData = WM4_NEW unsigned char[iQuantity];
        m_spkCubeFace = WM4_NEW Image(Image::IT_RGB888,iSize,iSize,aucData,
            "CubeFace");

        // Create a texture to own the image.
        m_pkCubeFaceTexture = WM4_NEW Texture("CubeFace",m_spkCubeFace);
        m_pkCubeFaceTexture->SetOffscreenTexture(true);

        // The creation of the color buffer is deferred until the first
        // UpdateFaces call, at which time a renderer is known and used for
        // the creation.
        m_pkCubeFaceBuffer = 0;
    }
    else
    {
        m_pkCuller = 0;
        m_pkCubeFaceTexture = 0;
        m_pkCubeFaceBuffer = 0;
    }

    ConnectPShaderConstant(0,"Reflectivity",m_afReflectivity);
}
//----------------------------------------------------------------------------
CubeMapEffect::CubeMapEffect ()
{
    m_pkCuller = 0;
    m_pkCubeFaceTexture = 0;
    m_pkCubeFaceBuffer = 0;
}
//----------------------------------------------------------------------------
CubeMapEffect::~CubeMapEffect ()
{
    WM4_DELETE m_pkCuller;
    WM4_DELETE m_pkCubeFaceTexture;
    WM4_DELETE m_pkCubeFaceBuffer;
}
//----------------------------------------------------------------------------
void CubeMapEffect::Draw (Renderer* pkRenderer, Spatial*,
    int iVisibleQuantity, VisibleObject* akVisible)
{
    VisibleObject* pkCurrent = akVisible;
    for (int i = 0; i < iVisibleQuantity; i++, pkCurrent++)
    {
        pkRenderer->Draw((Geometry*)pkCurrent->Object);
    }
}
//----------------------------------------------------------------------------
void CubeMapEffect::UpdateFaces (Renderer* pkRenderer, Spatial* pkScene,
    const Vector3f& rkEnvOrigin, const Vector3f& rkEnvDVector,
    const Vector3f& rkEnvUVector, const Vector3f& rkEnvRVector)
{
    if (!m_spkCubeFace)
    {
        // Dynamic updates were not enabled in the constructor.
        return;
    }

    if (!m_pkCubeFaceBuffer)
    {
        // This is the first call to UpdateFaces.  Since the renderer is now
        // known, it is safe to create the framebuffer object.
        m_pkCubeFaceTexture->SetOffscreenTexture(true);
        pkRenderer->LoadTexture(m_pkCubeFaceTexture);
        m_pkCubeFaceBuffer = FrameBuffer::Create(FrameBuffer::FT_FORMAT_RGB,
            FrameBuffer::DT_DEPTH_NONE,FrameBuffer::ST_STENCIL_NONE,
            FrameBuffer::BT_BUFFERED_SINGLE,FrameBuffer::MT_SAMPLING_NONE,
            pkRenderer,m_pkCubeFaceTexture);
        assert(m_pkCubeFaceBuffer);
    }

    // The camera is oriented six times along the coordinate axes and using
    // a frustum with a 90-degree field of view.  Save the current camera
    // for restoration at the end of this function.
    Camera* pkCamera = pkRenderer->GetCamera();
    m_spkCamera->SetFrustum(90.0f,1.0f,pkCamera->GetDMin(),
        pkCamera->GetDMax());
    pkRenderer->SetCamera(m_spkCamera);

    // The field of view is 90 degrees for each face, the aspect ratio is 1
    // since the cube faces are square, and the current near/far distances
    // are used.

    m_pkCubeFaceBuffer->Enable();

    // +x direction
    m_spkCamera->SetAxes(-rkEnvRVector,rkEnvUVector,rkEnvDVector);
    m_pkCuller->ComputeVisibleSet(pkScene);
    pkRenderer->ClearBuffers();
    pkRenderer->DrawScene(m_pkCuller->GetVisibleSet());
    m_pkCubeFaceBuffer->CopyToTexture(true);
    UpdateFace(0,m_pkCubeFaceTexture->GetImage());

    // -x direction
    m_spkCamera->SetAxes(rkEnvRVector,rkEnvUVector,-rkEnvDVector);
    m_pkCuller->ComputeVisibleSet(pkScene);
    pkRenderer->ClearBuffers();
    pkRenderer->DrawScene(m_pkCuller->GetVisibleSet());
    m_pkCubeFaceBuffer->CopyToTexture(true);
    UpdateFace(1,m_pkCubeFaceTexture->GetImage());

    // +y direction
    m_spkCamera->SetAxes(rkEnvUVector,-rkEnvDVector,rkEnvRVector);
    m_pkCuller->ComputeVisibleSet(pkScene);
    pkRenderer->ClearBuffers();
    pkRenderer->DrawScene(m_pkCuller->GetVisibleSet());
    m_pkCubeFaceBuffer->CopyToTexture(true);
    UpdateFace(2,m_pkCubeFaceTexture->GetImage());

    // -y direction
    m_spkCamera->SetAxes(-rkEnvUVector,rkEnvDVector,rkEnvRVector);
    m_pkCuller->ComputeVisibleSet(pkScene);
    pkRenderer->ClearBuffers();
    pkRenderer->DrawScene(m_pkCuller->GetVisibleSet());
    m_pkCubeFaceBuffer->CopyToTexture(true);
    UpdateFace(3,m_pkCubeFaceTexture->GetImage());

    // +z direction
    m_spkCamera->SetAxes(rkEnvDVector,rkEnvUVector,rkEnvRVector);
    m_pkCuller->ComputeVisibleSet(pkScene);
    pkRenderer->ClearBuffers();
    pkRenderer->DrawScene(m_pkCuller->GetVisibleSet());
    m_pkCubeFaceBuffer->CopyToTexture(true);
    UpdateFace(4,m_pkCubeFaceTexture->GetImage());

    // -z direction
    m_spkCamera->SetAxes(-rkEnvDVector,rkEnvUVector,-rkEnvRVector);
    m_pkCuller->ComputeVisibleSet(pkScene);
    pkRenderer->ClearBuffers();
    pkRenderer->DrawScene(m_pkCuller->GetVisibleSet());
    m_pkCubeFaceBuffer->CopyToTexture(true);
    UpdateFace(5,m_pkCubeFaceTexture->GetImage());

    m_pkCubeFaceBuffer->Disable();

    // Request a refresh of VRAM with the new cube map.
    GetPTexture(0,0)->Release();

    // Restore the original camera and propagate the information to the
    // renderer.
    pkRenderer->SetCamera(pkCamera);
}
//----------------------------------------------------------------------------
void CubeMapEffect::UpdateFace (int iFace, const Image* pkImage)
{
    int iSize = pkImage->GetBound(0);
    int iBytesPerPixel = pkImage->GetBytesPerPixel();
    int iQuantity = iBytesPerPixel*iSize*iSize;
    int iIndex = iFace*iQuantity;
    const unsigned char* aucSrc = pkImage->GetData();
    unsigned char* aucTrg = &m_spkHexaImage->GetData()[iIndex];

    System::Memcpy(aucTrg,iQuantity,aucSrc,iQuantity);
    ReflectXY(iSize,iSize,iBytesPerPixel,aucTrg);
}
//----------------------------------------------------------------------------
void CubeMapEffect::ReflectXY (int iXSize, int iYSize, int iBytesPerPixel,
    unsigned char* aucData)
{
    int iX, iY, iFlipX, iFlipY, iSrc, iTrg, i;
    unsigned char ucSave;

    for (iX = 0; iX < iXSize/2; iX++)
    {
        iFlipX = iXSize - 1 - iX;
        for (iY = 0; iY < iYSize; iY++)
        {
            iSrc = iBytesPerPixel*(iX + iXSize*iY);
            iTrg = iBytesPerPixel*(iFlipX + iXSize*iY);
            for (i = 0; i < iBytesPerPixel; i++)
            {
                ucSave = aucData[iSrc+i];
                aucData[iSrc+i] = aucData[iTrg+i];
                aucData[iTrg+i] = ucSave;
            }
        }
    }

    for (iY = 0; iY < iYSize/2; iY++)
    {
        iFlipY = iYSize - 1 - iY;
        for (iX = 0; iX < iXSize; iX++)
        {
            iSrc = iBytesPerPixel*(iX + iXSize*iY);
            iTrg = iBytesPerPixel*(iX + iXSize*iFlipY);
            for (i = 0; i < iBytesPerPixel; i++)
            {
                ucSave = aucData[iSrc+i];
                aucData[iSrc+i] = aucData[iTrg+i];
                aucData[iTrg+i] = ucSave;
            }
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* CubeMapEffect::GetObjectByName (const std::string& rkName)
{
    Object* pkFound = ShaderEffect::GetObjectByName(rkName);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_spkHexaImage)
    {
        pkFound = m_spkHexaImage->GetObjectByName(rkName);
        if (pkFound)
        {
            return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void CubeMapEffect::GetAllObjectsByName (const std::string& rkName,
    std::vector<Object*>& rkObjects)
{
    ShaderEffect::GetAllObjectsByName(rkName,rkObjects);

    if (m_spkHexaImage)
    {
        m_spkHexaImage->GetAllObjectsByName(rkName,rkObjects);
    }
}
//----------------------------------------------------------------------------
Object* CubeMapEffect::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = ShaderEffect::GetObjectByID(uiID);
    if (pkFound)
    {
        return pkFound;
    }

    if (m_spkHexaImage)
    {
        pkFound = m_spkHexaImage->GetObjectByID(uiID);
        if (pkFound)
        {
            return pkFound;
        }
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void CubeMapEffect::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    ShaderEffect::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_afReflectivity[0]);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkHexaImage
    pkLink->Add(pkObject);

    WM4_END_DEBUG_STREAM_LOAD(CubeMapEffect);
}
//----------------------------------------------------------------------------
void CubeMapEffect::Link (Stream& rkStream, Stream::Link* pkLink)
{
    ShaderEffect::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkHexaImage = (Image*)rkStream.GetFromMap(pkLinkID);
}
//----------------------------------------------------------------------------
bool CubeMapEffect::PostLink ()
{
    ConnectPShaderConstant(0,"Reflectivity",m_afReflectivity);
    return GetPConstant(0,0) != 0;
}
//----------------------------------------------------------------------------
bool CubeMapEffect::Register (Stream& rkStream) const
{
    if (!ShaderEffect::Register(rkStream))
    {
        return false;
    }

    if (m_spkHexaImage)
    {
        m_spkHexaImage->Register(rkStream);
    }

    return true;
}
//----------------------------------------------------------------------------
void CubeMapEffect::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    ShaderEffect::Save(rkStream);

    // native data
    rkStream.Write(m_afReflectivity[0]);

    // link data
    rkStream.Write(m_spkHexaImage);

    WM4_END_DEBUG_STREAM_SAVE(CubeMapEffect);
}
//----------------------------------------------------------------------------
int CubeMapEffect::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return ShaderEffect::GetDiskUsed(rkVersion) +
        sizeof(m_afReflectivity[0]) +
        WM4_PTRSIZE(m_spkHexaImage);
}
//----------------------------------------------------------------------------
StringTree* CubeMapEffect::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("reflectivity =",m_afReflectivity[0]));

    // children
    pkTree->Append(ShaderEffect::SaveStrings());

    if (m_spkHexaImage)
    {
        pkTree->Append(m_spkHexaImage->SaveStrings());
    }

    return pkTree;
}
//----------------------------------------------------------------------------
