////////////////////////////////////////////////////////////////////////////
//	Created		: 29.04.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef BODY_NODE_H_INCLUDED
#define BODY_NODE_H_INCLUDED
#include	"transform_drawable.h"
#include	"joint_node.h"

class body_node :
	public joint_node
	//public MPxTransform,//, /*MPxLocatorNode*/
	//public transform_drawable
{
typedef	joint_node	super;
public:
							body_node			();
	
	virtual void			draw				( M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status, MColor colorRGB );
    
    virtual bool			isBounded			() const;
    virtual MBoundingBox	boundingBox			() const; 
	virtual MStatus			compute				( const MPlug& plug, MDataBlock& data );
    static void*			creator				();
    static MStatus			initialize			();
	virtual	bool			first_time_computed ();
	virtual MStatus			validateAndSetValue(const MPlug&,
											const MDataHandle&,
											const MDGContext&);

    virtual MStatus         setDependentsDirty( const MPlug& plugBeingDirtied,
                                                                MPlugArray &affectedPlugs );

    static const MTypeId	typeId;
    static const MString	typeName;

    // Attributes
  
    static MObject			m_atribute_mass_center;
	static MObject			m_atribute_mass;
	static MObject			m_atribute_anim_joint;
	static MObject			m_atribute_joint;

	static MObject			m_attribute_anim_joint_parrent_invert_matrix;

	static MObject			m_attribute_self_matrix;

	static MObject			m_attribute_anim_joint_rotate;
	static MObject			m_attribute_anim_joint_orient;
	static MObject			m_attribute_anim_joint_translate;



	body_node				*get_actual_body				();
private:
	MStatus					get_mass_center					(MVector& p);
	void					get_points						(MPoint** points, unsigned short& points_count, unsigned short** indices, unsigned short& index_count ) const;
	bool					is_actual_body					( );
private:
	virtual MStatus			do_export_node					( xray::configs::lua_config_value cfg );	
	virtual bool			can_be_rigid					( ){ return true; }
public:
			MStatus			export_collision_to_actual_body	( xray::configs::lua_config_value table, body_node* ab );
private:
			bool			b_joint_orient_computed;
			bool			b_joint_translate_computed;
	
}; // class body_node





#endif // #ifndef BODY_NODE_H_INCLUDED