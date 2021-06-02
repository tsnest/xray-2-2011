////////////////////////////////////////////////////////////////////////////
//	Created		: 08.05.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "build_collision_cmd.h"

const MString build_collision_cmd::Name	( "xray_build_collision" );

const char *recurseFlag			= "-r";
const char *recurseLongFlag		= "-recurse";
const char *rootNameFlag		= "-j";
const char *rootNameLongFlag	= "-start_from";

MSyntax	build_collision_cmd::newSyntax()
{
	MSyntax	syntax;

	syntax.addFlag(recurseFlag,			recurseLongFlag);
	syntax.addFlag(rootNameFlag,		rootNameLongFlag,			MSyntax::kString);

	return syntax;
}

MStatus	build_collision_cmd::doIt(const MArgList& args)
{
	MStatus					stat;
	MArgDatabase argData	(syntax(), args, &stat);
	CHK_STAT_R				(stat);

	MString					root_name;
	stat					= argData.getFlagArgument(rootNameFlag, 0, root_name);
	CHK_STAT_R				(stat);
	
	MDagPath root_path;
	stat					= get_path_by_name( root_name, root_path, true );
	CHK_STAT_R				(stat);
	
	bool b_recursive		= argData.isFlagSet(recurseFlag);

	stat					= build(root_path, b_recursive, "");
	return					stat;
}

MStatus	build_collision_cmd::build(MDagPath const& joint_path, bool b_reciursive, MString const& parent_name)
{
	MStatus stat(MStatus::kSuccess);

	MFnDagNode	joint_node(joint_path);

	//create body
	MString body_name = joint_node.name();
	
	if( body_name == "none" ) // temporaly skip nones
	{
		display_warning( "build_collision_cmd: skip none" );
		return MStatus::kSuccess;
	}


	if(!joint_path.hasFn	(MFn::kJoint))
	{
		display_warning( "build_collision_cmd: " + body_name + " is not an kJoint class " );

		//return MStatus::kInvalidParameter;
		return MStatus::kSuccess;
	};

	//MFnIkJoint ik_joint( joint_node, &stat );
	//CHK_STAT_R( stat );
	//ik_joint.

	body_name		+= "_body";

	MString cmd		= "xray_body -n ";
	cmd				+= body_name;
	cmd				+= " -aj ";
	cmd				+= joint_node.name();

	stat			= MGlobal::executeCommand(cmd);



	//create collision
	MString	collision_name = joint_node.name();
	collision_name	+= "_c";

	cmd 			= "xray_collision -t box -a ";
	cmd				+= joint_node.name();
	cmd				+= " -n ";
	cmd				+= collision_name;

	cmd				+= " -p ";
	cmd				+= body_name;

	stat			= MGlobal::executeCommand(cmd);

	//create joint
	if( parent_name.length()>0 )
	{
		cmd 			= "xray_constraint -edit ";//-t fixed -n constraint
		cmd				+= " -link ";
		cmd				+= parent_name;
		cmd				+= " -link ";
		cmd				+= body_name;
		cmd				+= " ";
		cmd				+= body_name;
		stat			= MGlobal::executeCommand(cmd);
		
		cmd				= "parent ";
		cmd				+=body_name;
		cmd				+=" ";
		cmd				+= parent_name;
		stat			= MGlobal::executeCommand(cmd);
	}
	if(b_reciursive)
	{
		u32 count = joint_node.childCount();
		for(u32 i=0; i<count; ++i)
		{
			MObject child	= joint_node.child(i);

			//if(!child.hasFn(MFn::kJoint))
				//continue;
			if( !child.hasFn(MFn::kDependencyNode) )
				continue;

			MFnDagNode	child_node		(child, &stat);
			CHK_STAT_R				(stat);
			MDagPath				child_path;
			stat					= child_node.getPath(child_path);
			CHK_STAT_R				(stat);
			stat = build			(child_path, b_reciursive, body_name);
			CHK_STAT_R				(stat);
		}
	}

	return stat;
}