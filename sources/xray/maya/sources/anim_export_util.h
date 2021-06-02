////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef ANIM_EXPORT_UTIL_H_INCLUDED
#define ANIM_EXPORT_UTIL_H_INCLUDED
class MDagPath;
class MObject;


class anim_exporter_options
{
	
public:	
							//anim_exporter_options	( );
							//anim_exporter_options	( const MString& options );

							anim_exporter_options	( bool local, const MString& local_coordinat_center_node_name, bool additive,  const MString& additive_animation_path );

public:	
	MMatrix	 animation_position		( )const { return	m_local_animation_position; }
	bool	 is_additive			( )const { return	m_additive; }
	MString	 additive_animation_path( )const { return	m_additive_animation_path; }
	MStatus	 status					( )const { return	m_status; }

	float		m_precision_t;
	float		m_precision_r;
	float		m_precision_s;

private:
	MStatus	 set_local_animation_position	( const MString& node_name );

private:
	MMatrix		m_local_animation_position;
	bool		m_additive;
	bool		m_local;
	MString		m_additive_animation_path;
	MStatus		m_status;
}; // class anim_exporter_options


class anim_export_cmd : public MPxCommand 
{
public:
	static const MString	Name;

public:

	virtual MStatus			doIt		( const MArgList& );
	virtual bool			isUndoable	() const			{return false;} 

	static void*			creator		()					{return CRT_NEW( anim_export_cmd );}
	static MSyntax			newSyntax	();


};  // class anim_export_cmd

MObject retrive_root_joint( const MObject &oblect );

#endif // #ifndef ANIM_EXPORT_UTIL_H_INCLUDED
