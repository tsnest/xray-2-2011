////////////////////////////////////////////////////////////////////////////
//	Created		: 12.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "joint_node.h"
#include "RenderRoutines.h"
#include "body_node.h"
#include "maya_engine.h"

using xray::configs::lua_config_value;
using xray::math::float3;

const MTypeId joint_node::typeId	( joint_node_type_id );
const MString joint_node::typeName	( joint_node_type_name );
MObject joint_node::jointType;
MObject joint_node::body0;
MObject joint_node::body1;

atribute_freedom_degree	 joint_node::x_translate( atribute_freedom_degree::tx, MFnUnitAttribute::kDistance ) ;
atribute_freedom_degree	 joint_node::y_translate( atribute_freedom_degree::ty, MFnUnitAttribute::kDistance );
atribute_freedom_degree	 joint_node::z_translate( atribute_freedom_degree::tz, MFnUnitAttribute::kDistance );

atribute_freedom_degree	 joint_node::x_rotate( atribute_freedom_degree::rx, MFnUnitAttribute::kAngle );
atribute_freedom_degree	 joint_node::y_rotate( atribute_freedom_degree::ry, MFnUnitAttribute::kAngle  );
atribute_freedom_degree	 joint_node::z_rotate( atribute_freedom_degree::rz, MFnUnitAttribute::kAngle  );


bool joint_node::b_static_initialized = false;


MStatus joint_node::add_freedom_degree_atrribute(	const MObject& joint_type_attr, 
										MObject& enable_state, 
										MObject& limits_enable_state, 
										MObject& low_limit,
										MObject& high_limit,
										
										const MString& longName, 
										const MString& shortName,
										MFnUnitAttribute::Type unit_type,
										double min,
										double max
								)
{
	XRAY_UNREFERENCED_PARAMETER( max );
	XRAY_UNREFERENCED_PARAMETER( min );
	XRAY_UNREFERENCED_PARAMETER( unit_type );
	XRAY_UNREFERENCED_PARAMETER( low_limit );
	XRAY_UNREFERENCED_PARAMETER( high_limit );
	
	///if( !enable_state.isNull() )
		//return;
	MStatus stat;
	//MFnCompoundAttribute compAttr;
	MFnNumericAttribute numericAttr;
	if( enable_state.isNull() )
		enable_state = numericAttr.create( longName + "_enabled", "b" + shortName , MFnNumericData::kBoolean, false, &stat );
	CHK_STAT_R( stat );
	CHK_STAT_R( addAttribute( enable_state ) );

	//if( limits_enable_state.isNull() )
	//	limits_enable_state =  numericAttr.create( longName + "_limits_enabled", "bl" + shortName, MFnNumericData::kBoolean, false, &stat );
	//CHK_STAT_R(stat);
	////numericAttr.setWritable( false );
	////numericAttr.setHidden( true );
	//CHK_STAT_R( addAttribute( limits_enable_state ) );
	


	//MFnUnitAttribute	unitAttr;
	//if( low_limit.isNull() )
	//	low_limit = unitAttr.create( longName + "_low_limit", "llo" + shortName, unit_type, min, &stat  );
	//CHK_STAT_R( stat );
	//CHK_STAT_R( addAttribute( low_limit ) );
	//if( high_limit.isNull() )
	//	high_limit = unitAttr.create( longName + "_high_limit", "lhi" + shortName,  unit_type, max, &stat );
	//CHK_STAT_R(stat);
	//CHK_STAT_R( addAttribute( high_limit ) );

	//CHK_STAT_R( attributeAffects( limits_enable_state, low_limit ) );
	//CHK_STAT_R( attributeAffects( limits_enable_state, high_limit ) );

	CHK_STAT_R( attributeAffects( enable_state, limits_enable_state ) );
	
	//CHK_STAT_R( attributeAffects( enable_state, low_limit ) );
	//CHK_STAT_R( attributeAffects( enable_state, high_limit ) );




	CHK_STAT_R( attributeAffects( joint_type_attr, enable_state ) );
	CHK_STAT_R( attributeAffects( joint_type_attr, limits_enable_state ) );
	
	//CHK_STAT_R( attributeAffects( joint_type_attr, low_limit ) );
	//CHK_STAT_R( attributeAffects( joint_type_attr, high_limit ) );

	//CHK_STAT_R(unitAttr.setKeyable(true));
	//CHK_STAT_R(unitAttr.setSoftMin(min));
	//CHK_STAT_R(unitAttr.setSoftMax(max));
	//attrObj = numericAttr.create(longName, shortName, MFnNumericData::kBoolean, false, &stat);
	//aCompound = compAttr.create(longName, compoundShortName);
	//IfMErrorReturnIt(status);
	//IfMErrorReturnIt(numAttr.setConnectable(false));
	//IfMErrorReturnIt(numAttr.setStorable(false));
	return stat;
}
MStatus joint_node::add_freedom_degree_atrribute(	atribute_freedom_degree &attr,
										const MObject& joint_type_attr,
										const MString& longName, 
										const MString& shortName,
										
										double min,
										double max
								)
{
	//return add_freedom_degree_atrribute(joint_type_attr, attr.enable_state, attr.limits_enable_state, attr.low_limit, attr.high_limit, longName, shortName, attr.unit_type, min, max );
	return add_freedom_degree_atrribute(joint_type_attr, attr.enable_state, attr.transform_low_limit_enable, attr.transform_low_limit, attr.transform_high_limit, longName, shortName, attr.unit_type, min, max );
}



