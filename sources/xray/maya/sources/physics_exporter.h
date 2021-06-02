////////////////////////////////////////////////////////////////////////////
//	Created		: 21.05.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PHYSICS_EXPORTER_H_INCLUDED
#define PHYSICS_EXPORTER_H_INCLUDED

class physics_exporter  : public MPxFileTranslator
{
public:
	

	virtual MStatus 	writer				(const MFileObject &file, const MString &optionsString, FileAccessMode mode);
	virtual bool		haveWriteMethod		() const		{return true;}
	virtual MString 	defaultExtension	() const		{return (MString("physics"));}
	virtual MString 	filter				() const		{return (MString("*.physics"));}
//	virtual MFileKind	identifyFile		(const MFileObject &, const char *buffer, short size) const;

	static void *		creator				();
}; // class physics_exporter

#endif // #ifndef PHYSICS_EXPORTER_H_INCLUDED