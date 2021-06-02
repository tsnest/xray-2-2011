#include "pch.h"

#include "anim_text_file_export.h"
#include "anim_export_util.h"
#include "export_utils.h"

#include <xray/animation/api.h>
#include <xray/animation/world.h>
#include <xray/animation/i_editor_animation.h>
#include <xray/animation/sources/bi_spline_skeleton_animation.h>

#include <xray/maya_animation/discrete_data.h>
#include <xray/maya_animation/world.h>
#include <xray/fs/device_utils.h>
#include "maya_engine.h"

using	namespace xray::animation;
using	namespace xray::maya_animation;

using	xray::maya_animation::discrete_data;
using	xray::maya::maya_engine;
using	xray::maya::g_maya_engine;

MStatus		export_bi_splines		( MSelectionList &all_joints, const anim_exporter_options &opts, xray::configs::lua_config_value & ,xray::configs::lua_config_value &  );
MStatus		export_animation		( const anim_exporter_options &opts, MString path  );



const MString anim_export_cmd::Name	( "xray_anim_export" );



static pcstr localCoordinatCenterNodeFlag		= "-l";
static pcstr localCoordinatCenterNodeLongFlag	= "-localCoordinatCenterNode";

static pcstr additiveToAnimationFlag			= "-a";
static pcstr additiveToAnimationLongFlag		= "-additiveToAnimation";

static pcstr precision_t_Flag					= "-pt";
static pcstr precision_t_LongFlag				= "-precision_t";

static pcstr precision_r_Flag					= "-pr";
static pcstr precision_r_LongFlag				= "-precision_r";

static pcstr precision_s_Flag					= "-ps";
static pcstr precision_s_LongFlag				= "-precision_s";

MSyntax	anim_export_cmd::newSyntax()
{
	MSyntax	syntax;
	
	//syntax.addArg( 
	syntax.addFlag( file_name_flag, file_name_flag_l,	MSyntax::kString );

	syntax.addFlag( localCoordinatCenterNodeFlag,	localCoordinatCenterNodeLongFlag,	MSyntax::kString );
	syntax.addFlag( additiveToAnimationFlag,		additiveToAnimationLongFlag,		MSyntax::kString );
	syntax.addFlag( precision_t_Flag,				precision_t_LongFlag,				MSyntax::kDouble );
	syntax.addFlag( precision_r_Flag,				precision_r_LongFlag,				MSyntax::kDouble );
	syntax.addFlag( precision_s_Flag,				precision_s_LongFlag,				MSyntax::kDouble );
	
	return syntax;
}

MStatus	anim_export_cmd::doIt( const MArgList& args )
{
	using namespace xray::fs_new;

	MStatus				stat;

	MArgDatabase argData( syntax(), args );
	MString				file_name = file_name_from_args( argData, &stat );
	
	CHK_STAT_R			( stat );		

	MString		localCoordinatCenterNode;
	bool		b_localCoordinatCenterNode = argData.isFlagSet( localCoordinatCenterNodeFlag );

	if( b_localCoordinatCenterNode )
	{
		stat			= argData.getFlagArgument( localCoordinatCenterNodeFlag, 0, localCoordinatCenterNode );
		CHK_STAT_R		( stat );
	}

	MString		additiveToAnimation;
	bool		b_additiveToAnimation = argData.isFlagSet( additiveToAnimationFlag );
	if( b_additiveToAnimation )
	{
		stat			= argData.getFlagArgument( additiveToAnimationFlag, 0, additiveToAnimation );
		CHK_STAT_R		( stat );
	}

	anim_exporter_options options(	b_localCoordinatCenterNode, 
									localCoordinatCenterNode, 
									b_additiveToAnimation, additiveToAnimation );

	if( argData.isFlagSet( precision_t_Flag ) )
	{
		double value;
		argData.getFlagArgument( precision_t_Flag, 0, value );
		options.m_precision_t	= float(value);
	}
	if( argData.isFlagSet( precision_r_Flag ) )
	{
		double value;
		argData.getFlagArgument( precision_r_Flag, 0, value );
		options.m_precision_r	= float(value);
	}
	if( argData.isFlagSet( precision_s_Flag ) )
	{
		double value;
		argData.getFlagArgument( precision_s_Flag, 0, value );
		options.m_precision_s	= float(value);
	}

	if( options.status() == MStatus::kFailure )
		return  MStatus::kFailure;
	
	synchronous_device_interface const & device	=	xray::resources::get_synchronous_device();
	create_folder_r		( device, file_name.asChar(), false );
	
	if( MString( extension_from_path( file_name.asChar() ) ) != "clip" )
		file_name += ".clip" ;

	stat = export_animation		( options, file_name );

	xray::resources::wait_and_dispatch_callbacks( true );

	xray::memory::dump_statistics( true );

	return stat;
}