void joint_node::draw( M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status, MColor colorRGB )
{
	XRAY_UNREFERENCED_PARAMETER( style );
	XRAY_UNREFERENCED_PARAMETER( path );

	//MPxTransform::draw			( view, path, style, status );
	view.beginGL					( ); 

	glPushAttrib					( GL_CURRENT_BIT );
	//implement links and joint-body drawing

	MColor outlineCol				= colorRGB(status);
	if(status == M3dView::kLead )
		glColor4f(0.0f,1.0f,0.0f,0.5f);
	else
		glColor4f(outlineCol.r,outlineCol.g,outlineCol.b,0.5f);

	float sz						= 0.05f* 1;//(float)MDistance::uiToInternal(1);
	RenderRoutines::DrawSphere		(sz);

	MStatus stat;
	// line from center to linked bodies
	MFnDependencyNode node		(thisMObject(), &stat);

	MDagPath					p_this;
	stat						= MDagPath::getAPathTo(thisMObject(), p_this);
	CHK_STAT_RV					(stat);
	MMatrix m_this_i			= p_this.inclusiveMatrixInverse(&stat);
	for(int i=0; i<2; ++i)
	{
		MPlug body_plug			= node.findPlug( (i==0)?"body0" : "body1", true, &stat);
		CHK_STAT_RV				(stat);
		{
			MPlugArray				arr;
			body_plug.connectedTo	(arr, true, true, &stat);
			CHK_STAT_RV				(stat);
			if(arr.length())
			{
				MDagPath			p_body;
				stat				= MDagPath::getAPathTo(arr[0].node(), p_body);

				MMatrix mm			= p_body.inclusiveMatrix(&stat);
				CHK_STAT_RV			(stat);
				MMatrix rel			= mm * m_this_i;

			glBegin					(GL_LINES);
				glVertex3f			(0, 0, 0);
				glVertex3f			((float)rel[3][0], (float)rel[3][1], (float)rel[3][2]);
			glEnd					( );
			}
		}
	}
	glPopAttrib			();
	view.endGL			();
}

bool joint_node::isBounded() const
{
	return true;
}

MBoundingBox joint_node::boundingBox( ) const
{
	return MBoundingBox( MPoint( -0.5,  -0.5,  -0.5 ),  MPoint( 0.5,  0.5,  0.5 ) );
}

void* joint_node::creator( )
{
	return  CRT_NEW(joint_node); 
}


