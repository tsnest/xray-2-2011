////////////////////////////////////////////////////////////////////////////
//	Created		: 29.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "collision_node.h"
#include "body_node.h"
#include "maya_engine.h"

static void attr_changed_cb(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* clientData);

const MTypeId collision_node::typeId	( collision_node_type_id );
const MString collision_node::typeName	( collision_node_type_name );

MObject collision_node::dispType;

const double M_2PI = M_PI * 2.0;

MStatus disconnect_plug(MDGModifier& dgMod, MPlug& plug, bool asSrc, bool asDst)
{
	MStatus			stat;
	MPlugArray		connected_plugs;

	if(plug.numChildren()==0)
	{
		plug.connectedTo		(connected_plugs, asSrc, asDst, &stat);	
		CHK_STAT_R			(stat);
		for(unsigned int i=0; i<connected_plugs.length(); ++i)
		{
			stat				= dgMod.disconnect	(connected_plugs[i], plug);
			CHK_STAT_R			(stat);
		}
	}else
	{
		for(unsigned int ch=0; ch<plug.numChildren(); ++ch)
		{
			MPlug p				= plug.child(ch, &stat);
			CHK_STAT_R			(stat);
			stat				= disconnect_plug(dgMod, p, asSrc, asDst);
			CHK_STAT_R			(stat);
		}
	}

	return stat;
}

static void attr_changed_cb(MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* clientData)
{
	XRAY_UNREFERENCED_PARAMETER( clientData );
	XRAY_UNREFERENCED_PARAMETER( otherPlug );

	MStatus stat;
	if(msg & MNodeMessage::kAttributeSet)
	{
		if(plug.attribute()==collision_node::dispType)
		{
			MObject node				= plug.node(&stat);		CHK_STAT_RV(stat);
			MFnDependencyNode fnDepNode	(node, &stat);			CHK_STAT_RV(stat);
			MFnDagNode fnDagNode		(node, &stat);			CHK_STAT_RV(stat);
			
			MDagPath					path;
			stat = fnDagNode.getPath	(path);					CHK_STAT_RV(stat);
			path.pop					();// to transform
			MFnDependencyNode fnDepNodeT(path.node(), &stat);	CHK_STAT_RV(stat);


			//remove all connection for scale
			MDGModifier					dgMod;
			MPlug						scale_plug;
			scale_plug					= fnDepNodeT.findPlug("scale", true, &stat);	
			CHK_STAT_RV(stat);
			disconnect_plug				(dgMod, scale_plug, true, false);
			dgMod.doIt					();

			MPlug type_plug1			= fnDepNodeT.findPlug(collision_node::dispType, true, &stat);	CHK_STAT_RV(stat);
			display_info(type_plug1.info());
			MPlug type_plug				= fnDepNode.findPlug(collision_node::dispType, true, &stat);	CHK_STAT_RV(stat);
			display_info(type_plug1.info());
			short disp_type;
			stat						= type_plug.getValue(disp_type);	CHK_STAT_RV(stat);
			MFnExpression				fnExpr;
			MString						expr_string;
			bool bcreate				= false;
			switch(disp_type)
			{
				case collision_node::collision_cylinder:
					expr_string					= fnDepNodeT.name();
					expr_string					+= ".scaleX = ";
					expr_string					+= fnDepNodeT.name();
					expr_string					+= ".scaleZ;";
					bcreate						= true;
				break;
				case collision_node::collision_sphere:
					expr_string					= fnDepNodeT.name();
					expr_string					+= ".scaleX = ";
					expr_string					+= fnDepNodeT.name();
					expr_string					+= ".scaleZ = ";
					expr_string					+= fnDepNodeT.name();
					expr_string					+= ".scaleY;";
					bcreate						= true;
				break;
			}
			if(bcreate)
			{
				MObject expr_object	= fnExpr.create(expr_string, MObject::kNullObj, &stat);
				CHK_STAT_RV(stat);
			}
		}
	}
}


void collision_node::postConstructor()
{
	MStatus					stat;
	super::postConstructor	();

	MObject o				= thisMObject();
	MCallbackId cbid		= MNodeMessage::addAttributeChangedCallback(o, (MNodeMessage::MAttr2PlugFunction)attr_changed_cb, NULL, &stat);
	CHK_STAT_RV				(stat);
	xray::maya::g_maya_engine.register_callback_id	( cbid );
}

void collision_node::draw(M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status, MColor colorRGB )
{ 
	XRAY_UNREFERENCED_PARAMETER( status );
	XRAY_UNREFERENCED_PARAMETER( style );
	XRAY_UNREFERENCED_PARAMETER( path );

	view.beginGL	(); 
	glPushAttrib	(GL_CURRENT_BIT);

	int typeValue;

	unsigned short	points_count		= 0;
	MPoint*			pts					= 0;
	unsigned short* pindices			= NULL;
	unsigned short index_count			= 0;

	get_points(&pts, points_count, &pindices, index_count, typeValue);

	MMatrix				mscale;
	
	mscale.setToIdentity();
	mscale.matrix[0][0]	= 1;//MDistance::uiToInternal(1);
	mscale.matrix[1][1]	= 1;//MDistance::uiToInternal(1);
	mscale.matrix[2][2]	= 1;//MDistance::uiToInternal(1);
	glMatrixMode		(GL_MODELVIEW);
	glPushMatrix		();
	glMultMatrixd		(&mscale.matrix[0][0]);



	glEnableClientState	(GL_VERTEX_ARRAY);
	glVertexPointer		(3, GL_DOUBLE, sizeof(MPoint), pts);
	glDrawElements		(GL_LINES, index_count, GL_UNSIGNED_SHORT, pindices);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBegin(GL_LINES);
		glVertex3f( -0.5f, 0.0f, 0.0f );
		glVertex3f( 0.5f, 0.0f, 0.0f );

		glVertex3f( 0.0f, 0.0f, -0.5f );
		glVertex3f( 0.0f, 0.0f, 0.5f );
	glEnd();

	glPopMatrix			();
	glPopAttrib();
	view.endGL();       
}

bool collision_node::isBounded() const
{ 
	return true;
}

MBoundingBox get_bounding_box(const MPoint* pts, unsigned short points_count)

{
	MBoundingBox	bbox;
	for( unsigned int i=0; i < points_count; ++i )
		bbox.expand( pts[i] );

	return bbox;
}

MBoundingBox collision_node::boundingBox() const
//
// N.B. It is important to have this bounding box function otherwise zoom selected and 
// zoom all won't work correctly.
//
{   
	unsigned short points_count		= 0;
	MPoint*	points					= NULL;
	unsigned short* indices			= NULL;
	unsigned short index_count		= 0;
	int type						= 0;
	get_points						(&points, points_count, &indices, index_count, type);

	return get_bounding_box( points, points_count );
}


void* collision_node::creator()
{
	return CRT_NEW(collision_node)();
}

MStatus collision_node::initialize()
{ 
	MStatus				stat;

	MFnEnumAttribute	enumFn;
	dispType			= enumFn.create("shapeType", "sht", 0, &stat);		CHK_STAT_R(stat);

	stat				= enumFn.addField("box", collision_box);			CHK_STAT_R(stat);
	stat				= enumFn.addField("cylinder", collision_cylinder);	CHK_STAT_R(stat);
	stat				= enumFn.addField("sphere", collision_sphere);		CHK_STAT_R(stat);

	stat				= enumFn.setKeyable(true);							CHK_STAT_R(stat);
	stat				= addAttribute(dispType);							CHK_STAT_R(stat);

	return				stat;
}

