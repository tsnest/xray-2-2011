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

#ifndef WM4NAMEIDMCR_H
#define WM4NAMEIDMCR_H

//----------------------------------------------------------------------------
#define WM4_DECLARE_NAME_ID \
public: \
    virtual Object* GetObjectByName (const std::string& rkName); \
    virtual void GetAllObjectsByName (const std::string& rkName, \
        std::vector<Object*>& rkObjects); \
    virtual Object* GetObjectByID (unsigned int uiID)
//----------------------------------------------------------------------------
#define WM4_IMPLEMENT_DEFAULT_NAME_ID(classname,baseclassname) \
Object* classname::GetObjectByName (const std::string& rkName) \
{ \
    return baseclassname::GetObjectByName(rkName); \
} \
\
void classname::GetAllObjectsByName (const std::string& rkName, \
    std::vector<Object*>& rkObjects) \
{ \
    baseclassname::GetAllObjectsByName(rkName,rkObjects); \
} \
\
Object* classname::GetObjectByID (unsigned int uiID) \
{ \
    return baseclassname::GetObjectByID(uiID); \
}
//----------------------------------------------------------------------------

#endif
