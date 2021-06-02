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
// Version: 4.0.6 (2008/01/20)

#include "Wm4OpenGLRendererPCH.h"
#include "Wm4OpenGLRenderer.h"
#include "Wm4OpenGLResources.h"
using namespace Wm4;

GLenum OpenGLRenderer::ms_aeMapTypes[3] =
{
    GL_READ_ONLY,   // Renderer::DT_READ
    GL_WRITE_ONLY,  // Renderer::DT_WRITE
    GL_READ_WRITE   // Renderer::DT_READ_WRITE
};

//----------------------------------------------------------------------------
void OpenGLRenderer::SetVProgramConstant (int eCType, int iBaseRegister,
    int iRegisterQuantity, float* afData)
{
    if (eCType != Renderer::CT_NUMERICAL)
    {
        for (int j = 0; j < iRegisterQuantity; j++)
        {
            glProgramLocalParameter4fvARB(GL_VERTEX_PROGRAM_ARB,
                (GLuint)iBaseRegister,afData);
            iBaseRegister++;
            afData += 4;
        }
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::SetPProgramConstant (int eCType, int iBaseRegister,
    int iRegisterQuantity, float* afData)
{
    if (eCType != Renderer::CT_NUMERICAL)
    {
        for (int j = 0; j < iRegisterQuantity; j++)
        {
            glProgramLocalParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB,
                (GLuint)iBaseRegister,afData);
            iBaseRegister++;
            afData += 4;
        }
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnEnableVProgram (ResourceIdentifier* pkID)
{
    VProgramID* pkResource = (VProgramID*)pkID;
    glEnable(GL_VERTEX_PROGRAM_ARB);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB,pkResource->ID);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnDisableVProgram (ResourceIdentifier*)
{
    glDisable(GL_VERTEX_PROGRAM_ARB);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnEnablePProgram (ResourceIdentifier* pkID)
{
    PProgramID* pkResource = (PProgramID*)pkID;
    glEnable(GL_FRAGMENT_PROGRAM_ARB);
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,pkResource->ID);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnDisablePProgram (ResourceIdentifier*)
{
    glDisable(GL_FRAGMENT_PROGRAM_ARB);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnEnableTexture (ResourceIdentifier* pkID)
{
    SamplerInformation* pkSI = m_apkActiveSamplers[m_iCurrentSampler];
    SamplerInformation::Type eSType = pkSI->GetType();
    int iTextureUnit = pkSI->GetTextureUnit();
    int eTarget = ms_aeSamplerTypes[eSType];

    TextureID* pkResource = (TextureID*)pkID;
    glActiveTexture(GL_TEXTURE0 + iTextureUnit);
    glBindTexture(eTarget,pkResource->ID);
    if (pkResource->PBOHandle > 0)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,pkResource->PBOHandle);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnDisableTexture (ResourceIdentifier* pkID)
{
    TextureID* pkResource = (TextureID*)pkID;
    if (pkResource->PBOHandle > 0)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,0);
    }
}
//----------------------------------------------------------------------------
void* OpenGLRenderer::Lock (DynamicTexture* pkTexture, unsigned int eAccess)
{
    assert(pkTexture);
    ResourceIdentifier* pkID = pkTexture->GetIdentifier(this);
    assert(pkID);
    TextureID* pkResource = (TextureID*)pkID;
    assert(pkResource->PBOHandle > 0);

    const Image* pkImage = pkTexture->GetImage();
    assert(pkImage && !pkImage->IsCubeImage());
    (void)pkImage;  // avoid warning in release configurations

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pkResource->PBOHandle);
    m_pkLockedTexture = pkTexture;
    m_iLockedFace = 0;
    return glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, ms_aeMapTypes[eAccess]);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::Unlock (DynamicTexture* pkTexture)
{
    assert(pkTexture == m_pkLockedTexture && m_iLockedFace == 0);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);

    ResourceIdentifier* pkID = pkTexture->GetIdentifier(this);
    assert(pkID);
    TextureID* pkResource = (TextureID*)pkID;
    assert(pkResource->PBOHandle > 0);

    const Image* pkImage = pkTexture->GetImage();
    assert(pkImage && !pkImage->IsCubeImage());
    int iDimension = pkImage->GetDimension();
    int eFormat = ms_aeImageFormats[pkImage->GetFormat()];
    int eIType = ms_aeImageTypes[pkImage->GetFormat()];
    int eTarget = ms_aeSamplerTypes[iDimension-1];

    glBindTexture(eTarget,pkResource->ID);

    switch (iDimension)
    {
    case 1:
        glTexSubImage1D(eTarget,0,0,pkImage->GetBound(0),eFormat,eIType,0);
        break;

    case 2:
        glTexSubImage2D(eTarget,0,0,0,pkImage->GetBound(0),
            pkImage->GetBound(1),eFormat,eIType,0);
        break;

    case 3:
        glTexSubImage3D(eTarget,0,0,0,0,pkImage->GetBound(0),
            pkImage->GetBound(1),pkImage->GetBound(2),eFormat,eIType,0);
        break;

    default:
        assert(false);
        break;
    }

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    m_pkLockedTexture = 0;
    m_iLockedFace = -1;
}
//----------------------------------------------------------------------------
void* OpenGLRenderer::Lock (int iFace, DynamicTexture* pkTexture,
    unsigned int eAccess)
{
    assert(0 <= iFace && iFace < 6 && pkTexture);
    ResourceIdentifier* pkID = pkTexture->GetIdentifier(this);
    assert(pkID);
    TextureID* pkResource = (TextureID*)pkID;
    assert(pkResource->PBOHandle > 0);

    const Image* pkImage = pkTexture->GetImage();
    assert(pkImage && pkImage->IsCubeImage());
    int iNumFaceBytes = pkImage->GetBytesPerPixel()*pkImage->GetQuantity();

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pkResource->PBOHandle);

    unsigned char* pucData = (unsigned char*)
        glMapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,ms_aeMapTypes[eAccess]);
    m_pkLockedTexture = pkTexture;
    m_iLockedFace = iFace;
    return pucData + iFace*iNumFaceBytes;
}
//----------------------------------------------------------------------------
void OpenGLRenderer::Unlock (int iFace, DynamicTexture* pkTexture)
{
    assert(pkTexture && pkTexture == m_pkLockedTexture);
    assert(iFace == m_iLockedFace && 0 <= iFace && iFace < 6);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER_ARB);

    ResourceIdentifier* pkID = pkTexture->GetIdentifier(this);
    assert(pkID);
    TextureID* pkResource = (TextureID*)pkID;
    assert(pkResource->PBOHandle > 0);

    const Image* pkImage = pkTexture->GetImage();
    assert(pkImage && pkImage->IsCubeImage());
    int iNumFaceBytes = pkImage->GetBytesPerPixel()*pkImage->GetQuantity();
    int eFormat = ms_aeImageFormats[pkImage->GetFormat()];
    int eIType = ms_aeImageTypes[pkImage->GetFormat()];

    glBindTexture(GL_TEXTURE_CUBE_MAP,pkResource->ID);

    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + iFace,0,0,0,
        pkImage->GetBound(0),pkImage->GetBound(1),eFormat,eIType,
        (const unsigned char*)0 + iFace*iNumFaceBytes);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    m_pkLockedTexture = 0;
    m_iLockedFace = -1;
}
//----------------------------------------------------------------------------
// Some OpenGL 2.x drivers are not handling normal attributes correctly.  This
// is a problem should you want to use the normal vector to pass a 4-tuple of
// information to the shader.  The OpenGL 1.x glNormalPointer assumes the
// normals are 3-tuples.  If you know that your target machines correctly
// support OpenGL 2.x normal attributes, expose the following #define.
// Otherwise, the renderer will use the OpenGL 1.x glNormalPointer.
//
#define USE_OPENGL2_NORMAL_ATTRIBUTES

