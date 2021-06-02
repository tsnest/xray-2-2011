////////////////////////////////////////////////////////////////////////////
//	Created		: 29.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "body_node.h"
#include "RenderRoutines.h"
#include <xray/configs.h>
using xray::configs::lua_config_value;

const MTypeId body_node::typeId		( body_node_type_id );
const MString body_node::typeName	( body_node_type_name );

MObject body_node::m_atribute_mass_center;
MObject body_node::m_atribute_mass;
MObject body_node::m_atribute_anim_joint;
MObject body_node::m_atribute_joint;
MObject	body_node::m_attribute_anim_joint_parrent_invert_matrix;
MObject	body_node::m_attribute_self_matrix;
MObject	body_node::m_attribute_anim_joint_rotate;
MObject	body_node::m_attribute_anim_joint_translate;
MObject	body_node::m_attribute_anim_joint_orient;
void colorRGB( M3dView & view, M3dView::DisplayStyle style, M3dView::DisplayStatus displayStatus, MColor& clr )
{
	XRAY_UNREFERENCED_PARAMETER( style );
	switch (displayStatus)
	{
	case M3dView::kActive:
		clr = view.colorAtIndex(18, M3dView::kActiveColors);
		break;
	case M3dView::kDormant:
		clr = view.colorAtIndex(10, M3dView::kDormantColors);
		break;
	case M3dView::kTemplate:
		{
			unsigned short gr = (unsigned short) 0x78;
			clr = MColor(MColor::kRGB, gr, gr, gr);
		}
		break;
	case M3dView::kActiveTemplate:
		clr = view.colorAtIndex(19, M3dView::kActiveColors);
		break;
	case M3dView::kLead:
		clr = view.colorAtIndex(15, M3dView::kActiveColors);
		break;
	default:
		// Leave color unassigned.
		break;
	}
}
body_node::body_node		():
b_joint_orient_computed( false ),
b_joint_translate_computed( false )
{							 
							  
}
void body_node::draw( M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status, MColor _colorRGB )
{
//	MPxLocatorNode::draw			( view, path, style, status );
	super::draw(  view, path, style, status, _colorRGB );
	view.beginGL					( ); 

	glPushAttrib					( GL_CURRENT_BIT );

	unsigned short points_count		= 0;
	MPoint*	pts						= NULL;
	unsigned short* indices			= NULL;
	unsigned short index_count		= 0;
	

	get_points						( &pts, points_count, &indices, index_count );


	glBegin							( GL_LINE_STRIP );

	for( unsigned int i=0; i < points_count; i++ )
		for( unsigned int j=i; j < points_count; j++ )
	{
		glVertex3f( float(pts[i].x), float(pts[i].y), float(pts[i].z) );
		glVertex3f( float(pts[j].x), float(pts[j].y), float(pts[j].z) );

	}
	glEnd							( );

	
	MStatus					stat;
	MVector					mc;
	get_mass_center			(mc);

	// line from center to mass center
	glBegin					(GL_LINES);
		glVertex3f			(0, 0, 0);
		glVertex3f			((float)mc.x, (float)mc.y, (float)mc.z);
	glEnd					( );


	MMatrix m;
	m.setToIdentity			();
	m.matrix[3][0]			= mc.x;
	m.matrix[3][1]			= mc.y;
	m.matrix[3][2]			= mc.z;

	//draw mass center
	float sz = 0.05f * 1.;//(float)MDistance::uiToInternal(1);
	glMatrixMode		(GL_MODELVIEW);
	glPushMatrix		();
	glMultMatrixd		(&m.matrix[0][0]);

	MColor				outlineCol;;
	colorRGB			(view, style, status, outlineCol);
	glColor3f			(1.0f,0.0f,0.0f);
	RenderRoutines::DrawFilledCube		(sz, sz, sz);
	if(status == M3dView::kLead )
		glColor4f(0.0f,1.0f,0.0f,0.5f);
	else
		glColor4f(outlineCol.r, outlineCol.g, outlineCol.b,0.5f);

	RenderRoutines::DrawCrossXY			(sz*2, sz*2);
	RenderRoutines::DrawCrossXZ			(sz*2, sz*2);
	RenderRoutines::DrawCrossYZ			(sz*2, sz*2);

	glPopMatrix			();

	glPopAttrib			();
	view.endGL			();
}

