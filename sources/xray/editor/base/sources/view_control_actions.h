////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef VIEW_CONTROL_ACTIONS_H_INCLUDED
#define VIEW_CONTROL_ACTIONS_H_INCLUDED

#include "action_continuous.h"

namespace xray {
namespace editor_base {

struct pick_cache;

public ref class collision_objects
{
public:
				collision_objects		( );
				~collision_objects		( );
	u32							count	( );
	const xray::collision::object*	get		( u32 idx );
	void						add		( xray::collision::object const* );
	void						clear	( );
private:
	xray::collision::objects_type*	m_list;
};

public ref class collision_ray_objects
{
public:
				collision_ray_objects		( );
				~collision_ray_objects		( );
	u32							count	( );
	const xray::collision::ray_object_result&	get		( u32 idx );
	void						add			( xray::collision::ray_object_result& o );
	void						clear	( );
private:
	xray::collision::ray_objects_type*	m_list;
};

public ref class collision_ray_triangles
{
public:
				collision_ray_triangles		( );
				~collision_ray_triangles	( );
	u32							count	( );
	const xray::collision::ray_triangle_result&	get		( u32 idx );
	void						add		( xray::collision::ray_triangle_result& o );
	void						clear	( );
private:
	xray::collision::ray_triangles_type*	m_list;
};

ref class scene_view_panel;

public ref class view_action_base abstract : public editor_base::action_continuous
{
protected:
							view_action_base		( System::String^ name, scene_view_panel^ w );
	virtual					~view_action_base		( );

public:
	virtual bool 			capture					( ) override { return false; }
	virtual void			release					( ) override ;

protected:
			void			init_capture			( );

	scene_view_panel^		m_view_window;
	System::Drawing::Point	m_mouse_last_pos;
};

public ref class mouse_action_view_rotate_base abstract : public view_action_base
{
	typedef view_action_base		super;
public:

						mouse_action_view_rotate_base( System::String^ name, scene_view_panel^ w ):super( name, w ){};
protected:
	void				rotate_view_matrix			( math::float3 const& raw_angles, float const focus_distance );
}; // class mouse_action_view_rotate_base


public ref class mouse_action_view_move_xy : public view_action_base
{
	typedef view_action_base		super;
public:

						mouse_action_view_move_xy	( System::String^ name, scene_view_panel^ w );

	virtual bool		capture						( ) override;
	virtual bool		execute						( ) override;
	virtual void		release						( ) override;
}; // class mouse_action_view_move_xy

//////////////////////////////////////////////////////////////////////////
public ref class mouse_action_view_move_z : public view_action_base
{
	typedef view_action_base		super;
public:

						mouse_action_view_move_z	( System::String^ name, scene_view_panel^ w );

	virtual bool		capture						( ) override;
	virtual bool		execute						( ) override;
	virtual void		release						( ) override;

private:
	bool				m_continuse_drugging;
}; // class mouse_action_view_move_z

// maya rotate (alt+LMB)
public ref class mouse_action_view_rotate_around : public mouse_action_view_rotate_base
{
	typedef mouse_action_view_rotate_base	super;
public:

						mouse_action_view_rotate_around( System::String^ name, scene_view_panel^ w );

	virtual bool		capture						( ) override;
	virtual bool		execute						( ) override;
	virtual void		release						( ) override;
}; // class mouse_action_view_rotate_around


public ref class mouse_action_view_move_y_reverse : public view_action_base
{
	typedef view_action_base		super;
public:

						mouse_action_view_move_y_reverse( System::String^ name, scene_view_panel^ w );

	virtual bool		capture						( ) override;
	virtual bool		execute						( ) override;
	virtual void		release						( ) override;
}; // class mouse_action_view_move_y_reverse

public ref class mouse_action_view_move_xz_reverse : public view_action_base
{
	typedef view_action_base		super;
public:

						mouse_action_view_move_xz_reverse( System::String^ name, scene_view_panel^ w );

	virtual bool		capture						( ) override;
	virtual bool		execute						( ) override;
	virtual void		release						( ) override;
}; // class mouse_action_view_move_xz_reverse

// (LMB+RMB+Space)
public ref class mouse_action_view_rotate : public mouse_action_view_rotate_base
{
	typedef mouse_action_view_rotate_base	super;
public:

						mouse_action_view_rotate	( System::String^ name, scene_view_panel^ w );

	virtual bool		capture						( ) override;
	virtual bool		execute						( ) override;
	virtual void		release						( ) override;
}; // class mouse_action_view_rotate

public ref class camera_view_wsad : public mouse_action_view_rotate_base
{
	typedef mouse_action_view_rotate_base		super;
public:
	enum class move_action{sh_forward, sh_backward, sh_left, sh_right, rot_up, rot_down, rot_left, rot_right};

						camera_view_wsad		( System::String^ name, scene_view_panel^ w, move_action a );

	virtual bool		capture					( ) override;
	virtual bool		execute					( ) override;
	virtual void		release					( ) override;

private:
	move_action			m_action;
	timing::timer*		m_timer;
};

//public ref class mouse_action_view_roll : public view_action_base
//{
//	typedef view_action_base		super;
//public:
//
//						mouse_action_view_roll( System::String^ name, scene_view_panel^ w );
//
//	virtual bool		capture						( ) override;
//	virtual bool		execute						( ) override;
//	virtual void		release						( ) override;
//private:
//	float				m_start_angle_z;
//}; // class mouse_action_view_roll

} // namespace editor_base
} // namespace xray

#endif // #ifndef VIEW_CONTROL_ACTIONS_H_INCLUDED
