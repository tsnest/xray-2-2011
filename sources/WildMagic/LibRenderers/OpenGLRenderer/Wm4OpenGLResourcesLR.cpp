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
// Version: 4.0.7 (2008/12/10)

#include "Wm4OpenGLRendererPCH.h"
#include "Wm4OpenGLRenderer.h"
#include "Wm4OpenGLResources.h"
using namespace Wm4;

GLenum OpenGLRenderer::ms_aeTextureMipmap[Texture::MAX_FILTER_TYPES] =
{
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_LINEAR
};

GLenum OpenGLRenderer::ms_aeWrapMode[Texture::MAX_WRAP_TYPES] =
{
    GL_CLAMP,
    GL_REPEAT,
    GL_MIRRORED_REPEAT,
    GL_CLAMP_TO_BORDER,
    GL_CLAMP_TO_EDGE
};

GLenum OpenGLRenderer::ms_aeImageComponents[Image::IT_QUANTITY] =
{
    GL_RGB8,               // Image::IT_RGB888
    GL_RGBA8,              // Image::IT_RGBA8888
    GL_DEPTH_COMPONENT16,  // Image::IT_DEPTH16
    GL_DEPTH_COMPONENT24,  // Image::IT_DEPTH24
    GL_DEPTH_COMPONENT32,  // Image::IT_DEPTH32
    GL_RGB8,               // Image::IT_CUBE_RGB888
    GL_RGBA8,              // Image::IT_CUBE_RGBA8888
    GL_RGB32F_ARB,         // Image::IT_RGB32F
    GL_RGBA32F_ARB,        // Image::IT_RGBA32F
    GL_RGB16F_ARB,         // Image::IT_RGB16F
    GL_RGBA16F_ARB,        // Image::IT_RGBA16F
    GL_RGB16,              // Image::IT_RGB16I
    GL_RGBA16,             // Image::IT_RGBA16I
    GL_INTENSITY8,         // Image::IT_INTENSITY8I
    GL_INTENSITY16,        // Image::IT_INTENSITY16I
    GL_INTENSITY16F_ARB,   // Image::IT_INTENSITY16F
    GL_INTENSITY32F_ARB,   // Image::IT_INTENSITY32F
    GL_RGB5,               // Image::IT_RGB565
    GL_RGB5_A1,            // Image::IT_RGBA5551
    GL_RGBA4               // Image::IT_RGBA4444
};

GLenum OpenGLRenderer::ms_aeImageFormats[Image::IT_QUANTITY] =
{
    GL_RGB,              // Image::IT_RGB888
    GL_RGBA,             // Image::IT_RGBA8888
    GL_DEPTH_COMPONENT,  // Image::IT_DEPTH16
    GL_DEPTH_COMPONENT,  // Image::IT_DEPTH24
    GL_DEPTH_COMPONENT,  // Image::IT_DEPTH32
    GL_RGB,              // Image::IT_CUBE_RGB888
    GL_RGBA,             // Image::IT_CUBE_RGBA8888
    GL_RGB,              // Image::IT_RGB32F
    GL_RGBA,             // Image::IT_RGBA32F
    GL_RGB,              // Image::IT_RGB16F
    GL_RGBA,             // Image::IT_RGBA16F
    GL_RGB,              // Image::IT_RGB16I
    GL_RGBA,             // Image::IT_RGBA16I
    GL_INTENSITY,        // Image::IT_INTENSITY8I,
    GL_INTENSITY,        // Image::IT_INTENSITY16I,
    GL_INTENSITY,        // Image::IT_INTENSITY16F,
    GL_INTENSITY,        // Image::IT_INTENSITY32F
    GL_RGB,              // Image::IT_RGB565
    GL_RGBA,             // Image::IT_RGBA5551
    GL_RGBA              // Image::IT_RGBA4444
};