MStatus body_node::get_mass_center(MVector& p)
{
	MObject thisNode			= thisMObject();
	MFnDagNode dagFn			(thisNode);
	MPlug mass_center_plug		= dagFn.findPlug( m_atribute_mass_center);

	mass_center_plug.child(0).getValue(p.x);
	mass_center_plug.child(1).getValue(p.y);
	mass_center_plug.child(2).getValue(p.z);

	return MStatus::kSuccess;
}
 
bool body_node::isBounded() const
{
		return false;
}
//MBoundingBox get_bounding_box(const MPointArray		&pts );
MBoundingBox get_bounding_box(const MPoint* pts, unsigned short points_count);

MBoundingBox body_node::boundingBox() const
{
	unsigned short points_count		= 0;
	MPoint*	pts					= NULL;
	unsigned short* indices			= NULL;
	unsigned short index_count		= 0;
	

	get_points						(&pts, points_count, &indices, index_count );



	return get_bounding_box( pts, points_count ) ;
}

void* body_node::creator()
{
	return CRT_NEW(body_node);
}

MStatus body_node::initialize()
{
	

	MStatus				status;
	

	status = super::initialize();

	CHK_STAT_R			(status);
	MFnNumericAttribute numFn;
	MFnTypedAttribute	typedFn;
	//MFnNumericAttribute numFn = OpenMaya.MFnNumericAttribute();
	m_atribute_mass_center = numFn.create( "MassCenter", "mc", MFnNumericData::k3Double, 0.0 ,&status);
	CHK_STAT_R			(status);
	status = numFn.setDefault	(0.0, 0.0, 0.0);
	status = numFn.setHidden	(false);
	status = numFn.setKeyable	(true);
	status = numFn.setReadable	(true);
	status = numFn.setWritable	(true);
	status = numFn.setStorable	(true);
	status = addAttribute		(m_atribute_mass_center);

	m_atribute_mass = numFn.create( "Mass", "mass", MFnNumericData::kDouble, 0.0 ,&status);
	CHK_STAT_R			(status);
	status = numFn.setMin		(0.001);
	status = numFn.setMax		(10000.0);
	status = numFn.setHidden	(false);
	status = numFn.setKeyable	(true);
	status = numFn.setReadable	(true);
	status = numFn.setWritable	(true);
	status = numFn.setStorable	(true);
	status = addAttribute		(m_atribute_mass);

	m_atribute_anim_joint = typedFn.create( "AnimationJoint", "aj", MFnData::kString, MObject::kNullObj ,&status);
	CHK_STAT_R					(status);
	status = typedFn.setHidden	(false);
	status = typedFn.setKeyable	(true);
	status = typedFn.setReadable(true);
	status = typedFn.setWritable(true);
	status = typedFn.setStorable(true);
	status = addAttribute		(m_atribute_anim_joint);
	
	MFnMessageAttribute	messageFn;
	m_atribute_joint			= messageFn.create("joint", "j", &status);	CHK_STAT_R(status);
	status						= messageFn.setKeyable(true);
	status						= messageFn.setReadable(true);
	status						= messageFn.setWritable(true);
	status						= addAttribute(m_atribute_joint);			CHK_STAT_R(status);

	MFnMatrixAttribute	matrixFn;
	m_attribute_anim_joint_parrent_invert_matrix		= matrixFn.create( "JointParentInvertMatrix", "jpi",MFnMatrixAttribute::kDouble, &status );CHK_STAT_R(status);
	//status						= matrixFn.setKeyable(true);CHK_STAT_R(status);
	status						= matrixFn.setReadable(true);CHK_STAT_R(status);
	status						= matrixFn.setWritable(true);CHK_STAT_R(status);
	status						= matrixFn.setArray(true);CHK_STAT_R(status);
	status						= matrixFn.setAffectsWorldSpace( true );CHK_STAT_R(status);
	status						= matrixFn.setWorldSpace( true );CHK_STAT_R(status);
	
	status						= addAttribute(m_attribute_anim_joint_parrent_invert_matrix);			CHK_STAT_R(status);

	m_attribute_self_matrix		= matrixFn.create( "SelfMatrix", "sm",MFnMatrixAttribute::kDouble, &status );CHK_STAT_R(status);
	//status						= matrixFn.setKeyable(true);CHK_STAT_R(status);
	status						= matrixFn.setReadable(true);CHK_STAT_R(status);
	status						= matrixFn.setWritable(true);CHK_STAT_R(status);
	status						= matrixFn.setArray(true);CHK_STAT_R(status);
	status						= matrixFn.setAffectsWorldSpace( true );CHK_STAT_R(status);
	status						= matrixFn.setWorldSpace( true );CHK_STAT_R(status);
	
	status						= addAttribute(m_attribute_self_matrix);			CHK_STAT_R(status);

	

	
	m_attribute_anim_joint_rotate =numFn.create( "AnimJointRotate", "jr", MFnNumericData::k3Double, 0, &status );CHK_STAT_R(status);
	//status						= numFn.setKeyable(true);CHK_STAT_R(status);
	status						= numFn.setReadable(true);CHK_STAT_R(status);
	status						= numFn.setWritable(true);CHK_STAT_R(status);
	status						= numFn.setAffectsWorldSpace( true );;CHK_STAT_R(status);
	//status						= numFn.setWorldSpace( true );;CHK_STAT_R(status);
	status						= addAttribute(m_attribute_anim_joint_rotate);			CHK_STAT_R(status);


		
	m_attribute_anim_joint_orient =numFn.create( "AnimJointOrient", "jo", MFnNumericData::k3Double, 0, &status );CHK_STAT_R(status);
	//status						= numFn.setKeyable(true);CHK_STAT_R(status);
	status						= numFn.setReadable(true);CHK_STAT_R(status);
	status						= numFn.setWritable(true);CHK_STAT_R(status);
	status						= numFn.setAffectsWorldSpace( true );;CHK_STAT_R(status);
	//status						= numFn.setWorldSpace( true );;CHK_STAT_R(status);
	status						= addAttribute(m_attribute_anim_joint_orient);			CHK_STAT_R(status);
	

	m_attribute_anim_joint_translate =numFn.create( "AnimJointTranslate", "ajt", MFnNumericData::k3Double, 0, &status );CHK_STAT_R(status);
	//status						= numFn.setKeyable(true);CHK_STAT_R(status);
	status						= numFn.setReadable(true);CHK_STAT_R(status);
	status						= numFn.setWritable(true);CHK_STAT_R(status);
	status						= numFn.setAffectsWorldSpace( true );;CHK_STAT_R(status);
	//status						= numFn.setWorldSpace( true );;CHK_STAT_R(status);
	status						= addAttribute(m_attribute_anim_joint_translate);			CHK_STAT_R(status);

	CHK_STAT_R( attributeAffects( m_attribute_anim_joint_parrent_invert_matrix, m_attribute_anim_joint_translate ) );
	//CHK_STAT_R( attributeAffects( m_attribute_anim_joint_parrent_invert_matrix, m_attribute_anim_joint_rotate ) );
	CHK_STAT_R( attributeAffects( m_attribute_anim_joint_parrent_invert_matrix, m_attribute_anim_joint_orient ) );

	CHK_STAT_R( attributeAffects( m_attribute_self_matrix, m_attribute_anim_joint_translate ) );
	//CHK_STAT_R( attributeAffects( m_attribute_self_matrix, m_attribute_anim_joint_rotate ) );
	CHK_STAT_R( attributeAffects( m_attribute_anim_joint_rotate, m_attribute_anim_joint_orient ) );
	CHK_STAT_R( attributeAffects( m_attribute_self_matrix, m_attribute_anim_joint_orient ) );

	//status = inheritAttributesFrom( "xray_constraint" );CHK_STAT_R(status);


	// Notify Maya that there is an associated manipulator for this particular type of node
	status = MPxManipContainer::addToManipConnectTable( const_cast<MTypeId &>( typeId ) );
	
	return				status;
}