MStatus joint_node::initialize( )
{
	MStatus				stat;

	
	

	MFnEnumAttribute	enumFn;
	if( jointType.isNull() )
	{
		jointType			= enumFn.create("jointType", "jt", 0, &stat);	CHK_STAT_R(stat);
		stat				= enumFn.addField("rigid", joint_rigid);		CHK_STAT_R(stat);
		stat				= enumFn.addField("fixed", joint_fixed);		CHK_STAT_R(stat);
		stat				= enumFn.addField("ball", joint_ball);			CHK_STAT_R(stat);
		stat				= enumFn.addField("generic", joint_generic);	CHK_STAT_R(stat);
		stat				= enumFn.addField("wheel", joint_wheel);		CHK_STAT_R(stat);
		stat				= enumFn.addField("slider", joint_slider);		CHK_STAT_R(stat);
		stat				= enumFn.setKeyable(true);						CHK_STAT_R(stat);
	}
	stat				= addAttribute(jointType);						CHK_STAT_R(stat);
	
	MFnMessageAttribute	messageFn;
	if( body0.isNull() )
		body0				= messageFn.create("body0", "b0", &stat);	CHK_STAT_R(stat);
	stat				= addAttribute(body0);						CHK_STAT_R(stat);

	if( body1.isNull() )
		body1				= messageFn.create("body1", "b1", &stat);	CHK_STAT_R(stat);

	stat				= addAttribute(body1);						CHK_STAT_R(stat);
	
	
	x_translate.transform_attribute			 = MPxTransform::translateX;
	x_translate.transform_low_limit_enable	 = MPxTransform::minTransXLimitEnable;
	x_translate.transform_high_limit_enable  = MPxTransform::maxTransXLimitEnable;
	x_translate.transform_low_limit			 = MPxTransform::minTransXLimit;
	x_translate.transform_high_limit		 = MPxTransform::maxTransXLimit;
	CHK_STAT_R( add_freedom_degree_atrribute( x_translate, jointType, "x_translate", "xt", -1000, 1000 ) );


	y_translate.transform_attribute = MPxTransform::translateY;
	y_translate.transform_low_limit_enable   = MPxTransform::minTransYLimitEnable;
	y_translate.transform_high_limit_enable  = MPxTransform::maxTransYLimitEnable;
	y_translate.transform_low_limit			 = MPxTransform::minTransYLimit;
	y_translate.transform_high_limit		 = MPxTransform::maxTransYLimit;
	CHK_STAT_R( add_freedom_degree_atrribute( y_translate, jointType, "y_translate", "yt", -1000, 1000 ) );


	z_translate.transform_attribute = MPxTransform::translateZ;
	z_translate.transform_low_limit_enable   = MPxTransform::minTransZLimitEnable;
	z_translate.transform_high_limit_enable  = MPxTransform::maxTransZLimitEnable;
	z_translate.transform_low_limit			 = MPxTransform::minTransZLimit;
	z_translate.transform_high_limit		 = MPxTransform::maxTransZLimit;
	CHK_STAT_R( add_freedom_degree_atrribute( z_translate, jointType, "z_translate", "zt", -1000, 1000 ) );


	x_rotate.transform_attribute = MPxTransform::rotateX;
	x_rotate.transform_low_limit_enable   = MPxTransform::minRotXLimitEnable;
	x_rotate.transform_high_limit_enable  = MPxTransform::maxRotXLimitEnable;
	x_rotate.transform_low_limit		  = MPxTransform::minRotXLimit;
	x_rotate.transform_high_limit	  = MPxTransform::maxRotXLimit;
	CHK_STAT_R( add_freedom_degree_atrribute( x_rotate, jointType, "x_rotate", "xr", -M_PI, M_PI ) );


	y_rotate.transform_attribute = MPxTransform::rotateY;
	y_rotate.transform_low_limit_enable   = MPxTransform::minRotYLimitEnable;
	y_rotate.transform_high_limit_enable  = MPxTransform::maxRotYLimitEnable;
	y_rotate.transform_low_limit		  = MPxTransform::minRotYLimit;
	y_rotate.transform_high_limit	  = MPxTransform::maxRotYLimit;
	CHK_STAT_R( add_freedom_degree_atrribute( y_rotate, jointType, "y_rotate", "yr", -M_PI, M_PI ) );


	z_rotate.transform_attribute = MPxTransform::rotateZ;
	z_rotate.transform_low_limit_enable   = MPxTransform::minRotZLimitEnable;
	z_rotate.transform_high_limit_enable  = MPxTransform::maxRotZLimitEnable;
	z_rotate.transform_low_limit		  = MPxTransform::minRotZLimit;
	z_rotate.transform_high_limit		  = MPxTransform::maxRotZLimit;
	CHK_STAT_R( add_freedom_degree_atrribute( z_rotate, jointType, "z_rotate", "zr", -M_PI, M_PI ) );

	b_static_initialized = true;
	return				stat;
}