anim_exporter_options::anim_exporter_options( bool local, const MString& local_coordinat_center_node_name, bool additive,  const MString& additive_animation_path )
:m_additive					( additive ),
m_local						( local ),
m_additive_animation_path	( additive_animation_path ),
m_status					( MStatus::kFailure ),
m_precision_t				( 0.005f ),
m_precision_r				( 0.005f ),
m_precision_s				( 0.005f )

{
	m_status = MStatus::kSuccess;
	if( local )
		set_local_animation_position( local_coordinat_center_node_name );

}

MString  remove_space( MString arg )
{
	//int i = 0;
	//for(;arg.split;)
	MStringArray sr;
	arg.split(' ', sr );

	return sr[0];
}

bool get_arg(const MString opt_name, MString opt_string,  MString &arg )
{
	MStringArray opts1;
	opt_string.split( '=', opts1 );

	
	MString lopt	= remove_space ( opts1[0] );

	if( opt_name != lopt )
	{
		 arg = "";
		 return false;
	}

	arg	= remove_space ( opts1[1] );
	 
	return true;

}

//anim_exporter_options::anim_exporter_options( const MString& options ):
//m_additive		( false ),
//m_local			( false ),
//m_status		( MStatus::kFailure )
//{
//	
//	MStringArray opts;
//	options.split( ';', opts );
//
//	const u32 num_opts = opts.length();
//	
//	m_status = MStatus::kSuccess;
//
//	if( num_opts == 0 )
//	{
//		display_warning( MString( "anim_exporter_options: LocalCoordinatCenter not set" ) ) ;
//		return;
//	}
//
//	MString node_name;
//	if( get_arg("LocalCoordinatCenter", opts[0], node_name ) )
//		set_local_animation_position( node_name );
//	 else
//	{
//		display_warning( MString( "anim_exporter_options: LocalCoordinatCenter not set" ) ) ;
//		return;
//	}
//}

MStatus anim_exporter_options::set_local_animation_position( const MString& node_name )
{
	
	m_status = MStatus::kFailure;

	MSelectionList list;
	
	MGlobal::getSelectionListByName( node_name, list );

	if( list.length() == 0 )
	{
		display_warning( MString( "anim_exporter_options: LocalCoordinatCenter not found" ) ) ;
		
		return m_status;
	}

	if( list.length() > 1 )
	{
		display_warning( MString( "anim_exporter_options: LocalCoordinatCenter ambiguous" ) ) ;
		return m_status;
	}

	MObject locator;
	CHK_STAT_R( list.getDependNode( 0, locator ) );

	if( !locator.hasFn( MFn::kTransform ) )
	{
		display_warning( MString( "anim_exporter_options: LocalCoordinatCenter is not a transform node" ) ) ;
		return m_status;
	}
	
	MFnTransform locator_transform( locator, &m_status ); 
	CHK_STAT_R( m_status );
	
	MDagPath path;
	locator_transform.getPath( path );

	m_local_animation_position = path.inclusiveMatrix( &m_status );  
	CHK_STAT_R( m_status );

	m_status = MStatus::kSuccess;

	m_local = true;
	return m_status;
}