MStatus		body_node::compute( const MPlug& plug, MDataBlock& data )
{
	MStatus				status;
	bool compute_joint_translate	= ( plug == m_attribute_anim_joint_translate );
	//bool conpute_joint_rotate		= ( plug == m_attribute_anim_joint_rotate );
	bool compute_joint_orient		= ( plug == m_attribute_anim_joint_orient );

	if( !compute_joint_translate && !compute_joint_orient )
		return				super::compute(plug,data);

	//updateMatrixAttrs( );
	

	MArrayDataHandle arrayHandle =data.inputValue( m_attribute_anim_joint_parrent_invert_matrix, &status );CHK_STAT_R(status);
	
	MMatrix ijoint_parrent;
	if( arrayHandle.elementCount() != 0 )
	{
		status = arrayHandle.jumpToElement( 0 );CHK_STAT_R(status);
		MDataHandle m = arrayHandle.inputValue( &status );CHK_STAT_R(status);
		ijoint_parrent = m.asMatrix();

	}

	arrayHandle =data.inputValue( m_attribute_self_matrix, &status );CHK_STAT_R(status);
	//
	MMatrix self_matrix;
	if( arrayHandle.elementCount() != 0 )
	{
		status = arrayHandle.jumpToElement( 0 );CHK_STAT_R(status);
		MDataHandle m = arrayHandle.inputValue( &status );CHK_STAT_R(status);
		self_matrix = m.asMatrix();

	}

	if( compute_joint_orient )
	{
		const double3 &rorot = data.inputValue( m_attribute_anim_joint_rotate, &status ).asDouble3() ; CHK_STAT_R(status);
		double3  orot;
		MTransformationMatrix o_matrix;
		orot[0] = MAngle( rorot[0], MAngle::kDegrees ).asRadians();
		orot[1] = MAngle( rorot[1], MAngle::kDegrees ).asRadians() ;
		orot[2] = MAngle( rorot[2], MAngle::kDegrees ).asRadians() ;

		status = o_matrix.setRotation( orot, MTransformationMatrix::kXYZ); CHK_STAT_R(status);
		;
		
		MMatrix joint_local = self_matrix * ijoint_parrent ;//* o_matrix.asMatrixInverse() ;

		MTransformationMatrix t_matrix (   joint_local );
		
		MTransformationMatrix::RotationOrder roder;
		double3 &rot = data.outputValue( m_attribute_anim_joint_orient, &status ).asDouble3() ; CHK_STAT_R(status);
		status = t_matrix.getRotation( rot,  roder ); CHK_STAT_R(status);
		rot[0] = MAngle( rot[0] ).asDegrees() ;
		rot[1] = MAngle( rot[1] ).asDegrees() ;
		rot[2] = MAngle( rot[2] ).asDegrees() ;

		b_joint_orient_computed = true;


	}


	if( compute_joint_translate )
	{
		
		MMatrix joint_local = self_matrix * ijoint_parrent;

		MTransformationMatrix t_matrix (   joint_local );
		double unit_internal_to_ui = MDistance::internalToUI( 1. );
		MVector translation = unit_internal_to_ui * t_matrix.getTranslation( MSpace::kWorld, &status ); CHK_STAT_R(status);
		data.outputValue( m_attribute_anim_joint_translate, &status ).asVector() = translation ; CHK_STAT_R(status);
		b_joint_translate_computed = true;
	}

	data.setClean( plug );
	return				status;
}

