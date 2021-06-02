////////////////////////////////////////////////////////////////////////////
//	Created		: 21.05.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "physics_exporter.h"
#include <xray/configs.h>
#include "joint_node.h"
#include "body_node.h"
#include "collision_node.h"

using xray::configs::lua_config_value;

MStatus export_body_node		(lua_config_value table, MFnTransform& node_tr, MFnDagNode& node);
MStatus export_joint_node		(lua_config_value table, MFnTransform& node_tr, MFnDagNode& node);
MStatus export_collision_node	(lua_config_value table, MFnTransform& node_tr, MFnDagNode& node);
MStatus export_collision_node	(lua_config_value table, MFnTransform& node_tr, MFnDagNode& node, const MMatrix &dispacement );
MStatus export_transform		(lua_config_value table, MFnTransform& node_tr);
MStatus export_freedom_degree(lua_config_value table, atribute_freedom_degree& att, MFnDependencyNode& node, pcstr name);
MStatus export_transform(lua_config_value table, MFnTransform& node_tr, const MMatrix &dispacement );

void* physics_exporter::creator()
{
	return CRT_NEW(physics_exporter)();
}


/*
MStatus physics_exporter::writer(const MFileObject &file, const MString &optionsString, FileAccessMode mode)
{
	MStatus		stat;
	xray::configs::lua_config_ptr cfg		= xray::configs::create_lua_config();

	lua_config_value bodies_table	= (*cfg)["bodies"];
	lua_config_value joints_table	= (*cfg)["joints"];
	u32 bodies_counter				= 0;
	u32 joints_counter				= 0;
	MItDag			iter;
	while(!iter.isDone())
	{
		MDagPath			curr_path;
		stat				= iter.getPath(curr_path);

		MFnDagNode dep_node(curr_path.node(), &stat);
		

		if(dep_node.typeId() == body_node::typeId)
		{
			display_info	("exporting body" + curr_path.fullPathName());
			curr_path.pop			();
			MFnTransform dep_node_tr	(curr_path.node(), &stat);
			export_body_node		(bodies_table[bodies_counter++], dep_node_tr, dep_node);
		}else
		if(dep_node.typeId() == joint_node::typeId)
		{
			display_info	("exporting joint" + curr_path.fullPathName());
			curr_path.pop			();
			MFnTransform dep_node_tr	(curr_path.node(), &stat);
			export_joint_node		(joints_table[joints_counter++], dep_node_tr, dep_node);
		}



		iter.next					();
	}


	cfg->save_as			(file.fullName().asChar());
	return stat;
}
*/


MStatus physics_exporter::writer(const MFileObject &file, const MString &optionsString, FileAccessMode mode)
{
	XRAY_UNREFERENCED_PARAMETER( optionsString );
	XRAY_UNREFERENCED_PARAMETER( mode );
	MStatus		stat;
	xray::configs::lua_config_ptr cfg		= xray::configs::create_lua_config();
	


	MItDag			iter;
	while(!iter.isDone())
	{
		MDagPath			curr_path;
		stat				= iter.getPath(curr_path);	CHK_STAT_R(stat);
		MFnDagNode dep_node(curr_path.node(), &stat);	CHK_STAT_R(stat);
		node_exportable *exportable = dynamic_cast<node_exportable*>( dep_node.userNode() );
		if( exportable )
			exportable->export_node( *cfg );
		iter.next					();
	}
	cfg->save_as			(file.fullName().asChar(), xray::configs::target_sources);
	return stat;
}

MStatus  export_body_collision( lua_config_value table, MFnTransform& node_tr, MFnDagNode& /*node*/, const MMatrix &dispacement  )
{
	//XRAY_UNREFERENCED_PARAMETER( node );
	MStatus		stat;
	lua_config_value collision_table = table["collision"];
	u32 childs_count		= node_tr.childCount(&stat);
	u32 collision_counter	= collision_table.size();

	for(u32 i=0; i<childs_count; ++i)
	{
		MObject o			= node_tr.child(i, &stat);
		MDagPath			pth;
		stat				= MDagPath::getAPathTo(o, pth);
		//pth.extendToShape	();
		MFnDagNode			dep_node(pth);

		if(dep_node.typeId()==collision_node::typeId)
		{

			//pth.pop					();
			MFnTransform dep_node_tr	(pth.node(), &stat);
			export_collision_node	( collision_table[collision_counter++], dep_node_tr, dep_node, dispacement );
		}
	}
	return stat; 
}

MStatus export_body_node(lua_config_value table, MFnTransform& node_tr, MFnDagNode& node)
{
	MStatus stat;
	
	MDagPath path; stat = node_tr.getPath( path );

	export_transform	(table, node_tr, path.exclusiveMatrix(&stat));
	table["name"]		= node.name().asChar();
	MPlug plug			= node.findPlug(body_node::m_atribute_mass, true, &stat);
	float				float_val;
	stat				= plug.getValue(float_val);
	table["mass"]		= float_val;

	xray::math::float3	float3_val;
	plug				= node.findPlug(body_node::m_atribute_mass_center, true, &stat);
	stat				= plug.child(0).getValue(float3_val.x);
	stat				= plug.child(1).getValue(float3_val.y);
	stat				= plug.child(2).getValue(float3_val.z);

	translation_maya_to_xray( float3_val );

	table["mass_center"]= float3_val;

	MString				string_val;
	plug				= node.findPlug(body_node::m_atribute_anim_joint, true, &stat);
	stat				= plug.getValue(string_val);
	table["anim_joint"]	= string_val.asChar();
	
	//export attached collision if exist


	stat = export_body_collision( table, node_tr, node, MMatrix() );

	return stat;
}

