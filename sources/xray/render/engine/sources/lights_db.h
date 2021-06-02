////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef LIGHTS_DB_H_INCLUDED
#define LIGHTS_DB_H_INCLUDED

#include <xray/render/facade/light_props.h>

namespace xray {

namespace collision {
	struct space_partitioning_tree;
} // namespace collision

namespace render {

class light;
class resource_intrusive_base;
typedef intrusive_ptr<light, resource_intrusive_base, threading::single_threading_policy> ref_light;

struct light_data {
	inline			light_data ( u32 const id ) :
		id		( id )
	{
	}

	inline	bool	operator < ( light_data const& other ) const 
	{
		return	id < other.id;
	}

	inline	bool	operator == ( u32 const other_light_id ) const 
	{
		return	id == other_light_id;
	}

	render::light_props	properties;
	ref_light			light;
	u32					id;
}; // struct light_data

class lights_db {
public:
	typedef	render::vector< light_data >	lights_type;

public:
						lights_db		( );
						~lights_db		( );
			void		initialize_sun	( light_data& light_to_add);
			void		add_light		( u32 id, render::light_props const& props);
			void		update_light	( u32 id, render::light_props const& props);
			void		remove_light	( u32 id);

public:
	inline	ref_light	get_sun			( ) const { return m_sun; }
	inline	lights_type const& get_lights ( ) const { return m_lights; }
	inline	collision::space_partitioning_tree& lights_tree ( ) const { return *m_lights_tree; }

private:
	enum tree_operation_enum {
		tree_operation_add,
		tree_operation_no_operation,
	}; // enum tree_operation_enum
			light*		create			( tree_operation_enum const add_to_tree ) const;

private:
	lights_type							m_lights;
	ref_light							m_sun;
	collision::space_partitioning_tree*	m_lights_tree;
}; // class lights_db

} // namespace render 
} // namespace xray 


#endif // #ifndef LIGHTSDB_H_INCLUDED