MStatus export_animation( const anim_exporter_options &options, MString path  )
{
	
	if( options.status() == MStatus::kFailure )
		return MStatus::kFailure;

	MStringArray					all_joints_str;
	MGlobal::executeCommand			("skinCluster -q -inf", all_joints_str);
	MSelectionList all_joints;
	int length						= all_joints_str.length();
	
	if( length <= 0 )
	{
		display_warning( "anim_exporter: No animation joints found. Select a skin claster!" );
		display_info	( "Export " + path + " FAILED!\n" );
		return MStatus::kFailure;
	}

	//MString str = "found ";	str += length;	str += " animation joints";
	//display_info(str);
	
	for(int i=0; i<length; ++i)
		all_joints.add( all_joints_str[i] );


	xray::configs::lua_config_ptr cfg_splines		= xray::configs::create_lua_config();
	xray::configs::lua_config_value  v_cfg_splines  = *cfg_splines;

	xray::configs::lua_config_ptr	cfg_check_data		= xray::configs::create_lua_config();
	xray::configs::lua_config_value  v_cfg_check_data  = *cfg_check_data;

	CHK_STAT_R ( export_bi_splines( all_joints, options, v_cfg_splines, v_cfg_check_data ) );

	(*cfg_splines)["splines"]["is_additive"]	= options.is_additive();

	cfg_splines->save_as	( path.asChar(), xray::configs::target_sources );
	cfg_check_data->save_as	( ( path + "_check_data" ).asChar(), xray::configs::target_sources );

	//cfg_check_data->save_as_binary(( path + "_check_data_b" ).asChar());

	display_info	( "Export " + path + " successful!\n" );

	return	(MS::kSuccess);
}






	//\li matrix = [S] * [RO] * [R] * [JO] * [IS] * [T]


	//(where '*' denotes matrix multiplication).

	//These matrices are defined as follows:


	//\li <b>[S]</b> : scale
	//\li <b>[RO]</b> : rotateOrient (attribute name is rotateAxis)
	//\li <b>[R]</b> : rotate
	//\li <b>[JO]</b> : jointOrient
	//\li <b>[IS]</b> : parentScaleInverse
	//\li <b>[T]</b> : translate

//The methods to get the value of these matrices are:
//	\li <b>[S]</b> : getScale
//	\li <b>[RO]</b> : getScaleOrientation
//	\li <b>[R]</b> : getRotation
//	\li <b>[JO]</b> : getOrientation
//	\li <b>[IS]</b> : (the inverse of the getScale on the parent transformation matrix)
//	\li <b>[T]</b> : translation

static const int max_frame = 500;
//MStatus export_transform(lua_config_value table, MFnTransform& node_tr )

MStatus get_joint_params(const MFnIkJoint &jnt, MEulerRotation &rot, MVector &translation, double scale[3] )
{
	MStatus status;

	MEulerRotation R;
	MQuaternion RO;
	MEulerRotation JO;
	CHK_STAT_R( jnt.getScaleOrientation( RO ) );
	CHK_STAT_R( jnt.getOrientation( JO ) );
	CHK_STAT_R( jnt.getRotation( R ) );
	rot = RO.asEulerRotation()  * R * JO;
	MEulerRotation::RotationOrder ro = MEulerRotation::kXYZ ;

	ASSERT_U( rot.order == ro );
	translation = jnt.translation( MSpace::kTransform, &status );
	CHK_STAT_R(status);
	
	jnt.getScale( scale );
	return status;
}

MStatus params_from_matrix( MMatrix const&  m,  MEulerRotation &rot, MVector &translation, double scale[3] )
{
	MStatus status;

	MTransformationMatrix transformation( m );
	ASSERT( transformation.rotationOrder( &status ) == MTransformationMatrix::kXYZ );

	//transformation.reorderRotation( MTransformationMatrix::kXYZ ) 
	rot = transformation.eulerRotation();

	ASSERT( rot.order == MEulerRotation::kXYZ );
	//rot.reorderIt( MEulerRotation::kXYZ );

	translation = transformation.getTranslation( MSpace::kTransform, &status );
	CHK_STAT_R( status );
	CHK_STAT_R( transformation.getScale( scale, MSpace::kTransform ) );
	return status;
}

MStatus get_global_joint_params( MFnIkJoint const &jnt, MMatrix const& parent, MEulerRotation &rot, MVector &translation, double scale[3] )
{
	MStatus status;
	MDagPath	my_path;
	CHK_STAT_R( jnt.getPath( my_path ) );
	
	MMatrix  m = my_path.inclusiveMatrix(&status)*parent.inverse(); 
	CHK_STAT_R( status );

	status = params_from_matrix( m, rot, translation, scale );
	CHK_STAT_R( status );
	return status;
}

MObject parent_joint( const MFnIkJoint &jnt );

