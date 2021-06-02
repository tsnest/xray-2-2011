////////////////////////////////////////////////////////////////////////////
//	Created		: 30.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DRAG_AND_DROP_BEHAVIOR_H_INCLUDED
#define DRAG_AND_DROP_BEHAVIOR_H_INCLUDED

class drag_and_drop_behavior : public MPxDragAndDropBehavior
{
public:
	virtual bool 			shouldBeUsedFor		(MObject &sourceNode, MObject &destinationNode, MPlug &sourcePlug, MPlug &destinationPlug);
	virtual MStatus 		connectAttrToAttr	(MPlug &sourcePlug, MPlug &destinationPlug, bool force);
	virtual MStatus 		connectAttrToNode	(MPlug &sourcePlug, MObject &destinationNode, bool force);
	virtual MStatus 		connectNodeToAttr	(MObject &sourceNode, MPlug &destinationPlug, bool force);
	virtual MStatus 		connectNodeToNode	(MObject &sourceNode, MObject &destinationNode, bool force);
	static void*			creator				();
    static const MString	typeName;

}; // class drag_and_drop_behavior

#endif // #ifndef DRAG_AND_DROP_BEHAVIOR_H_INCLUDED