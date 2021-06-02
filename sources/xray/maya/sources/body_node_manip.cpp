////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "body_node_manip.h"

const MTypeId body_node_manip::typeId( body_node_manip_type_id );

//
// N.B. The name MUST have "Manip" appended to the locator/node name it will affect. 
// Without this Maya won't be able to find it.
const MString body_node_manip::typeName( body_node_manip_type_name ); 

void* body_node_manip::creator()
{
	return CRT_NEW(body_node_manip)();
}

MStatus body_node_manip::createChildren()
{
	MStatus stat = MStatus::kSuccess;

	m_mass_center_dag_path		= addFreePointTriadManip( "mass_centerManip", "mass_center");
	MFnFreePointTriadManip mass_centerFn( m_mass_center_dag_path );
	
	mass_centerFn.setGlobalTriadPlane	(MFnFreePointTriadManip::kViewPlane);

	return stat;
}

MStatus body_node_manip::connectToDependNode( const MObject &node )
{
	MStatus			stat;

	m_target_object = node;

	MFnDependencyNode nodeFn(node); 
    
	MFnFreePointTriadManip mass_centerFn( m_mass_center_dag_path );

	MPlug mass_centerPlug				= nodeFn.findPlug( "MassCenter", &stat );
	stat = mass_centerFn.connectToPointPlug	(mass_centerPlug);
	mass_centerFn.set					(node_transformation());

	// Finish up by calling the base classes connectToDependNode method
	// This function must be called only once
	stat								= finishAddingManips();
	MPxManipContainer::connectToDependNode(node);
	return stat;
}

void body_node_manip::draw( M3dView &view, const MDagPath &path, 
												M3dView::DisplayStyle style, M3dView::DisplayStatus status )
//
// Do any custom drawing here. In this case I just display text labels for 
// the manipulators.
//
{ 
	// This must be called before any other drawing
	MPxManipContainer::draw(view, path, style, status);

	view.beginGL(); 
	glPushAttrib( GL_CURRENT_BIT );
 //   
	MVector trans = mass_center_translation();
	view.drawText("mass center", trans, M3dView::kLeft);

	glPopAttrib();
	view.endGL();
}

MTransformationMatrix body_node_manip::node_transformation() const
//
// Returns a vector that contains the node's world position
//
{
	MDagPath			path;
	MFnDagNode dagFn	(m_target_object);
	dagFn.getPath		(path);
	path.pop			();

	MFnTransform transformFn(path);

	return transformFn.transformation();
}

MVector body_node_manip::mass_center_translation() const
//
// Returns a vector that contains the node's world position
//
{
	MFnDagNode dagFn	(m_target_object);


	MTransformationMatrix m = node_transformation();

	MPlug mass_center_plug	= dagFn.findPlug( "MassCenter" );
	MVector					offset;
	mass_center_plug.child(0).getValue(offset.x);
	mass_center_plug.child(1).getValue(offset.y);
	mass_center_plug.child(2).getValue(offset.z);
	
	m.addTranslation(offset, MSpace::kObject);

	return m.getTranslation(MSpace::kWorld);
}