//void calculate_local_joint_params( MFnIkJoint const &jnt, MEulerRotation &rot, MVector &translation, double scale[3] )
//{
//	MStatus status;
//	MDagPath	my_path;
//	CHK_STAT( jnt.getPath( my_path ) );
//	MMatrix  m = my_path.inclusiveMatrix(&status); CHK_STAT( status );
//
//	MObject parent_j = parent_joint( jnt );
//	if( !parent_j.isNull() )
//	{
//		ASSERT( parent_j.hasFn( MFn::kJoint ) );
//		MFnIkJoint pj( parent_j, &status ); CHK_STAT( status );
//		
//		MDagPath	parrent_path;
//		CHK_STAT( pj.getPath( parrent_path ) );
//		MMatrix  pm = parrent_path.inclusiveMatrix(&status); CHK_STAT( status );
//		
//		pm.inverse();
//		
//		MTransformationMatrix tm( m ), tp( pm );
//		
//		tm = tm * tp;
//
//		m = tm.asMatrix();// m * pm;
//
//	}
//
//	params_from_matrix( m, rot, translation, scale );
//}

inline MVector convert_to_meters( const MVector &translation_ )
{
	MDistance temp; temp.setUnit( MDistance::internalUnit() );
	MVector translation = translation_;
	temp.setValue( translation.x ); translation.x = temp.asMeters();
	temp.setValue( translation.y ); translation.y = temp.asMeters();
	temp.setValue( translation.z ); translation.z = -temp.asMeters();
	return translation;
}

void joint_params_to_frame( const MEulerRotation &rot, const MVector &translation, const double scale[3], frame &frm )
{
	frm.rotation = xray::float3( float( rot.x ),float( rot.y ),float( rot.z ));
	rotation_maya_to_xray( frm.rotation );

	frm.translation = xray::float3(float( translation.x ),float( translation.y ),float( translation.z ));
	translation_maya_to_xray( frm.translation );
	frm.scale = xray::float3(float( scale[0] ),float( scale[1] ),float( scale[2] ));

	ASSERT( xray::math::valid( frm.translation.x ) );
	ASSERT( xray::math::valid( frm.translation.y ) );
	ASSERT( xray::math::valid( frm.translation.z ) );
	ASSERT( xray::math::valid( frm.rotation.x ) );
	ASSERT( xray::math::valid( frm.rotation.y ) );
	ASSERT( xray::math::valid( frm.rotation.z ) );
	ASSERT( xray::math::valid( frm.scale.x ) );
	ASSERT( xray::math::valid( frm.scale.y ) );
	ASSERT( xray::math::valid( frm.scale.z ) );
}

void get_joint_global_frame( MFnIkJoint const &jnt, frame &frm )
{
		MStatus status;
		MEulerRotation rot;
		MVector translation;
		double scale[3];
		get_global_joint_params( jnt, MMatrix(), rot, translation, scale );
		joint_params_to_frame( rot, translation, scale, frm );
}

void get_root_joint_global_frame( MFnIkJoint const &jnt, frame &frm, MMatrix  const &local_animation_position )
{
		MStatus status;
		MEulerRotation rot;
		MVector translation;
		double scale[3];
		get_global_joint_params( jnt, local_animation_position, rot, translation, scale );
		joint_params_to_frame( rot, translation, scale, frm );
}

void get_joint_local_from_global_frame( MFnIkJoint const &jnt, frame &frm )
{
	MObject parent_j = parent_joint( jnt );
	ASSERT (!parent_j.isNull() );
	ASSERT( parent_j.hasFn( MFn::kJoint ) );
	
	get_joint_global_frame( jnt, frm );
	
	frame pfrm;
	get_joint_global_frame( parent_j ,pfrm );

	xray::float4x4 par = frame_matrix( pfrm );
	par.try_invert( xray::float4x4 ( par ) );

	xray::float4x4 m = frame_matrix( frm );
	xray::math::float3 s1 = m.get_scale();
	xray::math::float3 s2 = par.get_scale();
	frm = matrix_to_frame ( m * par);

}

void get_joint_local_frame( MFnIkJoint const &jnt, frame &frm )
{


	MEulerRotation rot;
	MVector translation;
	double scale[3];
	get_joint_params( jnt, rot, translation, scale );
	//get_joint_params( jnt, rot, translation, scale );

	joint_params_to_frame( rot, translation, scale, frm );


}

typedef xray::maya::vector< MObject >	joints_vector_type;


bool insert_joint( joints_vector_type& joints, MObject &joint )
{
	if( joint.isNull() )
		return false;

	if( !joint.hasFn( MFn::kJoint ) )
		return false;
	
	joints_vector_type::const_iterator i = std::find( joints.begin(), joints.end(), joint );
	
	if( i != joints.end() )
		return false;
	
	joints.push_back( joint );

	return true;
}

