////////////////////////////////////////////////////////////////////////////
//	Created		: 08.05.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BUILD_COLLISION_CMD_H_INCLUDED
#define BUILD_COLLISION_CMD_H_INCLUDED

class build_collision_cmd : public MPxCommand 
{
public:
	static const MString	Name;

public:

	virtual MStatus			doIt		(const MArgList&);
	virtual bool			isUndoable	() const			{return false;} 

	static void*			creator		()					{return CRT_NEW(build_collision_cmd);}
	static MSyntax			newSyntax	();
private:
			MStatus			build		(MDagPath const& path, bool b_reciursive, MString const& parent_name);

}; // class build_collision_cmd

#endif // #ifndef BUILD_COLLISION_CMD_H_INCLUDED