MStatus joint_node::freedom_degree_compute( atribute_freedom_degree &attr, const MPlug& plug, MDataBlock& data )
{
	MStatus				stat;
	if( plug == attr.enable_state   )
	{	
		short type = data.inputValue( jointType ).asShort();
		bool b_optional = false;
		bool b_enabled = false;
		attr.can_use(b_enabled, b_optional, type );
		data.outputValue( attr.enable_state ).setBool( b_enabled );
		//MPlug( thisMObject(), attr.enable_state ) .setLocked( !b_optional );
		stat = data.setClean( plug );CHK_STAT_R(stat);
		return stat;
	}

	//if( plug == attr.limits_enable_state  )
	if( plug == attr.transform_low_limit_enable )
	{

		short type = data.inputValue( jointType ).asShort();
		bool b_limits_optional = false;
		bool b_limits_enabled = false;
		attr.can_use(b_limits_enabled, b_limits_optional, type );
		bool b_enable_state =  data.inputValue( attr.enable_state ).asBool();
		if( !b_enable_state || !b_limits_enabled || !b_limits_optional )
			data.outputValue( attr.transform_low_limit_enable ).setBool( false );
			//data.outputValue( attr.limits_enable_state ).setBool( false );
		stat = data.setClean( plug );CHK_STAT_R(stat);
		return stat;
		//MPlug( thisMObject() ,attr.limits_enable_state ) .setLocked( !b_enable_state || !b_limits_optional );
		
	}
/*
	if( plug == attr.low_limit )
	{
		bool b_enable_state =  data.inputValue( attr.limits_enable_state ).asBool();
		//MPlug( thisMObject(),attr.low_limit ) .setLocked( !b_enable_state );
	}
	if( plug == attr.high_limit )
	{
		bool b_enable_state =  data.inputValue( attr.limits_enable_state ).asBool();
		//MPlug( thisMObject(),attr.high_limit ) .setLocked( !b_enable_state );
	}
*/
	return MStatus::kUnknownParameter;
}


MStatus		joint_node::compute( const MPlug& plug, MDataBlock& data )
{

	MStatus				stat;

	stat = freedom_degree_compute( x_translate,  plug, data );
	if(stat!=MStatus::kUnknownParameter)
		return stat;
	stat = freedom_degree_compute( y_translate,  plug, data );
	if(stat!=MStatus::kUnknownParameter)
		return stat;
	stat = freedom_degree_compute( z_translate,  plug, data );
	if(stat!=MStatus::kUnknownParameter)
		return stat;
	stat = freedom_degree_compute( x_rotate,  plug, data );
	if(stat!=MStatus::kUnknownParameter)
		return stat;
	stat = freedom_degree_compute( y_rotate, plug, data );
	if(stat!=MStatus::kUnknownParameter)
		return stat;
	stat = freedom_degree_compute( z_rotate, plug, data );
	if(stat!=MStatus::kUnknownParameter)
		return stat;
	

	return				MStatus::kUnknownParameter;
}


