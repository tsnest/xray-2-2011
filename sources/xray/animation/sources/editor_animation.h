////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_ANIMATION_EDITOR_ANIMATION_H_INCLUDED
#define XRAY_ANIMATION_EDITOR_ANIMATION_H_INCLUDED

#ifdef MASTER_GOLD
#	error do not include this header in MASTER_GOLD!
#endif // #ifdef MASTER_GOLD

#include <xray/animation/i_editor_animation.h>
#include <xray/animation/bone_animation.h>
#include <xray/animation/cubic_spline_skeleton_animation.h>

namespace xray {
namespace animation {

class	skeleton_animation;
struct	frame;

class editor_animation : public i_editor_animation {
public:
										editor_animation	( skeleton_animation_ptr animation );
										editor_animation	( pcstr file_name );
	virtual u32							bones_count			( ) const;
	virtual void						evaluate			( u32 bone, u32 ch, float time, float& point, u32& domain ) const;
	virtual	void						evaluate_frame		( pcstr bone,  float time, frame &f ) const;
	virtual pcstr						bone_name			( u32 index ) const;
	virtual u32							bone_index			( pcstr bone_name ) const;
	virtual configs::lua_config_value	config_value		( ) const;
	virtual void						save_config_value	( pcstr path ) const;
	virtual skeleton_animation_ptr		animation			( ) const { return m_animation; }
	virtual float						length				( ) const;
	virtual animation_types_enum		animation_type		( ) const { return cubic_spline_skeleton_animation_pinned( m_animation )->animation_type(); }
			bool						is_exist			( ) const { return !m_load_failed; }

private:
			void						on_animation_loaded	( resources::queries_result& resource );

private:
	 skeleton_animation_ptr				m_animation;
	 mutable current_frame_position		m_last_frame_position;
	 bool								m_load_failed;
}; // class editor_animation

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_EDITOR_ANIMATION_H_INCLUDED