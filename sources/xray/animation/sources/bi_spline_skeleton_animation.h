////////////////////////////////////////////////////////////////////////////
//	Created		: 19.01.2010
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BI_SPLINE_SKELETON_ANIMATION_H_INCLUDED
#define BI_SPLINE_SKELETON_ANIMATION_H_INCLUDED

#include "bi_spline_bone_animation.h"
#include <xray/animation/bone_names.h>
#include <xray/animation/animation_event_channels.h>
#include <xray/fs_path_string.h>
#include <xray/animation/type_definitions.h>

namespace xray {
namespace animation {

class skeleton_animation;
class bone_names;
class skeleton;

class bi_spline_skeleton_animation : public resources::unmanaged_resource {
public:
										bi_spline_skeleton_animation	( );
										~bi_spline_skeleton_animation	( );
			void						set_num_bones					( u32 size );
			void						set_bone_names_num_bones		( u32 num );
			void						set_bone_name					( bone_index_type bone_index, pcstr name );
			void						set_bone_names					( const animation::bone_names &bn );

public:
			void						write							( stream &file ) const;
#ifndef MASTER_GOLD
			void						save							( configs::lua_config_value cfg )const;
#endif // #ifndef MASTER_GOLD
			void						load							( configs::binary_config_value const &cfg );
			void						create_event_channels			( configs::binary_config_value const &cfg );
			bool						compare							( bi_spline_skeleton_animation const & skel )const;
			void						skeleton_bone_index				( skeleton const& skeleton, vector< u32 >& indices ) const;

public:
	inline	bi_spline_bone_animation&	bone							( u32 id )			{ return m_bone_data[ id ]; }
	inline	bi_spline_bone_animation const&	bone						( u32 id ) const	{ return m_bone_data[ id ]; } 
	inline	u32							bones_count						( ) const			{ return m_bone_data.size(); }
	inline	animation::bone_names const& bone_names						( ) const			{ return *m_bone_names; }
	inline	animation_event_channels const*	event_channels				( ) const			{ return m_event_channels; }
	inline	animation_types_enum		animation_type					( ) const			{ return m_animation_type; }

private:
	vector< bi_spline_bone_animation >	m_bone_data;
	animation_event_channels*			m_event_channels;
	pvoid								m_event_channels_memory;
	animation::bone_names*				m_bone_names;
	pvoid								m_bone_names_memory;
	animation_types_enum				m_animation_type;
}; // class bi_spline_skeleton_animation

} // namespace animation
} // namespace xray

#endif // #ifndef BI_SPLINE_SKELETON_ANIMATION_H_INCLUDED