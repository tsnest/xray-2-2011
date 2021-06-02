////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SOLID_VISUAL_EXPORTER_H_INCLUDED
#define SOLID_VISUAL_EXPORTER_H_INCLUDED

namespace xray{
namespace maya{

class solid_visual_exporter : public MPxCommand
{
public:
	static const MString			Name;

public:
	virtual MStatus			doIt		( const MArgList& );
	virtual bool			isUndoable	( ) const		{ return false; } 

	static void*			creator		( );
	static MSyntax			newSyntax	( );

private:
	MStatus				check_args		( MArgDatabase& arg_data );

	MString				m_file_name;
}; // class solid_visual_exporter


} //namespace maya
} //namespace xray

#endif // #ifndef SOLID_VISUAL_EXPORTER_H_INCLUDED