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
// Version: 4.5.3 (2008/10/11)

#ifndef WM4CATALOG_H
#define WM4CATALOG_H

#include "Wm4GraphicsLIB.h"
#include "Wm4System.h"

namespace Wm4
{

template <class T>
class Catalog
{
public:
    // The default catalog (for each class type Image, Texture, VertexProgram,
    // PixelProgram) is created in Main::Initialize and destroyed in
    // Main::Terminate (see Wm4Main.cpp).  The default catalog is stored in
    // ms_pkActive.
    //
    // Additional catalogs may be created by the application and are stored in
    // ms_apkCatalogs.  The catalogs are inserted into the array by the
    // constructor call and removed from the array by the destructor call.
    // Therefore, the default catalog is the first entry to be added to the
    // array (when created in Main::Initialize) and the last entry to be
    // removed from the array (when destroyed in Main::Terminate).

    Catalog (const std::string& rkName);
    ~Catalog ();

    const std::string& GetName () const;
    bool Insert (T* ptObject);
    bool Remove (T* ptObject);
    T* Find (const std::string& rkName);
    bool PrintContents (const std::string& rkFilename) const;

    // If the application changes the active catalog and wishes to restore the
    // previous catalog, it should do the following:
    //   Catalog<T>* pkPreviousCatalog = Catalog<T>::GetActive();
    //   Catalog<T>::SetActive(pkMyCatalog);
    //   <operations involving the active catalog)>
    //   Catalog<T>::SetActive(pkPreviousCatalog);
    static void SetActive (Catalog* pkActive);
    static Catalog* GetActive ();

    // Operations applied to the array of catalogs.  Objects of class T are
    // inserted into the active catalog during their construction.  Because
    // the active catalog at the time of destruction might be different, it
    // is necessary for the destructors to call RemoveAll.  The RemoveAll
    // function call iterates over the array of catalogs and attempts to
    // remove the object.  It is possible for an application to insert the
    // object into multiple catalogs, so RemoveAll must search all catalogs.
	static Catalog* GetCatalog(const std::string& rkName);
	static void SetActive(const std::string& rkName);
	static bool RemoveAll(T* ptObject);

private:
    std::string m_kName;

    typedef std::map<std::string,T*> Map;
    typedef typename Map::iterator MapIterator;
    typedef typename Map::const_iterator MapCIterator;
    Map m_kEntry;

    // The default catalog for each class type (Image, Texture,
    // VertexProgram, PixelProgram) is created in Main::Initialize and
    // destroyed in Main::Terminate (see Wm4Main.cpp).
    WM4_GRAPHICS_ITEM static Catalog* ms_pkActive;

    // Additional catalogs may be created by the application.  This array does
    // not contain the default catalog created by class Main.
	WM4_GRAPHICS_ITEM static std::vector<Catalog*> ms_apkCatalogs;
};

#include "Wm4Catalog.inl"

}

#endif
