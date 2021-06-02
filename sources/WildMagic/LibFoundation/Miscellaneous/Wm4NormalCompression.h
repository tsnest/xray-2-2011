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

#ifndef WM4NORMALCOMPRESSION_H
#define WM4NORMALCOMPRESSION_H

#include "Wm4FoundationLIB.h"
#include "Wm4System.h"

namespace Wm4
{

// Compress unit-length normal vectors (x,y,z) to 16-bit quantities.  3 bits
// are used to identify the octant containing the vector, the other 13 bits
// are used for the mantissa.

WM4_FOUNDATION_ITEM void CompressNormal (double dX, double dY, double dZ,
    unsigned short& rusIndex);

WM4_FOUNDATION_ITEM void UncompressNormal (unsigned short usIndex,
    double& rdX, double& rdY, double& rdZ);

// An example of how to use the compression.  This shows the maximum error is
// about 10-degrees between the original and the compressed-then-uncompressed
// vector.
//
//void TestCompression ()
//{
//    const int iS = 1024;
//    double dDotMin = 1.0;
//    int iXMin = iS, iYMin = iS;
//
//    for (int iY = 0; iY < iS; iY++)
//    {
//        double dY0 = iY/(double)iS;
//        for (int iX = 0; iX < iS; iX++)
//        {
//            double dX0 = iX/(double)iS;
//            double dZ0 = 1.0 - dX0*dX0 - dY0*dY0;
//            if ( dZ0 >= 0.0 )
//            {
//                dZ0 = sqrt(dZ0);
//
//                unsigned short usIndex;
//                CompressNormal(dX0,dY0,dZ0,usIndex);
//                assert( usIndex < 8192 );
//
//                double dX1, dY1, dZ1;
//                UncompressNormal(usIndex,dX1,dY1,dZ1);
//
//                double dDot = dX0*dX1+dY0*dY1+dZ0*dZ1;
//                if ( dDot < dDotMin )
//                {
//                    dDotMin = dDot;
//                    iXMin = iX;
//                    iYMin = iY;
//                }
//            }
//        }
//    }
//
//    // S = 16384, dotmin = 0.98474228151906 (about 10-degrees error)
//}
}

#endif
