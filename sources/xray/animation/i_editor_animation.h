////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2010
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_I_EDITOR_ANIMATION_H_INCLUDED
#define XRAY_ANIMATION_I_EDITOR_ANIMATION_H_INCLUDED

#ifdef MASTER_GOLD
#	error do not include this header in MASTER_GOLD!
#endif // #ifdef MASTER_GOLD

namespace xray {
namespace animation {

struct frame;

struct XRAY_NOVTABLE i_editor_animation {
	virtual	u32							bones_count			( ) const = 0;
	virtual	void						evaluate			(
											u32 bone,
											u32 channel,
											float time,
											float& point,
											u32& domain
										) const = 0;
	virtual	void						evaluate_frame		( pcstr bone, float time, frame& f ) const = 0;
	virtual	pcstr						bone_name			( u32 index) const = 0;
	virtual	u32							bone_index			( pcstr bone_name ) const = 0;
	virtual	void						save_config_value	( pcstr path ) const = 0;
	virtual	configs::lua_config_value	config_value		( ) const = 0;
	virtual	skeleton_animation_ptr		animation			( ) const = 0;
	virtual	float						length				( ) const = 0;
	virtual	animation_types_enum		animation_type		( ) const = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( i_editor_animation )
}; // class i_editor_animation

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_I_EDITOR_ANIMATION_H_INCLUDED