void 	 atribute_freedom_degree::can_use( bool &enabled, bool &optional, short joint_type )
{
	bool		rotational		= ( degree == rx ) || ( degree == ry ) || ( degree == rz );

//	bool		translational	= ( degree == tx ) || ( degree == ty ) || ( degree == tz );	

	enabled = false;
	optional = false;

	if( joint_type == joint_node::joint_fixed || joint_type == joint_node::joint_rigid )
		return;
	if(joint_type == joint_node::joint_generic)
	{
		enabled = false;
		optional = rotational;
	}
	if( joint_type == joint_node::joint_ball )
		enabled = rotational;

	if( joint_type == joint_node::joint_wheel )
		 optional = ( degree == rx);
	if( joint_type == joint_node::joint_slider )
		optional =  ( degree == rx || degree == tx );

	
}
 static bool	b_setting_limit = false;

 MStatus joint_node::freedom_degree_computeattr_changed_cb( atribute_freedom_degree &attr, MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* clientData )
 {
		XRAY_UNREFERENCED_PARAMETER( clientData );
		XRAY_UNREFERENCED_PARAMETER( otherPlug );
		MStatus				stat;

		if( b_setting_limit )
			return stat;	

		bool	b_transform_lo_limit_enable	 = ( plug.attribute() == attr.transform_low_limit_enable );
		bool	b_transform_hi_limit_enable	 = ( plug.attribute() == attr.transform_high_limit_enable );
		bool	b_transform_limit_enable	 = b_transform_lo_limit_enable || b_transform_hi_limit_enable;

		bool	b_set_attribute			 = !!( msg & MNodeMessage::kAttributeSet );
//		bool	b_eval_attribute		= !!( msg & MNodeMessage::kAttributeEval );
	 if(	(	attr.transform_attribute == plug.attribute() ||
				b_transform_limit_enable 
			)												||
				b_set_attribute	&&  
			(	plug.attribute() == joint_node::jointType	||
				plug.attribute() == attr.enable_state		//||
				//plug.attribute() == attr.limits_enable_state 
			)  
		){
			MObject node				= plug.node(&stat);		

			short type = MPlug( node, joint_node::jointType ).asShort();
			bool b_jt_optional = false;
			bool b_jt_enabled = false;
			attr.can_use( b_jt_enabled, b_jt_optional, type );
			
			bool b_enabled = MPlug( node, attr.enable_state ).asBool();

			//bool b_limits_enabled = MPlug( node, attr.limits_enable_state ).asBool();
			//bool b_limits_enabled = MPlug( node, attr.transform_low_limit_enable ).asBool();

			MPlug( node, attr.enable_state ).setLocked( !b_jt_optional );

			
			//MPlug( node ,attr.transform_high_limit_enable )	
			//MPlug( node ,attr.transform_low_limit_enable )	
			

			//MPlug( node ,attr.limits_enable_state ).setLocked( !b_enabled || !b_jt_optional );
			bool b_locked =  !b_enabled || !b_jt_optional ;
			//MPlug( node ,attr.limits_enable_state ).setLocked(b_locked);

			if(!b_locked)
			{
				MPlug( node ,attr.transform_high_limit_enable )	.setLocked( false );
				MPlug( node ,attr.transform_low_limit_enable )	.setLocked( false );
			}


			
			//MPlug( node,attr.low_limit ) .setLocked( !b_limits_enabled );
			//MPlug( node,attr.high_limit ) .setLocked( !b_limits_enabled );

			MFnDependencyNode fnDepNode	(node, &stat);
			joint_node* j = dynamic_cast<joint_node*> ( fnDepNode.userNode() );
			if( j && j->first_time_computed() )
				MPlug( node,attr.transform_attribute ).setLocked( !b_enabled );
			bool v_low	= MPlug( node,attr.transform_low_limit_enable ).asBool();
			bool v_high = MPlug( node,attr.transform_high_limit_enable ).asBool();
			//if( !b_enabled && v_low )
			//{
			//		MPlug( node,attr.transform_low_limit_enable ).setBool( false );
			//		MPlug( node ,attr.transform_low_limit_enable ).setLocked( b_locked );
			//}

			if( j )//&&  b_transform_limit_enable && b_set_attribute
			{
				
				freedom_degree &fd = j->m_freedom_degrees[ attr.degree ];
				
				//bool v_old = fd.b_limits_enabled ;
				//bool v = v_old;


				if(!fd.b_initialized)
				{
					fd.b_limits_enabled = v_low;
					fd.b_switching = ( fd.b_limits_enabled != v_high );
					fd.b_initialized = true;
					return stat;
				}

				if( !b_enabled && fd.b_limits_enabled )
				{
					fd.b_limits_enabled = false ;
					//fd.b_switching = true;
				}


				
				if( !fd.b_switching )
				{
					fd.b_switching = ( fd.b_limits_enabled != v_low ) || ( fd.b_limits_enabled != v_high );
					if( fd.b_switching )
						fd.b_limits_enabled = !fd.b_limits_enabled;
				} else
				{
					fd.b_switching = ( fd.b_limits_enabled != v_low ) || ( fd.b_limits_enabled != v_high );;
					if(!fd.b_switching)
						fd.b_initialized = false;
				}
			//}

			//if(j)
			//{
			//	freedom_degree &fd = j->m_freedom_degrees[ attr.degree ];
				if( fd.b_switching && fd.b_limits_enabled != v_high  )// fd.b_switching &&
				{
						MPlug( plug.node(&stat), attr.transform_high_limit_enable ).setBool( j->m_freedom_degrees[ attr.degree ].b_limits_enabled );
						MPlug( node ,attr.transform_high_limit_enable ).setLocked( b_locked );
				}
				if( fd.b_switching && fd.b_limits_enabled != v_low  )
				{
						MPlug( plug.node(&stat), attr.transform_low_limit_enable ).setBool( j->m_freedom_degrees[ attr.degree ].b_limits_enabled );
						MPlug( node ,attr.transform_low_limit_enable ).setLocked( b_locked );
				}
				if(!b_enabled && !fd.b_limits_enabled && !fd.b_switching )
				{
					MPlug( node ,attr.transform_high_limit_enable ).setLocked( b_locked );
					MPlug( node ,attr.transform_low_limit_enable ).setLocked( b_locked );
				}
			}
			
		
	}
	



	 //if( ( msg & MNodeMessage::kAttributeEval ) &&  
		//				(	plug.attribute() == attr.enable_state || 
		//					plug.attribute() ==attr.limits_enable_state || 	
		//					plug.attribute() == attr.low_limit ||
		//					plug.attribute() == attr.high_limit  ) 
		//)
	 //{
		// plug.setLocked( true );
	 //}

	
	 return stat;
 }


 void joint_node::attr_changed_cb(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* clientData)
{
	//if( msg & MNodeMessage::kAttributeSet )
	{
		freedom_degree_computeattr_changed_cb( x_translate,  msg, plug, otherPlug, clientData );
		freedom_degree_computeattr_changed_cb( y_translate,  msg, plug, otherPlug, clientData );
		freedom_degree_computeattr_changed_cb( z_translate,  msg, plug, otherPlug, clientData );
		freedom_degree_computeattr_changed_cb( x_rotate,  msg, plug, otherPlug, clientData );
		freedom_degree_computeattr_changed_cb( y_rotate,  msg, plug, otherPlug, clientData );
		freedom_degree_computeattr_changed_cb( z_rotate,  msg, plug, otherPlug, clientData );


	}
}

