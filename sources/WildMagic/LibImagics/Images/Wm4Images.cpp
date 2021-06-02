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
// Version: 4.0.1 (2006/09/21)

#include "Wm4ImagicsPCH.h"
#include "Wm4Images.h"
using namespace Wm4;

// VC6 incorrectly complains that the template-class specialization for
// TImage<Eint>, TImage2D<Eint>, and TImage3D<Eint> are already instantiated.
#ifdef WM4_USING_VC6
#pragma warning(disable : 4660 )
#endif


//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
namespace Wm4
{
template class WM4_IMAGICS_ITEM TImage<Echar>;
template class WM4_IMAGICS_ITEM TImage<Euchar>;
template class WM4_IMAGICS_ITEM TImage<Eshort>;
template class WM4_IMAGICS_ITEM TImage<Eushort>;
template class WM4_IMAGICS_ITEM TImage<Eint>;
template class WM4_IMAGICS_ITEM TImage<Euint>;
template class WM4_IMAGICS_ITEM TImage<Elong>;
template class WM4_IMAGICS_ITEM TImage<Eulong>;
template class WM4_IMAGICS_ITEM TImage<Efloat>;
template class WM4_IMAGICS_ITEM TImage<Edouble>;
template class WM4_IMAGICS_ITEM TImage<Ergb5>;
template class WM4_IMAGICS_ITEM TImage<Ergb8>;

template class WM4_IMAGICS_ITEM TImage2D<Echar>;
template class WM4_IMAGICS_ITEM TImage2D<Euchar>;
template class WM4_IMAGICS_ITEM TImage2D<Eshort>;
template class WM4_IMAGICS_ITEM TImage2D<Eushort>;
template class WM4_IMAGICS_ITEM TImage2D<Eint>;
template class WM4_IMAGICS_ITEM TImage2D<Euint>;
template class WM4_IMAGICS_ITEM TImage2D<Elong>;
template class WM4_IMAGICS_ITEM TImage2D<Eulong>;
template class WM4_IMAGICS_ITEM TImage2D<Efloat>;
template class WM4_IMAGICS_ITEM TImage2D<Edouble>;
template class WM4_IMAGICS_ITEM TImage2D<Ergb5>;
template class WM4_IMAGICS_ITEM TImage2D<Ergb8>;

template class WM4_IMAGICS_ITEM TImage3D<Echar>;
template class WM4_IMAGICS_ITEM TImage3D<Euchar>;
template class WM4_IMAGICS_ITEM TImage3D<Eshort>;
template class WM4_IMAGICS_ITEM TImage3D<Eushort>;
template class WM4_IMAGICS_ITEM TImage3D<Eint>;
template class WM4_IMAGICS_ITEM TImage3D<Euint>;
template class WM4_IMAGICS_ITEM TImage3D<Elong>;
template class WM4_IMAGICS_ITEM TImage3D<Eulong>;
template class WM4_IMAGICS_ITEM TImage3D<Efloat>;
template class WM4_IMAGICS_ITEM TImage3D<Edouble>;
template class WM4_IMAGICS_ITEM TImage3D<Ergb5>;
template class WM4_IMAGICS_ITEM TImage3D<Ergb8>;
}
//----------------------------------------------------------------------------
