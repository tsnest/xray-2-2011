////////////////////////////////////////////////////////////////////////////
//	Created		: 07.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SKELETON_H_INCLUDED
#define SKELETON_H_INCLUDED

#include <xray/animation/type_definitions.h>

namespace xray {

namespace configs {
	class lua_config_value;
} // namespace configs

namespace animation {

class bone : private boost::noncopyable {

public:
			bone			(
				pcstr const id,
				bone const* const parent,
				bone const* const children_begin,
				bone const* const children_end,
				bone_index_type const index
			);


	inline		bone_index_type index			( )const  { return m_index; }
	inline		pcstr const		id				( )const  { return m_id; }
	inline		bone const*		parent			( )const  { return m_parent; }
	
	inline		bone const*		children_begin	( )const  { return m_children_begin; }
	inline		bone const*		children_end	( )const  { return m_children_end; }

private:
	pcstr const				m_id;
	bone const* const		m_parent;
	bone const* const		m_children_begin;
	bone const* const		m_children_end;
#ifdef DEBUG
	bone_index_type	const	m_count;
#endif // #ifdef DEBUG
	bone_index_type	const	m_index; // do we really need it?
}; // class bone

class bind_pose {
public:
	

private:

}; // class bind_pose

class skeleton : public resources::unmanaged_resource 
{
public:
							skeleton	( configs::lua_config_value const& config );
							~skeleton	( );

	inline	bone const&		get_bone	( bone_index_type idx ) const;
	inline	bone_index_type	bone_count	( ) const	{ return m_bone_count; }

	//virtual	void			recalculate_memory_usage_impl ( ) { m_memory_usage_self.unmanaged = get_size(); }

private:
	bone*					m_bones;
	bone_index_type			m_bone_count;

}; // class skeleton


bone_index_type		skeleton_bone_index( const skeleton & skel, pcstr bone_name );

} // namespace animation
} // namespace xray

#include "skeleton_inline.h"

#endif // #ifndef SKELETON_H_INCLUDED