void joint_node::postConstructor()
{
	MStatus					stat;
	super::postConstructor	();

	MObject o				= thisMObject();
	MCallbackId cbid		= MNodeMessage::addAttributeChangedCallback(o, (MNodeMessage::MAttr2PlugFunction)attr_changed_cb, NULL, &stat);
	CHK_STAT_RV				(stat);
	xray::maya::g_maya_engine.register_callback_id	( cbid );
}


MStatus export_freedom_degree(lua_config_value table, atribute_freedom_degree& att, MFnDependencyNode& node, pcstr name)
{
	lua_config_value degree_table = table[name];
	
	MStatus stat;
	MPlug plug		=node.findPlug(att.enable_state, stat); //bool
	bool			bool_val;
	stat			= plug.getValue	(bool_val);
	degree_table["enable_state"] = bool_val;

	//plug			= node.findPlug(att.limits_enable_state, stat); //bool
	plug			= node.findPlug(att.transform_low_limit_enable, stat); //bool
	CHK_STAT_R( stat );
	stat			= plug.getValue	(bool_val);
	CHK_STAT_R( stat );
	degree_table["limits_enable_state"] = bool_val;
	

	float			float_val;
	//plug			= node.findPlug(att.low_limit, stat); //float
	plug			= node.findPlug(att.transform_low_limit, stat); //float
	CHK_STAT_R		( stat );

	//MAngle ang;
	//stat			= plug.getValue	(ang);
	//MString inf = plug.info();
	//CHK_STAT( stat );

	stat			= plug.getValue	(float_val);

	CHK_STAT_R( stat );

	degree_table["low_limit"] = float_val;//float( ang.asRadians() );//float_val;

	//plug			= node.findPlug(att.high_limit, stat); //float
	plug			= node.findPlug(att.transform_high_limit, stat); //float
	CHK_STAT_R( stat );
	stat			= plug.getValue	(float_val);
	CHK_STAT_R( stat );
	degree_table["hi_limit"] = float_val;

	return			stat;
}

