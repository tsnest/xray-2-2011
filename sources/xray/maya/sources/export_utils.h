////////////////////////////////////////////////////////////////////////////
//	Created		: 28.10.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EXPORT_UTILS_H_INCLUDED
#define EXPORT_UTILS_H_INCLUDED


static pcstr file_name_flag						= "-o";
static pcstr file_name_flag_l					= "-outfilename";

MString file_name_from_args( const MArgDatabase &argData, MStatus *status );
MStatus check_claster_joints( const MSelectionList &all_joints );



#endif // #ifndef EXPORT_UTILS_H_INCLUDED