MStatus	retrive_parrent_joints( const MObject &oblect, joints_vector_type& joints )
{
	MStatus	status;

	MFnDagNode	oblect_dag_node( oblect, &status ); 
	CHK_STAT_R( status );

	const u32 pcnt = oblect_dag_node.parentCount( &status  ); 
	CHK_STAT_R( status );
	
	for( u32 i = 0; i < pcnt; ++i )
	{
		MObject p_obj = oblect_dag_node.parent( i, &status ); 
		CHK_STAT_R( status );

		if( !insert_joint( joints, p_obj ) )
		{
			status = retrive_parrent_joints( p_obj, joints );
			CHK_STAT_R(status);
		}
	}

	return status;
}

MObject parent_joint( const MFnIkJoint &jnt )
{
	joints_vector_type joints;

	retrive_parrent_joints( jnt.object(), joints );

	if( joints.size() == 0 )
		return MObject();

	if( joints.size() > 1 )
		display_warning( " more than one parrent joint ! " );

	return joints[0];

}


MStatus retrive_root_joints( const MObject &oblect, MObject &current_joint,  joints_vector_type& joints )
{
	MStatus	status;
	
	if( oblect.hasFn( MFn::kJoint ) )
		current_joint = oblect;
			
	MFnDagNode	oblect_dag_node( oblect, &status ); 
	CHK_STAT_R( status );

	const u32 pcnt = oblect_dag_node.parentCount( &status  ); 
	CHK_STAT_R( status );
	
	if( pcnt == 0 )
		insert_joint( joints, current_joint );
			//joints.push_back( current_joint );

	for( u32 i = 0; i < pcnt; ++i )
	{
		MObject p_obj = oblect_dag_node.parent( i, &status ); 
		CHK_STAT_R( status );

		status = retrive_root_joints( p_obj, current_joint, joints );
		CHK_STAT_R(status);
	}
	return status;
}


u32 retrive_root_joints( const MObject &oblect, joints_vector_type& joints )
{
	MObject	null_object;
	retrive_root_joints( oblect, null_object , joints );
	return joints.size();
}


MObject retrive_root_joint( const MObject &oblect )
{
	joints_vector_type vec_root_joints;
	
	u32 cnt_root_joints = retrive_root_joints( oblect, vec_root_joints );

	if( cnt_root_joints > 1 )
		display_warning( " more than one root joint ! " );

	if( cnt_root_joints == 0 )
		return MObject();

	return vec_root_joints[0];

}

bool has_parent_joint( const MFnIkJoint &jnt )
{


	MObject p = parent_joint( jnt );

	return !p.isNull();
}

bool is_object_mover( const MFnIkJoint &jnt )
{
	
	return ! has_parent_joint( jnt );

}

MStatus object_mover_id( const MSelectionList &all_joints, u32& result )
{
	MStatus	status;
	
	result = u32(-1);
	
	int length = all_joints.length		();
	MString ret_jnt_name;
	for( int i=0; i < length; ++i )
	{
		MObject		anim_joint;
		status = all_joints.getDependNode( i, anim_joint ); 
		CHK_STAT_R( status );
		MFnIkJoint jnt( anim_joint, &status ); 
		CHK_STAT_R( status );
		
		if( is_object_mover( jnt ) )
		{
			//R_ASSERT_U( ret == u32(-1), "object_mover ambiguous" );
			if( result != u32(-1) )
			{
				display_warning(  
					"object_mover ambiguous! used: "  + ret_jnt_name +
					" - but for " + jnt.name() + " parent joint not found as well"

				);
				continue;
			}

			ret_jnt_name = jnt.name();
			result = i;
		}
	}

	R_ASSERT_U( result != u32(-1), "can not define object_mover" );
	return status;
}