MStatus export_joint_node(lua_config_value table, MFnTransform& node_tr, MFnDagNode& node);

MStatus	joint_node::do_export_node	(xray::configs::lua_config_value cfg )
{
	//if( is_rigid() && can_be_rigid())
	//	return export_rigidly_connected( *cfg );

	if(is_rigid())
		return MStatus();

	lua_config_value joints_table	= (cfg)["joints"];

	//MStatus stat = export_joint_node( joints_table[joints_table.size()], transform_node, dag_node );
	MStatus stat = export_joint( joints_table[joints_table.size()] );
	return stat;
}

bool			joint_node::is_rigid				()
{ 
	MStatus stat;
	MFnDagNode node( thisMObject(), &stat ); CHK_STAT_R	( stat );
	MPlug type_plug		= node.findPlug( joint_node::jointType, true, &stat);CHK_STAT_R	( stat );
	return  type_plug.asShort() == joint_node::joint_rigid;
}

body_node*		joint_node::actual_body	(ebody b)
{	
	body_node* b_node = connected_body_node( b );
	if( b_node )
		return b_node->get_actual_body();
	return 0;
}

body_node*	joint_node::connected_body_node		(joint_node::ebody b)
{
	MStatus stat;	
	pcstr name_str			= (b==b0)?"body0" : "body1";
	MFnDagNode node( thisMObject(), &stat ); CHK_ST	(stat,return 0);
	MFnTransform transform_node( thisMObject() );
	MPlug body_plug			= node.findPlug( name_str, true, &stat);
	CHK_ST	(stat,return 0);
	MPlugArray				arr;
	body_plug.connectedTo	(arr, true, true, &stat); CHK_ST(stat,return 0);
	CHK_ST	(stat,return 0);
	if(arr.length())
	{
		MFnDependencyNode	n( arr[0].node() );
		body_node *bn		= dynamic_cast<body_node*>( n.userNode() );
		return bn;

	}
	return 0;
}

MStatus	joint_node::export_rigidly_connected( lua_config_value table )
{
	ASSERT(is_rigid() );
	body_node *ba0 = actual_body( b0 );
	body_node *bn1 = connected_body_node( b1 );
	if( ba0 && bn1 )
		return bn1->export_collision_to_actual_body( table, ba0 );

	return MStatus();

}



