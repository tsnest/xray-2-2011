////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "create_body_cmd.h"
#include "body_node.h"
#include "fake_drawing_locator.h"

const MString createBodyCmd::Name	( "xray_body" );

static const char *nameFlag = "-n", *nameLongFlag = "-name";
static const char *animJointFlag = "-aj",	*animJointLongFlag = "-anim_joint";


static const char *linkFlag	= "-l",	*linkLongFlag	= "-link";


MSyntax createBodyCmd::newSyntax()
{
    MSyntax syntax;
	syntax.addFlag( nameFlag,			nameLongFlag,			MSyntax::kString );
	syntax.addFlag( animJointFlag,		animJointLongFlag,		MSyntax::kString);

	syntax.addFlag( linkFlag,			linkLongFlag,			MSyntax::kString);
	syntax.makeFlagMultiUse( linkFlag );


	syntax.enableEdit( true );
	syntax.setObjectType( MSyntax::kSelectionList, 0, 1 );
	syntax.useSelectionAsDefault( true );

	return syntax;
}

MStatus lock_attribute(const MFnDependencyNode& depNode, MStringArray attributes)
{
	MStatus					stat;
	for(unsigned int i=0; i<attributes.length(); ++i)
	{
		MString const& s		= attributes[i];
		MObject att				= depNode.attribute(s, &stat);
		CHK_STAT_R				(stat);
		MPlug plug				= depNode.findPlug(att, true, &stat);
		CHK_STAT_R				(stat);
		stat					= plug.setLocked(true);
		CHK_STAT_R				(stat);
	}
	return stat;
}

