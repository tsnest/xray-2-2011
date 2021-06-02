////////////////////////////////////////////////////////////////////////////
//	Created		: 29.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COLLISION_NODE_H_INCLUDED
#define COLLISION_NODE_H_INCLUDED
#include "transform_drawable.h"
class collision_node  : //public MPxLocatorNode
	public MPxTransform, /*MPxLocatorNode*/
	public transform_drawable
{
	typedef MPxTransform	super;
public:
	enum {collision_box, collision_cylinder, collision_sphere};
	virtual void			postConstructor();
    virtual void			draw			( M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status, MColor colorRGB );
    
    virtual bool			isBounded		() const;
    virtual MBoundingBox	boundingBox		() const; 

    static void*			creator			();
    static MStatus			initialize		();

    static const MTypeId	typeId;
    static const MString	typeName;

    // Attributes
    static MObject			dispType;

private:


    void					get_points	(MPoint** points, unsigned short& points_count, unsigned short** indices, unsigned short& index_count, int& type) const;
}; // class collision_node



void build_sphere			(MStatus& status);
void build_cylinder			(MStatus& status);
#endif // #ifndef COLLISION_NODE_H_INCLUDED