bool	body_node::first_time_computed ()
{ 
		return	b_joint_translate_computed && b_joint_orient_computed; 
}

const double r_size  = 0.3;
const MVector dx( r_size, 0, 0 );
const MVector dy( 0, r_size, 0 );
const MVector dz( 0, 0, r_size );
MPoint body_points[]={ 
	dz - dx,
	dx + dy,
	dx - dy,
	-dz - dx
};


void body_node::get_points(MPoint** points, unsigned short& points_count, unsigned short** indices, unsigned short& index_count ) const
 {
	*points			= body_points;
	points_count	= 4;
	*indices		= 0;
	index_count		= 0;
 }


  MStatus body_node:: setDependentsDirty( const MPlug& plugBeingDirtied,
                                                                MPlugArray &affectedPlugs )
	{
		MStatus  stat;
		if( plugBeingDirtied==MPxTransform::parentInverseMatrix || plugBeingDirtied==MPxTransform::matrix 
			||plugBeingDirtied==MPxTransform::rotate
			||plugBeingDirtied==MPxTransform::translate
			)
		{
			MPlug attribute_joint_orient(thisMObject(),m_attribute_anim_joint_orient);
			affectedPlugs.append( attribute_joint_orient );
			MPlug attribute_joint_translate(thisMObject(),m_attribute_anim_joint_translate);
			affectedPlugs.append( attribute_joint_translate );

//			MPlug attribute_anim_joint_orient(thisMObject(),m_attribute_anim_joint_orient);
//			affectedPlugs.append( attribute_anim_joint_orient );

		}
		return super::setDependentsDirty( plugBeingDirtied, affectedPlugs );
	}



 MStatus	body_node::validateAndSetValue(const MPlug& plug,
										const MDataHandle& handle,
										const MDGContext& context)
 {
		MStatus  stat;
		if (plug.isNull())
			return MS::kFailure;
		dirtyMatrix();

		//return stat;
		//dh.setClean();
		return super::validateAndSetValue(plug, handle, context);
		
 }

 MMatrix get_displacement(  MFnTransform from,  MFnTransform to)
 {
	 MStatus stat;
	 MDagPath from_path, to_path;
	 stat = from.getPath( from_path ); 
	 stat = to.getPath( to_path );
//////////////////////////////////////////////////////////////////////////////////////////////////
		//MTransformationMatrix  m_to(to_path.inclusiveMatrix( &stat ));

		//MVector vto			= m_to.getTranslation(MSpace::kTransform, &stat) * MDistance::internalToUI(1);
		//	
		//MTransformationMatrix::RotationOrder ro;
		//double rot_to[3];
		//stat				= m_to.getRotation( rot_to, ro );

		//MTransformationMatrix m_fr ( from_path.inclusiveMatrix( &stat ) );
		//
		//MVector vfr			= m_fr.getTranslation(MSpace::kTransform, &stat) * MDistance::internalToUI(1);
		//	
		//
		//double rot_fr[3];
		//stat				= m_fr.getRotation( rot_fr, ro );


	 //	MMatrix m_fri = from_path.inclusiveMatrix( &stat );

		
		
/////////////////////////////////////////////////////////////////////////////////////////////////////
	 MMatrix m = to_path.inclusiveMatrix( &stat ) * from_path.inclusiveMatrixInverse( &stat );
////////////////////////////

	// MTransformationMatrix mm( m );
	// MVector v			= mm.getTranslation(MSpace::kTransform, &stat) * MDistance::internalToUI(1);
			
		
	//	double rot[3];
	//	stat				= mm.getRotation( rot, ro );
////////////////////////////
	 //MMatrix m =	
	//				to.transformationMatrix( &stat ) * from.transformationMatrix( &stat ).inverse()  ; 
	 
	 //from.transformation( &stat ). ;
	 return m;
 }