//MStatus	doLinkIt(MArgDatabase &argData, MDagModifier &dagMod, MFnTransform	&dagNodeTransform);
MStatus createBodyCmd::doIt( const MArgList &args )
{ 
	MStatus				stat;

	MArgDatabase argData( syntax(), args );




	bool bEdit				= argData.isEdit();
	MObject object;
	if(!bEdit)
	{
		object		= dagMod.createNode(body_node::typeId, MObject::kNullObj, &stat);
		dagMod.doIt			();
		
		CHK_STAT_R(stat);

		MObject constraintLocatorObj = dagMod.createNode(fake_drawing_locator::typeId, object, &stat);
		dagMod.doIt();
		
		MString cmd 	= "xray_constraint -edit -type rigid "; // normal
		//MString cmd 	= "xray_constraint -edit -type ball "; // to export collision per each joint
		cmd				+= MFnDagNode(object, &stat).name(); CHK_STAT_R(stat);
		stat			= MGlobal::executeCommand(cmd); CHK_STAT_R(stat);

		CHK_STAT_R(stat);
	} else
	{
		MSelectionList selection;
		argData.getObjects( selection );
		u32 length = selection.length( &stat );
		R_ASSERT_U( length==1, "must pass one constraint" );
		CHK_STAT_R	( selection.getDependNode( 0, object ) );
	}


	
	MFnDagNode			dagNode(object, &stat);	CHK_STAT_R(stat);
	MDagPath			path;
	stat				= dagNode.getPath(path);CHK_STAT_R(stat);

    

	if( argData.isFlagSet( nameFlag ) )
	{
		MString				object_name;
		stat				= argData.getFlagArgument( nameFlag, 0, object_name );
		dagNode.setName(object_name);

	}
	CHK_STAT_R( dagMod.connect( object, body_node::worldMatrix, object, body_node::m_attribute_self_matrix ) );
	CHK_STAT_R( dagMod.doIt() );

	if( argData.isFlagSet( animJointFlag ) )
	{
		MString				joint_name;
		stat				= argData.getFlagArgument( animJointFlag, 0, joint_name );
		
		MDagPath joint_path;
		stat				= get_path_by_name( joint_name, joint_path, true );
		CHK_STAT_R			(stat);
		MFnTransform		nodeTransform(object, &stat);
		CHK_STAT_R			(stat);
		nodeTransform.set	(joint_path.inclusiveMatrix(&stat));
		CHK_STAT_R			(stat);

		MPlug p				= dagNode.findPlug(body_node::m_atribute_anim_joint, &stat);
		CHK_STAT_R			(stat);

		
		MFnDagNode join_dag_node( joint_path.node(), &stat ); 
		CHK_STAT_R			(stat);

		
		
		MString			cmd 			= "connectAttr ";
		cmd				+= joint_name;
		cmd				+= ".parentInverseMatrix ";
		cmd				+= dagNode.name();
		cmd				+= ".JointParentInvertMatrix";
		CHK_STAT_R			(  MGlobal::executeCommand(cmd) );

		{
			//MPlug in		= join_dag_node.findPlug("parentInverseMatrix", &stat);CHK_STAT_R	(stat);
			//MPlug out		= dagNode.findPlug( body_node::m_attribute_anim_joint_parrent_invert_matrix, &stat);CHK_STAT_R(stat);
			//CHK_STAT_R( dagMod.connect( in, out ) );
			//CHK_STAT_R( dagMod.doIt() );
		}
		

		//cmd 			= "connectAttr ";
		//cmd				+= joint_name;
		//cmd				+= ".rotate ";
		//cmd				+= dagNode.name();
		//cmd				+= ".AnimJointRotate ";

		//CHK_STAT_R			(  MGlobal::executeCommand(cmd) );

		{
			MPlug in		= join_dag_node.findPlug("rotate", &stat);
			CHK_STAT_R		(stat);
			MPlug out		= dagNode.findPlug( body_node::m_attribute_anim_joint_rotate, &stat);
			CHK_STAT_R		(stat);
			stat			= dagMod.connect( in, out );
			CHK_STAT_R		( stat );
			stat			= dagMod.doIt();
			CHK_STAT_R		( stat );
		}

		//cmd 			= "connectAttr ";
		//cmd				+= dagNode.name();
		//cmd				+= ".AnimJointOrient ";
		//cmd				+= joint_name;
		//cmd				+= ".jointOrient";


		//CHK_STAT_R			(  MGlobal::executeCommand(cmd) );

		{
			MPlug in		= dagNode.findPlug( body_node::m_attribute_anim_joint_orient, &stat);
			CHK_STAT_R		(stat);
			MPlug out		= join_dag_node.findPlug("jointOrient", &stat);
			CHK_STAT_R		(stat);
			CHK_STAT_R		( dagMod.connect( in, out ) );
			CHK_STAT_R		( dagMod.doIt() );
		}

		//cmd 			= "connectAttr ";
		//cmd				+= dagNode.name();
		//cmd				+= ".AnimJointTranslate ";
		//cmd				+= joint_name;
		//cmd				+= ".translate";
		//CHK_STAT_R			(  MGlobal::executeCommand(cmd) );


		{
			MPlug in		= dagNode.findPlug( body_node::m_attribute_anim_joint_translate, &stat);CHK_STAT_R(stat);
			MPlug out		= join_dag_node.findPlug("translate", &stat);CHK_STAT_R	(stat);
			CHK_STAT_R( dagMod.connect( in, out ) );
			CHK_STAT_R( dagMod.doIt() );
		}
		CHK_STAT_R			( p.setString			(joint_name) );
	}
	
	


	//connectAttr joint1.parentInverseMatrix joint1_body.JointParentInvertMatrix
	//connectAttr joint1_body.AnimJointRotate joint1.rotate
	//connectAttr joint1_body.AnimJointTranslate joint1.translate
	//connectAttr joint1_body.worldMatrix joint1_body.SelfMatrix



	MStringArray			sa;
	sa.append				("scale");
	stat					= lock_attribute(dagNode, sa);
//	sa.clear				();
//	sa.append				("localPosition");
//	sa.append				("localScale");
//	stat					= lock_attribute(depNodeBody, sa);

	//MFnTransform nodeTransform( object, &stat );
	//CHK_STAT_R( stat );
	//CHK_STAT_R( doLinkIt( argData, dagMod, nodeTransform ) );
	
	return					stat;
}


MStatus createBodyCmd::undoIt()
{
	return dagMod.undoIt();
}

MStatus createBodyCmd::redoIt()
{
	return dagMod.doIt();
}

