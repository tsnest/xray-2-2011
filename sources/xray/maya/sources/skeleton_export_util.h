////////////////////////////////////////////////////////////////////////////
//	Created		: 05.02.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SKELETON_EXPORT_UTIL_H_INCLUDED
#define SKELETON_EXPORT_UTIL_H_INCLUDED

class skeleton_export_cmd : public MPxCommand 
{
public:
	static const MString	Name;

public:

	virtual MStatus			doIt		( const MArgList& );
	virtual bool			isUndoable	() const			{return false;} 

	static void*			creator		()					{return CRT_NEW( skeleton_export_cmd );}
	static MSyntax			newSyntax	();


};  // class anim_export_cmd

#endif // #ifndef SKELETON_EXPORT_UTIL_H_INCLUDED