MStatus	joint_node::export_joint	( lua_config_value table )
{
	
	MStatus stat;
	MFnDagNode node( thisMObject(), &stat ); CHK_STAT_R	( stat );
	MFnTransform transform_node( thisMObject(), &stat  ); CHK_STAT_R	( stat );
	MDagPath path; stat = node.getPath( path );CHK_STAT_R	( stat );

	stat = export_transform	(table, transform_node, path.exclusiveMatrix( &stat ) ); CHK_STAT_R	( stat );

	table["name"]		= node.name().asChar();

	stat = export_freedom_degree(table, joint_node::x_translate, node, "x_translate");
	stat = export_freedom_degree(table, joint_node::y_translate, node, "y_translate");
	stat = export_freedom_degree(table, joint_node::z_translate, node, "z_translate");
	stat = export_freedom_degree(table, joint_node::x_rotate, node, "x_rotate");
	stat = export_freedom_degree(table, joint_node::y_rotate, node, "y_rotate");
	stat = export_freedom_degree(table, joint_node::z_rotate, node, "z_rotate");

	body_node *bd0  = actual_body( b0 ); 
	body_node *bd1  = actual_body( b1 );

	if(bd0)
		table["body0"] = bd0->name().asChar();
	if(bd0)
		table["body1"] = bd1->name().asChar();
	/*
	for(u32 i=0; i<2; ++i)
	{
		pcstr name_str			= (i==0)?"body0" : "body1";
		MPlug body_plug			= node.findPlug( name_str, true, &stat);
		CHK_STAT_R				(stat);
		MPlugArray				arr;
		body_plug.connectedTo	(arr, true, true, &stat);
		CHK_STAT_R				(stat);
		if(arr.length())
		{
			MFnDependencyNode	n(arr[0].node());
			body_node *bn		= dynamic_cast<body_node*>(n.userNode());
			ASSERT( bn );

			MString body_name	= n.name(&stat);
			table[name_str]		= body_name.asChar();
		}
	}
	*/
	MPlug plug					= node.findPlug( joint_node::jointType, true, &stat);
	MFnEnumAttribute			att(joint_node::jointType);
	short						val;
	stat						= plug.getValue(val);
	MString str_val				= att.fieldName(val, &stat);
	table["type"]				= str_val.asChar();
	

	return stat;
}





MStatus export_collision_node(lua_config_value table, MFnTransform& node_tr, MFnDagNode& node, const MMatrix &dispacement )
{
	MStatus				stat;
	export_transform	(table, node_tr, dispacement );

	MPlug plug			= node.findPlug(collision_node::dispType, true, &stat);
	MFnEnumAttribute	att(collision_node::dispType);
	short				val;
	stat				= plug.getValue(val);
	MString str_val		= att.fieldName(val, &stat);
	table["type"]		= str_val.asChar();
	return				stat;
}

MStatus export_collision_node(lua_config_value table, MFnTransform& node_tr, MFnDagNode& node)
{
	return export_collision_node( table, node_tr, node, MMatrix() );
}






MStatus export_transform(lua_config_value table, MFnTransform& node_tr, const MMatrix &dispacement )
{
	MStatus				stat;
	xray::math::float3	p;

	MTransformationMatrix  m (  node_tr.transformation().asMatrix() * dispacement );//  );
	
	//m.rotation
	
	MVector v			= m.getTranslation(MSpace::kTransform, &stat); // * MDistance::internalToUI(1);

	p.x 				= (float)v.x;
	p.y 				= (float)v.y;
	p.z 				= (float)v.z;

	//MVector tv			= node_tr.getTranslation(MSpace::kTransform, &stat);
	translation_maya_to_xray( p );
	table["position"]	= p;
	
	//MEulerRotation		rot;
	MTransformationMatrix::RotationOrder ro;
	double rot[3];
	stat				= m.getRotation( rot, ro );

	//MEulerRotation	test_r;
	//node_tr.getRotation( test_r );

	p.x 				= (float)rot[0];
	p.y 				= (float)rot[1];
	p.z 				= (float)rot[2];

	rotation_maya_to_xray( p );
	table["rotation"]	= p;

	double vd[3];
	stat				= node_tr.getScale( vd );
	p.x 				= (float)vd[0];
	p.y 				= (float)vd[1];
	p.z 				= (float)vd[2];

	size_maya_to_xray	( p );
	table["scale"]		= p;
	
	return				stat;
}

MStatus export_transform(lua_config_value table, MFnTransform& node_tr)
{
	return export_transform( table, node_tr, MMatrix() );
}
/*
#define PI_MUL2 (3.1415926535898f*2.0f)
void test()
{
	FILE* f = fopen("c:\\test_rot.txt", "wt");
	//Fvector& t, Fvector& r
	float _d = 0.3f;
	double __rot[3];
	for(__rot[0]=0; __rot[0]<PI_MUL2; __rot[0]+=_d)
		for(__rot[1]=0; __rot[1]<PI_MUL2; __rot[1]+=_d)
			for(__rot[2]=0; __rot[2]<PI_MUL2; __rot[2]+=_d)
	{
		MTransformationMatrix	mat;
		mat.setRotation			(__rot, MTransformationMatrix::kXYZ);
		MTransformationMatrix::RotationOrder ro		= MTransformationMatrix::kZXY;
		mat.reorderRotation		(ro);
		MEulerRotation rot		= mat.eulerRotation();

		fprintf( f, "%g %g %g %g %g %g \n", __rot[0], __rot[1], __rot[2], rot.x, rot.y, rot.z);
			
	}
	fclose(f);
}
*/