GLenum OpenGLRenderer::ms_aeImageTypes[Image::IT_QUANTITY] =
{
    GL_UNSIGNED_BYTE,               // Image::IT_RGB888
    GL_UNSIGNED_BYTE,               // Image::IT_RGBA8888
    GL_FLOAT,                       // Image::IT_DEPTH16
    GL_FLOAT,                       // Image::IT_DEPTH24
    GL_DEPTH_COMPONENT,             // Image::IT_DEPTH32
    GL_UNSIGNED_BYTE,               // Image::IT_CUBE_RGB888
    GL_UNSIGNED_BYTE,               // Image::IT_CUBE_RGBA8888
    GL_FLOAT,                       // Image::IT_RGB32F
    GL_FLOAT,                       // Image::IT_RGBA32F
    GL_HALF_FLOAT_ARB,              // Image::IT_RGB16F
    GL_HALF_FLOAT_ARB,              // Image::IT_RGBA16F
    GL_UNSIGNED_SHORT,              // Image::IT_RGB16I
    GL_UNSIGNED_SHORT,              // Image::IT_RGBA16I
    GL_UNSIGNED_BYTE,               // Image::IT_INTENSITY8I
    GL_UNSIGNED_SHORT,              // Image::IT_INTENSITY16I
    GL_HALF_FLOAT_ARB,              // Image::IT_INTENSITY16F
    GL_FLOAT,                       // Image::IT_INTENSITY32F
    GL_UNSIGNED_SHORT_5_6_5_REV,    // Image::IT_RGB565
    GL_UNSIGNED_SHORT_1_5_5_5_REV,  // Image::IT_RGBA5551
    GL_UNSIGNED_SHORT_4_4_4_4_REV   // Image::IT_RGBA4444
};

GLenum OpenGLRenderer::ms_aeSamplerTypes[
    SamplerInformation::MAX_SAMPLER_TYPES] =
{
    GL_TEXTURE_1D,        // SamplerInformation::SAMPLER_1D
    GL_TEXTURE_2D,        // SamplerInformation::SAMPLER_2D
    GL_TEXTURE_3D,        // SamplerInformation::SAMPLER_3D
    GL_TEXTURE_CUBE_MAP,  // SamplerInformation::SAMPLER_CUBE
    GL_TEXTURE_2D,        // SamplerInformation::SAMPLER_PROJ
};

