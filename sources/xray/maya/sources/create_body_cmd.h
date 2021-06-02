////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CREATE_BODY_CMD_H_INCLUDED
#define CREATE_BODY_CMD_H_INCLUDED


class createBodyCmd : public MPxCommand 
{
public:
	static const MString			Name;

public:
	virtual MStatus	doIt ( const MArgList& );
	virtual MStatus undoIt();
 	virtual MStatus redoIt();
	virtual bool isUndoable() const { return true; } 

	static void *creator() { return CRT_NEW(createBodyCmd); }
	static MSyntax newSyntax();


private:
	MDagModifier dagMod;
};



//class create_body_cmd  : public MPxCommand 
//{
//public:
//static void *creator() { return CRT_NEW(create_body_cmd); }
//static MSyntax newSyntax();
//
// 	
//virtual MStatus	doIt( const MArgList& args );
//virtual MStatus undoIt();
//virtual MStatus redoIt();
//virtual bool isUndoable() const { return true; }
//
//private:
////	MStatus	do_create_body_node( const MArgList& args )
//	MDagModifier m_dag_moifier;
//	
//	MStatus create_shape_node( MDagPath &path, const MArgList& args );
//
//
//}; // class create_body_cmd

#endif // #ifndef CREATE_BODY_CMD_H_INCLUDED