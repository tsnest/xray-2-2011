////////////////////////////////////////////////////////////////////////////
//	Created		: 08.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "create_collision_cmd.h"
#include "collision_node.h"
#include <xray/math_utils.h>
#include "fake_drawing_locator.h"
#include <wildmagic/sdk/include/Wm4ContBox3.h>
#include <wildmagic/sdk/include/Wm4ContSphere3.h>

const MString createCollisionCmd::Name	( "xray_collision" );

static const char *nameFlag = "-n",		*nameLongFlag	= "-name";
static const char *typeFlag = "-t",		*typeLongFlag	= "-type";
static const char *parentFlag = "-p",	*parentLongFlag = "-parent";
static const char *autoFlag = "-a",		*autoLongFlag	= "-auto";

MSyntax createCollisionCmd::newSyntax()
{
    MSyntax syntax;
	syntax.addFlag(nameFlag,			nameLongFlag,			MSyntax::kString);
	syntax.addFlag(typeFlag,			typeLongFlag,			MSyntax::kString);
	syntax.addFlag(parentFlag,			parentLongFlag,			MSyntax::kString);
	syntax.addFlag(autoFlag,			autoLongFlag,			MSyntax::kString);

	return syntax;
}

MStatus get_skin_cluster(MFnDependencyNode& joint_node, MObject& result )
{
	MStatus				stat;
	MPlug skin_link		= joint_node.findPlug("lockInfluenceWeights", true, &stat);
	CHK_STAT_R			(stat);
	MPlugArray			arr;
	skin_link.connectedTo(arr, false, true, &stat);
	CHK_STAT_R			(stat);
	
	if(arr.length()==0 || arr[0].node().apiType()!=MFn::kSkinClusterFilter)
	{
		stat = MStatus::kNotFound;
	}else
		result = arr[0].node();

	return stat;
}

u32 make_point_index(u32 point_index, u32 joint_index, u32 influence_count)
{
	return 	(point_index*influence_count) + joint_index;
}

bool is_max_weight(MDoubleArray const& weights_list, u32 point_index, u32 joint_index, u32 influence_count)
{
	double joint_w	= weights_list[make_point_index(point_index, joint_index, influence_count)];
	
	for(u32 i=0; i<influence_count; ++i)
	{
		if(i==joint_index)
			continue;

		double w	= weights_list[make_point_index(point_index, i, influence_count)];
		if(w>joint_w)
			return false;
	}
	return				true;
}

MObject get_node_by_name(MString const& name, MStatus& stat)
{
	MObject				result;
	MSelectionList		lst;
	MGlobal::getSelectionListByName(name, lst);
	if(lst.length()==1)
	{
		//MObject			parent;
		stat			= lst.getDependNode(0, result );
		if(!result.hasFn(MFn::kDependencyNode))
			stat = MStatus::kFailure;
		if(stat.error())
			return result;
		//stat			= MStatus(MStatus::kSuccess);
	}else
	{
		stat			= MStatus(MStatus::kNotFound);
	}

	return result;
}