lua_config_value  find_body( lua_config_value bodies_table, MString name )
{
	pcstr n = name.asChar();
	u32 b_count = bodies_table.size();



	for( u32 i = 0; i < b_count; ++i )
	{
		lua_config_value ret = bodies_table[i];
		if( strcmp( ret["name"], n ) == 0 )
			return ret;
	}
	xray::configs::lua_config_ptr fake_cfg		= xray::configs::create_lua_config();
	lua_config_value ret = *fake_cfg;
	return ret;
}
MStatus  export_body_collision( lua_config_value table, MFnTransform& node_tr, MFnDagNode& node, const MMatrix &dispacement  );

MStatus		body_node::export_collision_to_actual_body	( lua_config_value table, body_node* ab )
{
	
	MStatus stat;
	if(!ab)
	{
		R_ASSERT_U( ab, "root is static elements not supported !!" );
		return stat;
	}
	lua_config_value bodies_table	= table["bodies"];
	lua_config_value ab_table = find_body( bodies_table, ab->name() );
	if( ab_table.empty() )
	{
		ab->export_node( table );

		ab_table = bodies_table[ bodies_table.size()-1 ];
	}

	//MFnDagNode	 actual_body_dag_node	(ab->thisMObject());	
	MFnTransform actual_body_transform_node( ab->thisMObject() );

	MFnDagNode	 this_body_dag_node	(thisMObject());	
	MFnTransform this_body_transform_node( thisMObject() );

	return export_body_collision( ab_table, this_body_transform_node, this_body_dag_node, get_displacement( actual_body_transform_node, this_body_transform_node ) );
}

MStatus export_body_node(lua_config_value table, MFnTransform& node_tr, MFnDagNode& node);
bool		body_node::is_actual_body( )
{ 
	return !super::is_rigid()||!super::connected_body_node(super::b0); 
}
MStatus	body_node::do_export_node	( xray::configs::lua_config_value cfg )
{
	
	MStatus stat = super::do_export_node( cfg );CHK_STAT_R(stat);


	if(!is_actual_body())
		return export_collision_to_actual_body( cfg, get_actual_body() );

	lua_config_value bodies_table	= (cfg)["bodies"];

	if( !find_body( bodies_table, name() ).empty() )
		return stat;

	MFnDagNode	 dag_node	(thisMObject());	
	MFnTransform transform_node( thisMObject() );
	stat = export_body_node( bodies_table[bodies_table.size()], transform_node, dag_node );CHK_STAT_R(stat);
	return stat;
}
body_node	*body_node::get_actual_body			()
{
	if(is_actual_body())
		return this;
	body_node* p =  super::connected_body_node( super::b0 );
	if( p )
		return p->get_actual_body();
	return 0;
}