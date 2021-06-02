////////////////////////////////////////////////////////////////////////////
//	Created		: 12.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "create_joint_cmd.h"
#include "joint_node.h"
#include "body_node.h"
#include "fake_drawing_locator.h"

const MString createJointCmd::Name	( "xray_constraint" );

static const char *nameFlag	= "-n",	*nameLongFlag	= "-name";
static const char *typeFlag	= "-t",	*typeLongFlag	= "-type";
static const char *linkFlag	= "-l",	*linkLongFlag	= "-link";

MSyntax createJointCmd::newSyntax()
{
    MSyntax syntax;
	syntax.addFlag(nameFlag,			nameLongFlag,			MSyntax::kString);
	syntax.addFlag(typeFlag,			typeLongFlag,			MSyntax::kString);
	syntax.addFlag(linkFlag,			linkLongFlag,			MSyntax::kString);
	syntax.makeFlagMultiUse(linkFlag);
	//syntax.setMaxObjects( 1 );
	//syntax.setMinObjects( 1 );
	syntax.enableEdit(true);
	syntax.setObjectType( MSyntax::kSelectionList, 0, 1 );
	syntax.useSelectionAsDefault(true );
	
	return syntax;
}


MStatus	doLinkIt(MArgDatabase &argData, MDagModifier &dagMod, MFnTransform	&dagNodeTransform)
{
	MArgList al;
	MStatus	 stat;

	MDagPath				path;
	stat					= dagNodeTransform.getPath(path);
	CHK_STAT_R				(stat);
	MFnDependencyNode		depNodeJoint(path.node(), &stat);
	CHK_STAT_R				(stat);

	if(argData.numberOfFlagUses(linkFlag)>0)
	{
		stat					= argData.getFlagArgumentList( linkFlag, 0, al ); 
		CHK_STAT_R				(stat);

		MString	link0			= al.asString(0, &stat);
		CHK_STAT_R				(stat);
		MPlug plug_link0		= depNodeJoint.findPlug(joint_node::body0, true, &stat);
		CHK_STAT_R				(stat);
		MDagPath link0_path;	
		stat					= get_path_by_name( link0, link0_path, true );
		//link0_path.extendToShape();
		CHK_STAT_R				(stat);

		MFnDependencyNode		depNodeLink0Joint(link0_path.node(), &stat);
		CHK_STAT_R				(stat);
		MPlug p1				= depNodeLink0Joint.findPlug(body_node::m_atribute_joint);
		MPlug p2				= depNodeJoint.findPlug(joint_node::body0);
		display_info( p1.name() );
		display_info( p2.name() );
		stat					= dagMod.connect(p1, p2);
		CHK_STAT_R				(stat);

		//dagNodeTransform.set	(link0_path.inclusiveMatrix(&stat));	CHK_STAT_R(stat);
	}

	if(argData.numberOfFlagUses(linkFlag)==2)
	{
		stat				= argData.getFlagArgumentList(linkFlag, 1, al); 
		CHK_STAT_R			(stat);
		MString	link1		= al.asString(0, &stat);
		CHK_STAT_R			(stat);
		MDagPath link1_path;
		stat				= get_path_by_name( link1, link1_path, true );
		//link1_path.extendToShape();
		CHK_STAT_R			(stat);


		//stat				= dagMod.connect(link1_path.node(), body_node::m_atribute_joint, object, joint_node::body1);
		//CHK_STAT_R			(stat);

		MFnDependencyNode	depNodeLink1Joint(link1_path.node(), &stat);
		CHK_STAT_R			(stat);

		MPlug p1			= depNodeLink1Joint.findPlug(body_node::m_atribute_joint);
		MPlug p2			= depNodeJoint.findPlug(joint_node::body1);
		//p1.name
		display_info( p1.name() );
		display_info( p2.name() );
		stat				= dagMod.connect(p1, p2);
		CHK_STAT_R			(stat);

		//dagNodeTransform.set	(link1_path.inclusiveMatrix(&stat));	CHK_STAT_R(stat);
	}
	return stat;
}


MStatus	createJointCmd::doIt ( const MArgList &args )
{
	MStatus					stat;
	MArgDatabase argData	(syntax(), args);
	
	bool bEdit				= argData.isEdit();
	MObject object;

	if(!bEdit)
	{
		object			= dagMod.createNode(joint_node::typeId, MObject::kNullObj, &stat);

		CHK_STAT_R				(stat);
		stat					= dagMod.doIt();
		CHK_STAT_R				(stat);

		MObject constraintLocatorObj = dagMod.createNode(fake_drawing_locator::typeId, object, &stat);
		dagMod.doIt();
		CHK_STAT_R(stat);
	



	} else
	{
		MSelectionList selection;
	
		argData.getObjects( selection );
		u32 length = selection.length( &stat );
		R_ASSERT_U( length==1, "must pass one constraint" );
		CHK_STAT_R	( selection.getDependNode( 0, object ) );
	}

	MFnTransform			dagNodeTransform(object, &stat);
	CHK_STAT_R				(stat);
	MDagPath				path;
	stat					= dagNodeTransform.getPath(path);
	CHK_STAT_R				(stat);
	MFnDependencyNode		depNodeJoint(path.node(), &stat);
	CHK_STAT_R				(stat);

	if(!bEdit)
	{
		MPlug type_att			= depNodeJoint.findPlug(joint_node::jointType, false, &stat);
		CHK_STAT_R				(stat);
		CHK_STAT_R				( type_att.setShort( joint_node::joint_rigid ) );
	}

	if(argData.isFlagSet(nameFlag))
	{
		MString				object_name;
		stat				= argData.getFlagArgument(nameFlag, 0, object_name);
		CHK_STAT_R			(stat);
		dagNodeTransform.setName(object_name, &stat);
		CHK_STAT_R			(stat);
		depNodeJoint.setName(MString(object_name+"Shape"), &stat);
		CHK_STAT_R			(stat);
	}

	if(argData.isFlagSet(typeFlag))
	{
		MString				type_name;
		stat				= argData.getFlagArgument(typeFlag, 0, type_name);
		CHK_STAT_R			(stat);
		MPlug type_plug		= depNodeJoint.findPlug(joint_node::jointType, true, &stat);
		CHK_STAT_R			(stat);
		MFnEnumAttribute	fnAttr(joint_node::jointType, &stat);
		CHK_STAT_R			(stat);
		short type_value	= fnAttr.fieldIndex(type_name, &stat);
		CHK_STAT_R			(stat);
		type_plug.setValue	(type_value);
	}

	CHK_STAT_R	( doLinkIt( argData, dagMod, dagNodeTransform ) );
	return redoIt			();
}

MStatus createJointCmd::undoIt()
{
	return dagMod.undoIt();
}
MStatus createJointCmd::redoIt()
{
		return dagMod.doIt();
}


