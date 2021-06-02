////////////////////////////////////////////////////////////////////////////
//	Created		: 12.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef JOINT_NODE_H_INCLUDED
#define JOINT_NODE_H_INCLUDED
#include	"transform_drawable.h"
#include	"node_exportable.h"

class body_node;

struct atribute_freedom_degree
{
	enum e_degree{
		tx,ty,tz,rx,ry,rz,e_degree_end
	} degree;

	atribute_freedom_degree(e_degree _degree, MFnUnitAttribute::Type type):unit_type( type ), degree( _degree ) 
	{

	}


	void 	 can_use ( bool &enabled, bool &optional, short joint_type );
///////////////////////////////////////////////////////////////////						
	MObject	transform_attribute;
	MObject	transform_low_limit_enable;
	MObject	transform_high_limit_enable;
	MObject	transform_low_limit;
	MObject	transform_high_limit;
/////////////////////////////////////////////////////////////////////////////////////////////

	MObject	enable_state;
//	MObject	limits_enable_state;
//	MObject	low_limit;
//	MObject	high_limit;
	MFnUnitAttribute::Type unit_type;
};

struct	freedom_degree
{
	freedom_degree():
		b_limits_enabled(false), 
		b_initialized(false), 
		b_switching(false)
	{
	}
	bool	b_limits_enabled;
	bool	b_initialized;
	bool	b_switching;

};



class joint_node: 
	public MPxTransform, /*MPxLocatorNode*/
	public transform_drawable,
	public node_exportable
{
	typedef MPxTransform	super;
public:
	enum {joint_rigid, joint_fixed, joint_ball, joint_generic, joint_wheel, joint_slider};
    virtual void					draw			( M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status, MColor colorRGB);
    
    virtual bool					isBounded		() const;
    virtual MBoundingBox			boundingBox		() const; 

    static void*					creator			();
    static MStatus					initialize		();

    static const MTypeId			typeId;
    static const MString			typeName;	
    // Attributes
    static MObject			jointType;
    static MObject			body0;
    static MObject			body1;
protected:
	virtual MStatus		compute								( const MPlug& plug, MDataBlock& data );
private:
	virtual void		postConstructor						( );

	static void		attr_changed_cb							( MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* clientData);
	static MStatus	freedom_degree_computeattr_changed_cb	( atribute_freedom_degree &attr, MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* clientData );
	MStatus			freedom_degree_compute					( atribute_freedom_degree &attr, const MPlug& plug, MDataBlock& data );
protected:
	virtual	MStatus	do_export_node							( xray::configs::lua_config_value cfg );	
private:
	MStatus	export_rigidly_connected						( xray::configs::lua_config_value table );	
	MStatus	export_joint									( xray::configs::lua_config_value table );	
public:
	static atribute_freedom_degree	x_translate;
	static atribute_freedom_degree	y_translate;
	static atribute_freedom_degree	z_translate;

	static atribute_freedom_degree	x_rotate;
	static atribute_freedom_degree	y_rotate;
	static atribute_freedom_degree	z_rotate;



	freedom_degree	m_freedom_degrees[atribute_freedom_degree::e_degree_end];

protected:
	static MStatus add_freedom_degree_atrribute(	const MObject& joint_type_attr, 
										MObject& enable_state, 
										MObject& limits_enable_state, 
										MObject& low_limit,
										MObject& high_limit,
										
										const MString& longName, 
										const MString& shortName,
										MFnUnitAttribute::Type unit_type,
										double min,
										double max
								);
	static MStatus add_freedom_degree_atrribute(	atribute_freedom_degree &attr,
										const MObject& joint_type_attr,
										const MString& longName, 
										const MString& shortName,
										
										double min,
										double max
								);
public:
	virtual	bool			first_time_computed()				{ return	true; }
protected:
			bool			is_rigid	();
			enum ebody
			{
				b0,
				b1,
			};
			body_node*		connected_body_node		(ebody b);
			body_node*		actual_body				(ebody b);
			
private:
	virtual bool			can_be_rigid		(){ return false; }
		
private:
static 	bool	b_static_initialized;
}; // class joint_node
#endif // #ifndef JOINT_NODE_H_INCLUDED