MStatus createCollisionCmd::doIt( const MArgList &args )
{ 
	MArgDatabase argData	(syntax(), args);

	MStatus					stat;
	MObject object			= dagMod.createNode(collision_node::typeId, MObject::kNullObj, &stat);
	CHK_STAT_R				(stat);
	stat					= dagMod.doIt();
	CHK_STAT_R				(stat);
	
	MObject constraintLocatorObj = dagMod.createNode(fake_drawing_locator::typeId, object, &stat);
	dagMod.doIt();
	CHK_STAT_R(stat);

	MFnDagNode				dagNodeTransform(object, &stat);
	MDagPath				path;
	stat					= dagNodeTransform.getPath(path);
	//path.extendToShape		();
	MFnDependencyNode		depNodeBody(path.node(), &stat);
    


	if(argData.isFlagSet(nameFlag))
	{
		MString				object_name;
		stat				= argData.getFlagArgument(nameFlag, 0, object_name);
		CHK_STAT_R			(stat);
		dagNodeTransform.setName(object_name, &stat);
		CHK_STAT_R			(stat);
		depNodeBody.setName	(MString(object_name+"Shape"), &stat);
		CHK_STAT_R			(stat);
	}
	if(argData.isFlagSet(parentFlag))
	{
		MString				parent_name;
		stat				= argData.getFlagArgument(parentFlag, 0, parent_name);
		CHK_STAT_R			(stat);

		MDagPath			parent;
		stat				= get_path_by_name( parent_name, parent, true );
		stat				= dagMod.reparentNode(object, parent.node());
		CHK_STAT_R			(stat);
		dagMod.doIt			();
	}
	if(argData.isFlagSet(typeFlag))
	{
		MString				type_name;
		stat				= argData.getFlagArgument(typeFlag, 0, type_name);
		CHK_STAT_R			(stat);
		MPlug type_att		= depNodeBody.findPlug(collision_node::dispType, true, &stat);
		CHK_STAT_R			(stat);
		MFnEnumAttribute	fnAttr(type_att.attribute(), &stat);
		CHK_STAT_R			(stat);
		short type_value	= fnAttr.fieldIndex(type_name, &stat);
		CHK_STAT_R			(stat);
		if(stat)
			stat			= type_att.setValue(type_value);

		if(argData.isFlagSet(autoFlag))
		{
			MString				joint_name;
			stat				= argData.getFlagArgument(autoFlag, 0, joint_name);
			MDagPath joint_path;
			stat				= get_path_by_name( joint_name, joint_path, true );

			if( !stat )
				display_error( joint_name  + " not found" );

			CHK_STAT_R			(stat);
			MFnIkJoint			jointFn(joint_path.node(), &stat);
			CHK_STAT_R			(stat);
			MObject o_;
			stat = get_skin_cluster(jointFn, o_);
			MFnSkinCluster skin_cluster(o_);
			MSelectionList		points_sel_list;
			MDoubleArray		weights_list;
			stat				= skin_cluster.getPointsAffectedByInfluence(joint_path, points_sel_list, weights_list);
			u32 curr_joint_idx	= skin_cluster.indexForInfluenceObject(joint_path, &stat);
			CHK_STAT_R			(stat);

			MDagPath			component_path; //kMesh
			MObject				mesh_object;
			stat				= points_sel_list.getDagPath(0, component_path, mesh_object);
			CHK_STAT_R			(stat);

			u32					infl_count = 0;
			stat				= skin_cluster.getWeights(component_path, mesh_object, weights_list, infl_count);
			CHK_STAT_R			(stat);

			MItGeometry itPoints	(component_path, mesh_object);
			xray::maya::vector< Wm4::Vector3<float> > verts;
			u32 point_idx = 0;
			while(!itPoints.isDone())
			{
				if( is_max_weight(weights_list, point_idx, curr_joint_idx, infl_count) )
				{
					MPoint p	= itPoints.position(MSpace::kWorld, &stat);
					CHK_STAT_R	(stat);
					verts.push_back( Wm4::Vector3<float>( float( p.x ), float( p.y ), float( p.z ) ) );
				}
				itPoints.next();
				++point_idx;
			}

			auto_calculate_shape(verts, type_value, object);
			//calc_mesh_type...
		}
	}
	return					stat;
}


MStatus createCollisionCmd::undoIt()
{
	return dagMod.undoIt();
}

MStatus createCollisionCmd::redoIt()
{
	return dagMod.doIt();
}

MMatrix box_rotation ( const Wm4::Box3<float> &box )
{
	MMatrix mrotate; mrotate.setToIdentity();
	for( u32 i = 0; i < 3 ; ++i )
		for( u32 j = 0; j < 3 ; ++j )
			mrotate[i][j] = box.Axis[i][j];
	return mrotate;
}

MVector box_scale ( const Wm4::Box3<float> &box )
{
	double unit = 1;///MDistance::internalToUI(1);//1./ MDistance::uiToInternal(1);
	return MVector(
			2. * box.Extent[0]*unit,
			2. * box.Extent[1]*unit,
			2. * box.Extent[2]*unit
		);
}



