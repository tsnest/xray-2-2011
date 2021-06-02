////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BODY_NODE_MANIP_H_INCLUDED
#define BODY_NODE_MANIP_H_INCLUDED

class body_node_manip : public MPxManipContainer
{
public:  
    virtual MStatus createChildren			();
    virtual MStatus connectToDependNode		(const MObject & node);
    virtual void	draw					(M3dView & view, const MDagPath & path, M3dView::DisplayStyle style,M3dView::DisplayStatus status); 

    static void*	creator					();

  
	MTransformationMatrix body_node_manip::node_transformation() const;
    MVector			mass_center_translation	() const;

    static const MTypeId typeId;
    static const MString typeName;

    MManipData		center_point_callback	(unsigned index) const;
private:
    // Paths to child manipulators
    MDagPath		m_mass_center_dag_path;

    // Object that the manipulator will be operating on
    MObject			m_target_object;
};

#endif // #ifndef BODY_NODE_MANIP_H_INCLUDED