void OpenGLRenderer::OnEnableVBuffer (ResourceIdentifier* pkID)
{
    // Bind the current vertex buffer.
    VBufferID* pkResource = (VBufferID*)pkID;
    glBindBuffer(GL_ARRAY_BUFFER,pkResource->ID);

    const Attributes& rkRAttr = pkResource->IAttr;
    GLsizei iSize = (GLsizei)(sizeof(float)*rkRAttr.GetChannelQuantity());
    const float* afData = 0;

    if (rkRAttr.HasPosition())
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(rkRAttr.GetPositionChannels(),GL_FLOAT,iSize,
            afData + rkRAttr.GetPositionOffset());
    }

    if (rkRAttr.HasBlendWeight())
    {
        glEnableClientState(GL_WEIGHT_ARRAY_ARB);
        glWeightPointerARB(rkRAttr.GetBlendWeightChannels(),GL_FLOAT,iSize,
            afData + rkRAttr.GetBlendWeightOffset());
    }

    if (rkRAttr.HasNormal())
    {
#ifdef USE_OPENGL2_NORMAL_ATTRIBUTES
        glEnableVertexAttribArrayARB(2);
        glVertexAttribPointerARB(2,rkRAttr.GetNormalChannels(),GL_FLOAT,
            GL_FALSE,iSize,afData + rkRAttr.GetNormalOffset());
#else
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT,iSize,afData + rkRAttr.GetNormalOffset());
#endif
    }

    if (rkRAttr.HasColor(0))
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(rkRAttr.GetColorChannels(0),GL_FLOAT,iSize,
            afData + rkRAttr.GetColorOffset(0));
    }

    if (rkRAttr.HasColor(1))
    {
        glEnableClientState(GL_SECONDARY_COLOR_ARRAY);
        glSecondaryColorPointer(rkRAttr.GetColorChannels(1),GL_FLOAT,iSize,
            afData + rkRAttr.GetColorOffset(1));
    }

    if (rkRAttr.HasFog())
    {
        glEnableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
        glFogCoordPointerEXT(GL_FLOAT,iSize,afData + rkRAttr.GetFogOffset());
    }

    if (rkRAttr.HasPSize())
    {
        glEnableVertexAttribArrayARB(6);
        glVertexAttribPointerARB(6,rkRAttr.GetPSizeChannels(),GL_FLOAT,
            GL_FALSE,iSize,afData + rkRAttr.GetPSizeOffset());
    }

    if (rkRAttr.HasBlendIndices())
    {
        glEnableClientState(GL_MATRIX_INDEX_ARRAY_ARB);
        glMatrixIndexPointerARB(rkRAttr.GetBlendIndicesChannels(),GL_FLOAT,
            iSize,afData + rkRAttr.GetBlendIndicesOffset());
    }

    for (int iUnit = 0; iUnit < rkRAttr.GetMaxTCoords(); iUnit++)
    {
        if (rkRAttr.HasTCoord(iUnit))
        {
            glClientActiveTexture(GL_TEXTURE0 + iUnit);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
            glTexCoordPointer(rkRAttr.GetTCoordChannels(iUnit),GL_FLOAT,iSize,
                afData + rkRAttr.GetTCoordOffset(iUnit));
        }
    }

    if (rkRAttr.HasTangent())
    {
        glEnableVertexAttribArrayARB(14);
        glVertexAttribPointerARB(14,rkRAttr.GetTangentChannels(),GL_FLOAT,
            GL_FALSE,iSize,afData + rkRAttr.GetTangentOffset());
    }

    if (rkRAttr.HasBitangent())
    {
        glEnableVertexAttribArrayARB(15);
        glVertexAttribPointerARB(15,rkRAttr.GetBitangentChannels(),GL_FLOAT,
            GL_FALSE,iSize,afData + rkRAttr.GetBitangentOffset());
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnDisableVBuffer (ResourceIdentifier* pkID)
{
    VBufferID* pkResource = (VBufferID*)pkID;
    const Attributes& rkRAttr = pkResource->IAttr;

    // Unbind the current vertex buffer.
    glBindBuffer(GL_ARRAY_BUFFER,0);

    if (rkRAttr.HasPosition())
    {
        glDisableClientState(GL_VERTEX_ARRAY);
    }

    if (rkRAttr.HasBlendWeight())
    {
        glDisableClientState(GL_WEIGHT_ARRAY_ARB);
    }

    if (rkRAttr.HasNormal())
    {
#ifdef USE_OPENGL2_NORMAL_ATTRIBUTES
        glDisableVertexAttribArrayARB(2);
#else
        glDisableClientState(GL_NORMAL_ARRAY);
#endif
    }

    if (rkRAttr.HasColor(0))
    {
        glDisableClientState(GL_COLOR_ARRAY);
    }

    if (rkRAttr.HasColor(1))
    {
        glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
    }

    if (rkRAttr.HasFog())
    {
        glDisableClientState(GL_FOG_COORDINATE_ARRAY_EXT);
    }

    if (rkRAttr.HasPSize())
    {
        glDisableVertexAttribArrayARB(6);
    }

    if (rkRAttr.HasBlendIndices())
    {
        glDisableClientState(GL_MATRIX_INDEX_ARRAY_ARB);
    }

    for (int iUnit = 0; iUnit < rkRAttr.GetMaxTCoords(); iUnit++)
    {
        if (rkRAttr.HasTCoord(iUnit))
        {
            glClientActiveTexture(GL_TEXTURE0 + iUnit);
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }
    }

    if (rkRAttr.HasTangent())
    {
        glDisableVertexAttribArrayARB(14);
    }

    if (rkRAttr.HasBitangent())
    {
        glDisableVertexAttribArrayARB(15);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnEnableIBuffer (ResourceIdentifier* pkID)
{
    // Bind the current index buffer.
    IBufferID* pkResource = (IBufferID*)pkID;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,pkResource->ID);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnDisableIBuffer (ResourceIdentifier*)
{
    // Unbind the current index buffer.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
}
//----------------------------------------------------------------------------