xray::animation::i_editor_animation* create_base_animation ( const anim_exporter_options &opts, MStatus* stat  )
{
	if( stat )
		*stat = MStatus::kSuccess;

	ASSERT( opts.status() == MStatus::kSuccess );
	if( !opts.is_additive() )
		return 0;
	
	
	bool right_extension	= MString( xray::fs_new::extension_from_path( opts.additive_animation_path().asChar() ) ) == "clip";
	bool no_extension		= MString( xray::fs_new::extension_from_path( opts.additive_animation_path().asChar() ) ) == "";

	if( !right_extension && !no_extension )
	{
		display_warning( "bad extension in additive base name: " + opts.additive_animation_path()  );
		*stat = MStatus::kFailure;
		return 0;
	}

	xray::fs_new::virtual_path_string virtual_path;
	xray::resources::convert_physical_to_virtual_path( 
		&virtual_path, 
		xray::fs_new::native_path_string::convert(opts.additive_animation_path().asChar()).make_lowercase(), 
		xray::resources::exported_mount );

	if( right_extension )
	{
		virtual_path[	virtual_path.find(".") ] = '\0';
	}


	//xray::animation::i_editor_animation* anim = create_editor_animation( opts.additive_animation_path().asChar() );
	xray::animation::i_editor_animation* anim = create_editor_animation( virtual_path.c_str() );
	
	if( !anim )
		display_warning( "can not find: " + opts.additive_animation_path()  );
	
	if( !anim && stat )
		*stat = MStatus::kFailure;

	return anim;


}

xray::animation::frame get_base_frame( pcstr bone, float time, const xray::animation::i_editor_animation* base_anim )
{
	if( !base_anim )
		return zero;
	
	if( base_anim->bone_index( bone ) == xray::animation::bone_index_type( -1 ) ) 
		return zero;

	xray::animation::frame f;

	base_anim->evaluate_frame( bone, time, f );

	return f;

}

MStatus write_discrete_data( discrete_data &data, const MSelectionList &all_joints, const anim_exporter_options &opts )
{
	XRAY_UNREFERENCED_PARAMETERS( opts );
	
	MStatus	status;	
	
	CHK_STAT_R( check_claster_joints ( all_joints ) );
	
	int length = all_joints.length	( &status ); 
	CHK_STAT_R( status );

		// Remember the frame the scene was at so we can restore it later.
	MTime currentFrame	= MAnimControl::currentTime();
	MTime startFrame	= MAnimControl::minTime();
	MTime endFrame		= MAnimControl::maxTime();
	

	xray::animation::i_editor_animation *base_animation = create_base_animation( opts, &status );

	if( !status  )
	{	
		destroy_editor_animation( base_animation );
		return status;
	}

	int frameFirst			= (int) startFrame.as( MTime::uiUnit() );
	int frameLast			= (int) endFrame.as( MTime::uiUnit() );

	MTime	tmNew; tmNew.setUnit		(MTime::uiUnit());

	if (currentFrame.unit()!=MTime::kNTSCFrame){
		display_warning("!Can't export animation with FPS!=30.f") ;
		destroy_editor_animation( base_animation );
		return MStatus::kFailure;
	}

	data.set_bone_count( length + 1 );
	xray::math::clamp( frameLast, 0, max_frame );
	
	u32 id_object_mover;
	status = object_mover_id( all_joints, id_object_mover );
	CHK_STAT_R(status);
	ASSERT( id_object_mover < u32( length + 1 ) );

	//int from_frame = xray::math::max( opts.min_frame, startFrame );
	//int to_frame = !opts.cycled ? xray::math::min( opts.max_frame, frameLast ) : opts.max_frame;
	//MTime time; time.setUnit( MTime::currentFrame.unit() );
	//for(int ji=0; ji<length; ++ji)
	//{
	//	MDagPath pth;
	//	all_joints.getDagPath   ( ji, pth );
	//	LOG_ERROR	("[%d]=%s", ji, pth.fullPathName().asChar());
	//}
	for ( int j = frameFirst; j <= frameLast; j++ )//frameLast
	{
		tmNew.setValue ( j );
		MGlobal::viewFrame( tmNew );
		
		for( int i=0; i<length; ++i )
		{
			MObject							anim_joint;
			all_joints.getDependNode		( i, anim_joint );
			
			MFnIkJoint jnt( anim_joint, &status ); 
			CHK_STAT_R( status );
			
			pcstr joint_name = jnt.name().asChar();
			frame base_frame = get_base_frame( joint_name, 0.f + float( j - frameFirst), base_animation ); 
			CHK_STAT_R( status );

			if( id_object_mover == u32( i ) )
			{
				frame frm;
				get_root_joint_global_frame( jnt, frm, opts.animation_position() );

				data.add_frame( 0, frm, base_frame );
				data.add_frame( i + 1, zero, zero );

				continue;
			}

			frame frm;
			get_joint_local_from_global_frame(  jnt, frm  );
			//get_joint_local_frame( jnt, frm );

			data.add_frame( i + 1, frm, base_frame );
		}

	}

	destroy_editor_animation( base_animation );

	data.calculate();
	MGlobal::viewFrame( currentFrame );

	return MStatus::kSuccess;
}