//----------------------------------------------------------------------------
void OpenGLRenderer::OnLoadVProgram (ResourceIdentifier*& rpkID,
    VertexProgram* pkVProgram)
{
    // Generate binding information and compile the program.
    VProgramID* pkResource = WM4_NEW VProgramID;
    rpkID = pkResource;

    const char* acProgramText = pkVProgram->GetProgramText().c_str();
    int iProgramLength = (int)strlen(acProgramText);

    glEnable(GL_VERTEX_PROGRAM_ARB);
    glGenProgramsARB(1,&pkResource->ID);
    glBindProgramARB(GL_VERTEX_PROGRAM_ARB,pkResource->ID);
    glProgramStringARB(GL_VERTEX_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,
        iProgramLength,acProgramText);
    glDisable(GL_VERTEX_PROGRAM_ARB);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnReleaseVProgram (ResourceIdentifier* pkID)
{
    bool bIsActive = IsActive();
    if (!bIsActive)
    {
        SaveContext();
        Activate();
    }

    VProgramID* pkResource = (VProgramID*)pkID;
    glDeleteProgramsARB(1,&pkResource->ID);
    WM4_DELETE pkResource;

    if (!bIsActive)
    {
        RestoreContext();
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnLoadPProgram (ResourceIdentifier*& rpkID,
    PixelProgram* pkPProgram)
{
    // Generate binding information and compile the shader.
    PProgramID* pkResource = WM4_NEW PProgramID;
    rpkID = pkResource;

    const char* acProgramText = pkPProgram->GetProgramText().c_str();
    int iProgramLength = (int)strlen(acProgramText);

    glEnable(GL_FRAGMENT_PROGRAM_ARB);
    glGenProgramsARB(1,&pkResource->ID);
    glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,pkResource->ID);
    glProgramStringARB(GL_FRAGMENT_PROGRAM_ARB,GL_PROGRAM_FORMAT_ASCII_ARB,
        iProgramLength,acProgramText);
    glDisable(GL_FRAGMENT_PROGRAM_ARB);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnReleasePProgram (ResourceIdentifier* pkID)
{
    bool bIsActive = IsActive();
    if (!bIsActive)
    {
        SaveContext();
        Activate();
    }

    PProgramID* pkResource = (PProgramID*)pkID;
    glDeleteProgramsARB(1,&pkResource->ID);
    WM4_DELETE pkResource;

    if (!bIsActive)
    {
        RestoreContext();
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnLoadTexture (ResourceIdentifier*& rpkID,
    Texture* pkTexture)
{
    // Activate the texture unit in hardware that will manage this texture.
    TextureID* pkResource = WM4_NEW TextureID;
    pkResource->TextureObject = pkTexture;
    rpkID = pkResource;

    // Get the texture image and its information.
    const Image* pkImage = pkTexture->GetImage();
    assert(pkImage);
    int iDimension = pkImage->GetDimension();
    unsigned char* aucData = pkImage->GetData();
    int iComponent = ms_aeImageComponents[pkImage->GetFormat()];
    int eFormat = ms_aeImageFormats[pkImage->GetFormat()];
    int eIType = ms_aeImageTypes[pkImage->GetFormat()];

    bool bIsRegularImage = !pkImage->IsCubeImage();
    int eTarget;
    if (bIsRegularImage)
    {
        eTarget = ms_aeSamplerTypes[iDimension-1];
    }
    else
    {
        eTarget = GL_TEXTURE_CUBE_MAP;
    }

    // Generate the name and binding information.
    glGenTextures((GLsizei)1,&pkResource->ID);
    glBindTexture(eTarget,pkResource->ID);

    if(DynamicCast<DynamicTexture>(pkTexture))
    {
        // Bind a pixel buffer object as the data source of the texture.
        glGenBuffers(1,&pkResource->PBOHandle);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,pkResource->PBOHandle);

        GLsizeiptr iNumBytes =
            (GLsizeiptr)(pkImage->GetBytesPerPixel()*pkImage->GetQuantity());

        glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB,iNumBytes,aucData,
            GL_DYNAMIC_DRAW);

        aucData = 0;
    }
    else
    {
        pkResource->PBOHandle = 0;
    }

    // Enable anisotropic filtering (if requested).
    float fAnisotropy = pkTexture->GetAnisotropyValue();
    if (1.0f < fAnisotropy && fAnisotropy <= ms_fMaxAnisotropy)
    {
        glTexParameterf(eTarget,GL_TEXTURE_MAX_ANISOTROPY_EXT,fAnisotropy);
    }
    else
    {
        glTexParameterf(eTarget,GL_TEXTURE_MAX_ANISOTROPY_EXT,1.0f);
    }

    // Set the filter mode.
    Texture::FilterType eFType = pkTexture->GetFilterType();
    if (eFType == Texture::NEAREST
    ||  iComponent == GL_RGB32F_ARB
    ||  iComponent == GL_RGBA32F_ARB)
    {
        glTexParameteri(eTarget,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    }
    else
    {
        glTexParameteri(eTarget,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    }

    // Set the mipmap mode.
    if(iComponent == GL_RGB32F_ARB || iComponent == GL_RGBA32F_ARB)
    {
        glTexParameteri(eTarget,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    }
    else
    {
        glTexParameteri(eTarget,GL_TEXTURE_MIN_FILTER,
            ms_aeTextureMipmap[eFType]);
    }

    // Set the border color (for clamp to border).
    glTexParameterfv(eTarget,GL_TEXTURE_BORDER_COLOR,
        (const float*)pkTexture->GetBorderColor());

    // Copy the image data from system memory to video memory.
    bool bNoMip =
        (eFType == Texture::NEAREST || eFType == Texture::LINEAR);

    switch (iDimension)
    {
    case 1:
        if (bNoMip)
        {
            glTexImage1D(eTarget,0,iComponent,pkImage->GetBound(0),0,
                eFormat,eIType,aucData);
        }
        else
        {
            gluBuild1DMipmaps(eTarget,iComponent,pkImage->GetBound(0),
                eFormat,eIType,aucData);
        }
        glTexParameteri(eTarget,GL_TEXTURE_WRAP_S,
            ms_aeWrapMode[pkTexture->GetWrapType(0)]);
        break;

    case 2:
        if (bIsRegularImage)
        {
            if (bNoMip)
            {
                glTexImage2D(eTarget,0,iComponent,pkImage->GetBound(0),
                    pkImage->GetBound(1),0,eFormat,eIType,aucData);
            }
            else
            {
                gluBuild2DMipmaps(eTarget,iComponent,pkImage->GetBound(0),
                    pkImage->GetBound(1),eFormat,eIType,aucData);
            }
        }
        else
        {
            // A cube map image has 6 subimages (+x,-x,+y,-y,+z,-z).
            int iDelta = pkImage->GetBytesPerPixel()*pkImage->GetQuantity();
            int i;

            if (bNoMip)
            {
                for (i = 0; i < 6; i++, aucData += iDelta)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,0,
                        iComponent,pkImage->GetBound(0),pkImage->GetBound(1),
                        0,eFormat,eIType,aucData);
                }
            }
            else
            {
                for (i = 0; i < 6; i++, aucData += iDelta)
                {
                    gluBuild2DMipmaps(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i,
                        iComponent,pkImage->GetBound(0),pkImage->GetBound(1),
                        eFormat,eIType,aucData);
                }
            }
        }

        glTexParameteri(eTarget,GL_TEXTURE_WRAP_S,
            ms_aeWrapMode[pkTexture->GetWrapType(0)]);
        glTexParameteri(eTarget,GL_TEXTURE_WRAP_T,
            ms_aeWrapMode[pkTexture->GetWrapType(1)]);
        break;

    case 3:
        // TO DO.  Microsoft's GLU library does not implement the function
        // gluBuild3DMipmaps.  DirectX9 SDK does not support automatic
        // generation of mipmaps for volume textures.  For now, do not
        // support mipmaps of 3D textures.  However, manually generated
        // mipmaps can be added later.  The LibGraphics Sampler classes
        // already implement this for software rendering.
        glTexImage3D(eTarget,0,iComponent,pkImage->GetBound(0),
            pkImage->GetBound(1),pkImage->GetBound(2),0,eFormat,eIType,
            aucData);

        glTexParameteri(eTarget,GL_TEXTURE_WRAP_S,
            ms_aeWrapMode[pkTexture->GetWrapType(0)]);
        glTexParameteri(eTarget,GL_TEXTURE_WRAP_T,
            ms_aeWrapMode[pkTexture->GetWrapType(1)]);
        glTexParameteri(eTarget,GL_TEXTURE_WRAP_R,
            ms_aeWrapMode[pkTexture->GetWrapType(2)]);
        break;

    default:
        assert(false);
        break;
    }

    if (pkResource->PBOHandle > 0)
    {
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB,0);
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnReleaseTexture (ResourceIdentifier* pkID)
{
    bool bIsActive = IsActive();
    if (!bIsActive)
    {
        SaveContext();
        Activate();
    }

    TextureID* pkResource = (TextureID*)pkID;
    if (pkResource->PBOHandle > 0)
    {
        glDeleteBuffers(1,&pkResource->PBOHandle);
    }
    glDeleteTextures(1,&pkResource->ID);
    WM4_DELETE pkResource;

    if (!bIsActive)
    {
        RestoreContext();
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnLoadVBuffer (ResourceIdentifier*& rpkID,
    const Attributes& rkIAttr, const Attributes& rkOAttr,
    VertexBuffer* pkVBuffer)
{
    VBufferID* pkResource = WM4_NEW VBufferID;
    rpkID = pkResource;
    pkResource->IAttr = rkIAttr;
    pkResource->OAttr = rkOAttr;

    int iChannels;
    float* afCompatible = 0;  // allocated by BuildCompatibleArray
    pkVBuffer->BuildCompatibleArray(rkIAttr,false,iChannels,afCompatible);

    // Generate the name and binding information.
    glGenBuffers(1,&pkResource->ID);
    glBindBuffer(GL_ARRAY_BUFFER,pkResource->ID);

    // Copy the vertex buffer data from system memory to video memory.
    glBufferData(GL_ARRAY_BUFFER,iChannels*sizeof(float),afCompatible,
        GL_STATIC_DRAW);

    WM4_DELETE[] afCompatible;
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnReleaseVBuffer (ResourceIdentifier* pkID)
{
    bool bIsActive = IsActive();
    if (!bIsActive)
    {
        SaveContext();
        Activate();
    }

    VBufferID* pkResource = (VBufferID*)pkID;
    glDeleteBuffers(1,&pkResource->ID);
    WM4_DELETE pkResource;

    if (!bIsActive)
    {
        RestoreContext();
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnLoadIBuffer (ResourceIdentifier*& rpkID,
    IndexBuffer* pkIBuffer)
{
    IBufferID* pkResource = WM4_NEW IBufferID;
    rpkID = pkResource;

    glGenBuffers(1,&pkResource->ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,pkResource->ID);

    // Copy the index buffer data from system memory to video memory.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        pkIBuffer->GetIndexQuantity()*sizeof(int),pkIBuffer->GetData(),
        GL_STATIC_DRAW);
}
//----------------------------------------------------------------------------
void OpenGLRenderer::OnReleaseIBuffer (ResourceIdentifier* pkID)
{
    bool bIsActive = IsActive();
    if (!bIsActive)
    {
        SaveContext();
        Activate();
    }

    IBufferID* pkResource = (IBufferID*)pkID;
    glDeleteBuffers(1,&pkResource->ID);
    WM4_DELETE pkResource;

    if (!bIsActive)
    {
        RestoreContext();
    }
}
//----------------------------------------------------------------------------
