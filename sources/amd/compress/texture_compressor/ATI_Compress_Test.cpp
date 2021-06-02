// ATI_Compress_Test.cpp : Defines the entry point for the console application.
//
#include "pch.h"

#include <stdio.h>
#include <tchar.h>
#include <ddraw.h>
#include <d3d9types.h>
#include <assert.h>
#include "../ATI_Compress.h"
#include "ATI_Compress_Test_Helpers.h"

int _tmain(int argc, _TCHAR* argv[])
{
   if(argc < 5)
   {
      _tprintf(_T("AMD texture compressor SourceFile DestFile Format Speed\n"));
      return 0;
   }

   TCHAR* pszSourceFile = argv[1];
   TCHAR* pszDestFile = argv[2];
   ATI_TC_FORMAT destFormat = ParseFormat(argv[3]);
   int nCompressionSpeed = _tstoi(argv[4]);

   if(destFormat == ATI_TC_FORMAT_Unknown)
   {
      _tprintf(_T("Unsupported dest format\n"));
      return 0;
   }

   // Load the source texture
   ATI_TC_Texture srcTexture;
   if(!LoadDDSFile(pszSourceFile, srcTexture))
      return 0;

   // Init dest texture
   ATI_TC_Texture destTexture;
   destTexture.dwSize = sizeof(destTexture);
   destTexture.dwWidth = srcTexture.dwWidth;
   destTexture.dwHeight = srcTexture.dwHeight;
   destTexture.dwPitch = 0;
   destTexture.format = destFormat;
   destTexture.dwDataSize = ATI_TC_CalculateBufferSize(&destTexture);
   destTexture.pData = (ATI_TC_BYTE*) malloc(destTexture.dwDataSize);

   ATI_TC_CompressOptions options;
   memset(&options, 0, sizeof(options));
   options.dwSize = sizeof(options);
   options.nCompressionSpeed = (ATI_TC_Speed) nCompressionSpeed;

   ATI_TC_ConvertTexture(&srcTexture, &destTexture, &options, NULL, NULL, NULL);

   SaveDDSFile(pszDestFile, destTexture);

   free(srcTexture.pData);
   free(destTexture.pData);

   return 0;
}