#include <io.h>


static xray::threading::mutex s_build_animation_data_mutex;
static bool s_build_animation_data_ready = false;

void build_animation_data_thread( const discrete_data *data, bi_spline_skeleton_animation *animation_data )
{
	s_build_animation_data_mutex.lock();
	
	u32 id_thread = xray::maya::g_allocator.user_thread_id();
	xray::maya::g_allocator.user_current_thread_id();

	g_maya_engine.maya_animation_world->build_animation_data( *data, *animation_data );
	
	xray::maya::g_allocator.user_thread_id( id_thread );
	
	s_build_animation_data_ready = true;

	s_build_animation_data_mutex.unlock();
}

void run_build_animation_data_thread( const discrete_data *data, bi_spline_skeleton_animation *animation_data )
{
	s_build_animation_data_ready = false;
	xray::threading::spawn( boost::bind( &build_animation_data_thread, data, animation_data ), "build_animation_data_thread", "build_animation_data_thread", 0, 0, xray::threading::tasks_aware ); ;

	for(;;){
		s_build_animation_data_mutex.lock();
			if( s_build_animation_data_ready )
				break;
		s_build_animation_data_mutex.unlock();
	} ;
	
	s_build_animation_data_mutex.unlock();
}

using xray::animation::enum_channel_id;
MStatus export_bi_splines(	MSelectionList &all_joints, 
							const anim_exporter_options &options, 
							xray::configs::lua_config_value &cfg_splines, 
							xray::configs::lua_config_value &cfg_check_data  )
{

	MStatus	status;
	ASSERT							( g_maya_engine.maya_animation_world );
	ASSERT							( g_maya_engine.animation_world );
	discrete_data *data				= g_maya_engine.maya_animation_world->create_discrete_data();
	data->set_precision				( options.m_precision_t, options.m_precision_r, options.m_precision_s );
	status = write_discrete_data	( *data, all_joints, options );
		
	if( !status )
	{
		g_maya_engine.maya_animation_world->destroy( data );
		return status;
	}

	xray::animation::bi_spline_skeleton_animation *skel = g_maya_engine.animation_world->create_skeleton_animation_data( 0 );

	{
		xray::configs::lua_config_value tmp = cfg_check_data["discrete_data"] ;
		data->save( tmp );
	}

//	{
//		using xray::configs::lua_config_ptr;
//		lua_config_ptr temp_config	= xray::configs::create_lua_config();
//		*temp_config				= v;
//
//		pcstr const file_name		= "d:/temp.lua";
//		temp_config->save_as		( file_name );
//
//		FILE* f						= fopen(file_name, "rb" );
//		u32 const file_size			= _filelength(_fileno(f));
//		pstr buffer					= (pstr)MALLOC( file_size + 1, "test temp animation file" );
//		fread						( buffer, file_size, 1, f );
//		fclose						( f );
//
//		buffer[file_size]			= 0;
//		lua_config_ptr const temp	= xray::configs::create_lua_config_from_string( buffer );
//		FREE						( buffer );
//
//		data->check					( (*temp)["discrete_data"] );
////		data->load					( (*temp)["discrete_data"] );
//	}

	run_build_animation_data_thread( data, skel );

	//g_maya_engine.maya_animation_world->build_animation_data( *data, *skel );
	
	int length = all_joints.length( );

	//skel->get_bone_names()->set_bones_number( length + 1 );
	skel->set_bone_names_num_bones( length + 1  );
	skel->set_bone_name( 0, object_mover_bone_name );
	for( int i = 0; i< length; ++ i)
	{
			MObject							anim_joint;
			all_joints.getDependNode		( i  , anim_joint );
			MFnIkJoint jnt( anim_joint, &status ); 
			CHK_STAT_R( status );
			skel->set_bone_name( i + 1 , jnt.name().asChar() );
	}
	
	{
		xray::configs::lua_config_value tmp = cfg_splines["splines"];
		skel->save( tmp );
	}

	g_maya_engine.maya_animation_world->destroy( data );
	g_maya_engine.animation_world->destroy( skel );

	return status;
}