MMatrix set_box_matrix( const Wm4::Box3<float> &box )
{
	const MMatrix mrotate = box_rotation( box );
	MMatrix mscale; mscale.setToIdentity();

	const MVector vscale = box_scale( box );
	mscale[0][0] = vscale.x;
	mscale[1][1] = vscale.y; 
	mscale[2][2] = vscale.z;

	MMatrix mpos; mpos.setToIdentity();
	mpos[3][0] = box.Center[0]; 
	mpos[3][1] = box.Center[1]; 
	mpos[3][2] = box.Center[2]; 
	MMatrix matrix =  mscale*mrotate*mpos;
	return matrix;
}

MMatrix set_cylinder_matrix( const Wm4::Box3<float> &box )
{
	const MVector vbox_scale = box_scale( box );
	
	u8 ax[3] = {0,1,2};
	xray::math::float_sorted<u8> ax_sorted( ax[0], ax[1], ax[2], float( vbox_scale.x ),float(  vbox_scale.y ) ,float(  vbox_scale.z ) );
	
	u8 ax_cyl[3] = { (ax_sorted.r0() + 2)%3 ,ax_sorted.r0(), (ax_sorted.r0() + 1)%3 };

	MMatrix mrotate; mrotate.setToIdentity();
	for( u32 i = 0; i < 3 ; ++i )
		for( u32 j = 0; j < 3 ; ++j )
			mrotate[i][j] = box.Axis[ ax_cyl[i] ][j];
	
	MMatrix mscale; mscale.setToIdentity();

	mscale[0][0] = vbox_scale[ax_sorted.r1()];
	mscale[1][1] = vbox_scale[ ax_cyl[1] ]; 
	mscale[2][2] = vbox_scale[ax_sorted.r1()];

	MMatrix mpos; mpos.setToIdentity();
	mpos[3][0] = box.Center[0]; 
	mpos[3][1] = box.Center[1]; 
	mpos[3][2] = box.Center[2]; 
	MMatrix matrix =  mscale*mrotate*mpos;
	return matrix;
}

MMatrix set_sphere_matrix( const Wm4::Sphere3<float> &sphere )
{
	MMatrix mscale; mscale.setToIdentity();
	double unit = 1;//MDistance::internalToUI(1);//1./ MDistance::uiToInternal(1);
	mscale[0][0] = 2.* unit * sphere.Radius;
	mscale[1][1] = 2.* unit * sphere.Radius; 
	mscale[2][2] = 2.* unit * sphere.Radius;

	MMatrix mpos; mpos.setToIdentity();
	mpos[3][0] = sphere.Center[0]; 
	mpos[3][1] = sphere.Center[1]; 
	mpos[3][2] = sphere.Center[2]; 

	MMatrix matrix =  mscale*mpos;
	return matrix;
}

MStatus createCollisionCmd::auto_calculate_shape( const xray::maya::vector< Wm4::Vector3<float> >& verts, short type, MObject& object )
{
	MStatus	stat;
	

	u32 length = verts.size();
	if( length == 0 )
		return MStatus::kFailure;
	MFnTransform			depNodeTransform(object, &stat);
	MDagPath				path;
	stat = depNodeTransform.getPath(path);
	CHK_STAT_R(stat);
	MTransformationMatrix my_inv_world = path.exclusiveMatrixInverse( &stat );
	MMatrix my_w_i = my_inv_world.asMatrix();

	MMatrix res; res.setToIdentity();

	CHK_STAT_R(stat);
	switch(type)
	{
		case collision_node::collision_box :
			{
				Wm4::Box3<float> min_box= Wm4::ContMinBox( length, &verts.front(), 0.f, Wm4::Query::QT_REAL );
				res = set_box_matrix( min_box );
				break;
			}
		case collision_node::collision_sphere :
			{
				Wm4::Sphere3<float> sphere;
				Wm4::MinSphere3<float> ( length, &verts.front(), sphere, Wm4::Query::QT_REAL );
				res = set_sphere_matrix( sphere );
				break;
			}
		case collision_node::collision_cylinder :
			{
				Wm4::Box3<float> min_box= Wm4::ContMinBox( length, &verts.front(), 0.f, Wm4::Query::QT_REAL );
				res = set_cylinder_matrix( min_box );
				break;
			}
	}

	MMatrix local_b =    res  * my_w_i;
	MTransformationMatrix t_matrix (   local_b );
	depNodeTransform.set( t_matrix );

	return stat;
}