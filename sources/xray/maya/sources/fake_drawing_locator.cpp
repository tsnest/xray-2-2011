////////////////////////////////////////////////////////////////////////////
//	Created		: 18.06.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fake_drawing_locator.h"
#include "transform_drawable.h"

const MTypeId fake_drawing_locator::typeId	( fake_drawing_locator_type_id );
const MString fake_drawing_locator::typeName( fake_drawing_locator_type_name );

MObject fake_drawing_locator::aEnabled;

fake_drawing_locator::fake_drawing_locator()
{
}

fake_drawing_locator::~fake_drawing_locator()
{
}

MStatus fake_drawing_locator::compute(const MPlug& plug, MDataBlock& data)
{ 
	XRAY_UNREFERENCED_PARAMETER( plug );
	XRAY_UNREFERENCED_PARAMETER( data );
	return MS::kUnknownParameter;
}

void fake_drawing_locator::draw(M3dView& view, const MDagPath& dagPath,
							   M3dView::DisplayStyle style, M3dView::DisplayStatus displayStatus)
{ 
	//displayInfo("in fake_drawing_locator::draw");

	MStatus status;

	// Check the parent node. If it's a NxRigidConstraintNode instance,
	// call its draw method.
	MFnTransform parentTransform(dagPath, &status);
	CHK_STAT_RV(status);
	//if (parentTransform.typeId() == body_node::typeId)
	//{
	//	//displayInfo("Found parent NxRigidConstraintNode.");
	//	
	//	body_node* pConstraintNode = 
	//		(body_node*) parentTransform.userNode(&status);

	//	if (pConstraintNode)
	//		pConstraintNode->draw(view, dagPath, style, displayStatus);
	//}

	transform_drawable *to_draw = dynamic_cast<transform_drawable *> (  parentTransform.userNode(&status) );
	if( to_draw )
		to_draw->draw(view, dagPath, style, displayStatus, colorRGB(displayStatus));
	else
		R_ASSERT_U( false, "Wrong parent type." );
		//display_info("Wrong parent type.");
	
}

void* fake_drawing_locator::creator()
{
	return CRT_NEW(fake_drawing_locator)();
}

MStatus fake_drawing_locator::initialize()
{ 
	return MS::kSuccess;
}