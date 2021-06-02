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
// Version: 4.0.4 (2007/08/11)

#include "Wm4GraphicsPCH.h"
#include "Wm4Terrain.h"
#include "Wm4Camera.h"
#include "Wm4MultitextureEffect.h"
using namespace Wm4;

WM4_IMPLEMENT_RTTI(Wm4,Terrain,Node);
WM4_IMPLEMENT_STREAM(Terrain);
WM4_IMPLEMENT_DEFAULT_NAME_ID(Terrain,Node);

//----------------------------------------------------------------------------
Terrain::Terrain (const char* acHeightName, const char* acImageName,
    const Attributes& rkAttr, Camera* pkCamera, float fUVBias,
    ColorRGBA* pkBorderColor)
    :
    m_kAttr(rkAttr),
    m_spkCamera(pkCamera),
    m_fUVBias(fUVBias),
    m_kBorderColor(pkBorderColor ? *pkBorderColor : ColorRGBA::BLACK)
{
    m_iCameraRow = -1;
    m_iCameraCol = -1;

    m_fUVBias = fUVBias;

    // load global terrain information
    LoadHeader(acHeightName);

    // load terrain pages
    Allocate<TerrainPagePtr>(m_iCols,m_iRows,m_aaspkPage);
    int iRow, iCol;
    for (iRow = 0; iRow < m_iRows; iRow++)
    {
        for (iCol = 0; iCol < m_iCols; iCol++)
        {
            const size_t uiSize = 64;
            char acSuffix[uiSize];
            System::Sprintf(acSuffix,uiSize,"%d.%d",iRow,iCol);
            LoadPage(iRow,iCol,acHeightName,acSuffix,acImageName,acSuffix);
        }
    }

    // Attach the terrain pages to the terrain node.
    m_kChild.resize(m_iRows*m_iCols);
    for (iRow = 0; iRow < m_iRows; iRow++)
    {
        for (iCol = 0; iCol < m_iCols; iCol++)
        {
            AttachChild(m_aaspkPage[iRow][iCol]);
        }
    }
}
//----------------------------------------------------------------------------
Terrain::Terrain ()
    :
    m_kBorderColor(ColorRGBA::BLACK)
{
    m_iRows = 0;
    m_iCols = 0;
    m_iSize = 0;
    m_fMinElevation = 0.0f;
    m_fMaxElevation = 0.0f;
    m_fSpacing = 0.0f;;
    m_aaspkPage = 0;
    m_iCameraRow = -1;
    m_iCameraCol = -1;
    m_fUVBias = 0.0f;
}
//----------------------------------------------------------------------------
Terrain::~Terrain ()
{
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            m_aaspkPage[iRow][iCol] = 0;
        }
    }
    Deallocate<TerrainPagePtr>(m_aaspkPage);
}
//----------------------------------------------------------------------------
TerrainPage* Terrain::GetPage (int iRow, int iCol)
{
    assert(0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols);
    if (0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols)
    {
        return m_aaspkPage[iRow][iCol];
    }

    return 0;
}
//----------------------------------------------------------------------------
TerrainPage* Terrain::GetCurrentPage (float fX, float fY) const
{
    float fInvLength = 1.0f/(m_fSpacing*(float)(m_iSize-1));

    int iCol = (int)Mathf::Floor(fX*fInvLength);
    iCol %= m_iCols;
    if (iCol < 0)
    {
        iCol += m_iCols;
    }

    int iRow = (int)Mathf::Floor(fY*fInvLength);
    iRow %= m_iRows;
    if (iRow < 0)
    {
        iRow += m_iRows;
    }

    return m_aaspkPage[iRow][iCol];
}
//----------------------------------------------------------------------------
float Terrain::GetHeight (float fX, float fY) const
{
    TerrainPage* pkPage = GetCurrentPage(fX,fY);

    // subtract off the translation due to wrap-around
    fX -= pkPage->Local.GetTranslate().X();
    fY -= pkPage->Local.GetTranslate().Y();

    return pkPage->GetHeight(fX,fY);
}
//----------------------------------------------------------------------------
Vector3f Terrain::GetNormal (float fX, float fY) const
{
    float fXp = fX + m_fSpacing;
    float fXm = fX - m_fSpacing;
    float fYp = fY + m_fSpacing;
    float fYm = fY - m_fSpacing;

    TerrainPage* pkPage;
    float fXTmp, fYTmp;

    pkPage = GetCurrentPage(fXp,fY);
    fXTmp = fXp - pkPage->Local.GetTranslate().X();
    fYTmp = fY - pkPage->Local.GetTranslate().Y();
    float fHpz = pkPage->GetHeight(fXTmp,fYTmp);

    pkPage = GetCurrentPage(fXm,fY);
    fXTmp = fXm - pkPage->Local.GetTranslate().X();
    fYTmp = fY - pkPage->Local.GetTranslate().Y();
    float fHmz = pkPage->GetHeight(fXTmp,fYTmp);

    pkPage = GetCurrentPage(fX,fYp);
    fXTmp = fX - pkPage->Local.GetTranslate().X();
    fYTmp = fYp - pkPage->Local.GetTranslate().Y();
    float fHzp = pkPage->GetHeight(fXTmp,fYTmp);

    pkPage = GetCurrentPage(fX,fYm);
    fXTmp = fX - pkPage->Local.GetTranslate().X();
    fYTmp = fYm - pkPage->Local.GetTranslate().Y();
    float fHzm = pkPage->GetHeight(fXTmp,fYTmp);

    Vector3f kNormal(fHmz-fHpz,fHzm-fHzp,1.0f);
    kNormal.Normalize();
    return kNormal;
}
//----------------------------------------------------------------------------
void Terrain::LoadHeader (const char* acHeightName)
{
    // Load the data into temporary variables.  EndianCopy uses memcpy, so
    // if you were to load directly into the class members and use memcpy,
    // the source and destination overlap, in which case memcpy results are
    // unpredictable.
    char acFilename[512];
    System::Sprintf(acFilename,512,"%s.wmhf",acHeightName);
    const char* acPath = System::GetPath(acFilename,System::SM_READ);
    FILE* pkHeader = System::Fopen(acPath,"rb");
    assert(pkHeader);
    unsigned short usSize;
    System::Read4le(pkHeader,1,&m_iRows);
    System::Read4le(pkHeader,1,&m_iCols);
    System::Read2le(pkHeader,1,&usSize);
    m_iSize = (int)usSize;
    System::Read4le(pkHeader,1,&m_fMinElevation);
    System::Read4le(pkHeader,1,&m_fMaxElevation);
    System::Read4le(pkHeader,1,&m_fSpacing);
    System::Fclose(pkHeader);
}
//----------------------------------------------------------------------------
void Terrain::LoadPage (int iRow, int iCol, const char* acHeightName,
    const char* acHeightSuffix, const char* acImageName,
    const char* acImageSuffix)
{
    int iQuantity = m_iSize*m_iSize;
    unsigned short* ausHeight = WM4_NEW unsigned short[iQuantity];
    const size_t uiSize = 512;
    char acFileName[uiSize];
    System::Sprintf(acFileName,uiSize,"%s.%s.wmhf",acHeightName,
        acHeightSuffix);
    const char* acPath = System::GetPath(acFileName,System::SM_READ);
    FILE* pkHeightFile = System::Fopen(acPath,"rb");
    assert(pkHeightFile);
    if (pkHeightFile)
    {
        System::Read2le(pkHeightFile,iQuantity,ausHeight);
        System::Fclose(pkHeightFile);
    }
    else
    {
        assert(false);
        memset(ausHeight,0,iQuantity*sizeof(unsigned short));
    }

    float fLength = m_fSpacing*(float)(m_iSize-1);
    Vector2f kOrigin(iCol*fLength,iRow*fLength);
    TerrainPage* pkPage = WM4_NEW TerrainPage(m_kAttr,m_iSize,ausHeight,
        kOrigin,m_fMinElevation,m_fMaxElevation,m_fSpacing,m_fUVBias);

    System::Sprintf(acFileName,uiSize,"%s.%s",acImageName,acImageSuffix);
    MultitextureEffect* pkEffect = WM4_NEW MultitextureEffect(1);
    pkEffect->SetTextureName(0,acFileName);
    pkEffect->Configure();
    pkPage->AttachEffect(pkEffect);

    m_aaspkPage[iRow][iCol] = pkPage;
}
//----------------------------------------------------------------------------
TerrainPagePtr Terrain::ReplacePage (int iRow, int iCol,
    const char* acHeightName, const char* acHeightSuffix,
    const char* acImageName, const char* /* acImageSuffix */)
{
    // TO DO:  At some time a change was made so that acImageSuffix was no
    // longer needed.  Figure out whether it is irrelevant, and then remove
    // the formal parameter.
    assert(0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols);
    if (0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols)
    {
        TerrainPagePtr spkSave = m_aaspkPage[iRow][iCol];
        LoadPage(iRow,iCol,acHeightName,acHeightSuffix,acImageName,
            acHeightSuffix);
        return spkSave;
    }
    return 0;
}
//----------------------------------------------------------------------------
TerrainPagePtr Terrain::ReplacePage (int iRow, int iCol,
    TerrainPage* pkNewPage)
{
    assert(0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols);
    if (0 <= iRow && iRow < m_iRows && 0 <= iCol && iCol < m_iCols)
    {
        TerrainPagePtr spkSave = m_aaspkPage[iRow][iCol];
        m_aaspkPage[iRow][iCol] = pkNewPage;
        return spkSave;
    }
    return 0;
}
//----------------------------------------------------------------------------
void Terrain::OnCameraMotion ()
{
    assert(m_spkCamera);
    if (!m_spkCamera)
    {
        return;
    }

    // get camera location/direction in model space of terrain
    Vector3f kWorldEye = m_spkCamera->GetLocation();
    Vector3f kWorldDir = m_spkCamera->GetDVector();
    Vector3f kModelEye = World.ApplyInverse(kWorldEye);
    Vector3f kModelDir = kWorldDir*World.GetRotate();

    // Update the model-space origins of the terrain pages.  Start the
    // process by locating the page that contains the camera.
    float fLength = m_fSpacing*(float)(m_iSize-1);
    float fInvLength = 1.0f/fLength;
    int iNewCameraCol = (int)Mathf::Floor(kModelEye.X()*fInvLength);
    int iNewCameraRow = (int)Mathf::Floor(kModelEye.Y()*fInvLength);
    if (iNewCameraCol != m_iCameraCol || iNewCameraRow != m_iCameraRow)
    {
        m_iCameraCol = iNewCameraCol;
        m_iCameraRow = iNewCameraRow;

        // translate page origins for toroidal wraparound
        int iCMinO = m_iCameraCol - m_iCols/2;
        int iCMinP = iCMinO % m_iCols;
        if (iCMinP < 0)
        {
            iCMinP += m_iCols;
        }

        int iRMinO = m_iCameraRow - m_iRows/2;
        int iRMinP = iRMinO % m_iRows;
        if (iRMinP < 0)
        {
            iRMinP += m_iRows;
        }

        int iRO = iRMinO, iRP = iRMinP;
        for (int iRow = 0; iRow < m_iRows; iRow++)
        {
            int iCO = iCMinO, iCP = iCMinP;
            for (int iCol = 0; iCol < m_iCols; iCol++)
            {
                TerrainPage* pkPage = m_aaspkPage[iRP][iCP];
                Vector2f kOldOrigin = pkPage->GetOrigin();
                Vector2f kNewOrigin(iCO*fLength,iRO*fLength);
                Vector2f kTrn = kNewOrigin - kOldOrigin;
                Vector3f kPTrn(kTrn.X(),kTrn.Y(),
                    pkPage->Local.GetTranslate().Z());
                pkPage->Local.SetTranslate(kPTrn);

                iCO++;
                if (++iCP == m_iCols)
                {
                    iCP = 0;
                }
            }

            iRO++;
            if (++iRP == m_iRows)
            {
                iRP = 0;
            }
        }
        UpdateGS();
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void Terrain::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM4_BEGIN_DEBUG_STREAM_LOAD;

    Node::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iRows);
    rkStream.Read(m_iCols);
    rkStream.Read(m_iSize);
    rkStream.Read(m_fMinElevation);
    rkStream.Read(m_fMaxElevation);
    rkStream.Read(m_fSpacing);
    rkStream.Read(m_iCameraRow);
    rkStream.Read(m_iCameraCol);
    rkStream.Read(m_fUVBias);
    rkStream.Read(m_kBorderColor);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkCamera
    pkLink->Add(pkObject);
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            rkStream.Read(pkObject);  // m_aaspkPage[iRow][iCol]
            pkLink->Add(pkObject);
        }
    }

    WM4_END_DEBUG_STREAM_LOAD(Terrain);
}
//----------------------------------------------------------------------------
void Terrain::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkCamera = (Camera*)rkStream.GetFromMap(pkLinkID);

    Allocate<TerrainPagePtr>(m_iCols,m_iRows,m_aaspkPage);
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            pkLinkID = pkLink->GetLinkID();
            m_aaspkPage[iRow][iCol] =
                (TerrainPage*)rkStream.GetFromMap(pkLinkID);
        }
    }
}
//----------------------------------------------------------------------------
bool Terrain::Register (Stream& rkStream) const
{
    if (!Node::Register(rkStream))
    {
        return false;
    }

    if (m_spkCamera)
    {
        m_spkCamera->Register(rkStream);
    }

    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            m_aaspkPage[iRow][iCol]->Register(rkStream);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void Terrain::Save (Stream& rkStream) const
{
    WM4_BEGIN_DEBUG_STREAM_SAVE;

    Node::Save(rkStream);

    // native data
    rkStream.Write(m_iRows);
    rkStream.Write(m_iCols);
    rkStream.Write(m_iSize);
    rkStream.Write(m_fMinElevation);
    rkStream.Write(m_fMaxElevation);
    rkStream.Write(m_fSpacing);
    rkStream.Write(m_iCameraRow);
    rkStream.Write(m_iCameraCol);
    rkStream.Write(m_fUVBias);
    rkStream.Write(m_kBorderColor);

    // link data
    rkStream.Write(m_spkCamera);
    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            rkStream.Write(m_aaspkPage[iRow][iCol]);
        }
    }

    WM4_END_DEBUG_STREAM_SAVE(Terrain);
}
//----------------------------------------------------------------------------
int Terrain::GetDiskUsed (const StreamVersion& rkVersion) const
{
    return Node::GetDiskUsed(rkVersion) +
        sizeof(m_iRows) +
        sizeof(m_iCols) +
        sizeof(m_iSize) +
        sizeof(m_fMinElevation) +
        sizeof(m_fMaxElevation) +
        sizeof(m_fSpacing) +
        sizeof(m_iCameraRow) +
        sizeof(m_iCameraCol) +
        sizeof(m_fUVBias) +
        sizeof(m_kBorderColor) +
        WM4_PTRSIZE(m_spkCamera) +
        m_iRows*m_iCols*WM4_PTRSIZE(m_aaspkPage[0][0]);
}
//----------------------------------------------------------------------------
StringTree* Terrain::SaveStrings (const char*)
{
    StringTree* pkTree = WM4_NEW StringTree;

    // strings
    pkTree->Append(Format(&TYPE,GetName().c_str()));
    pkTree->Append(Format("rows =",m_iRows));
    pkTree->Append(Format("cols =",m_iCols));
    pkTree->Append(Format("size =",m_iSize));
    pkTree->Append(Format("min elevation =",m_fMinElevation));
    pkTree->Append(Format("max elevation =",m_fMaxElevation));
    pkTree->Append(Format("spacing =",m_fSpacing));
    pkTree->Append(Format("camera row =",m_iCameraRow));
    pkTree->Append(Format("camera col =",m_iCameraCol));
    pkTree->Append(Format("uv bias =",m_fUVBias));
    pkTree->Append(Format("border color =",m_kBorderColor));

    // children
    pkTree->Append(Node::SaveStrings());

    for (int iRow = 0; iRow < m_iRows; iRow++)
    {
        for (int iCol = 0; iCol < m_iCols; iCol++)
        {
            const size_t uiSize = 64;
            char acPageName[uiSize];
            System::Sprintf(acPageName,uiSize,"page[%d][%d] =",iRow,iCol);
            pkTree->Append(m_aaspkPage[iRow][iCol]->SaveStrings(acPageName));
        }
    }

    return pkTree;
}
//----------------------------------------------------------------------------
