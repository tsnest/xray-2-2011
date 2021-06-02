////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRANSFORM_CONTROL_H_INCLUDED
#define TRANSFORM_CONTROL_H_INCLUDED


namespace xray{
namespace editor{
namespace detail{

class collision_object_control;
}//namespace detail

enum enum_transform_axis{
	enum_transform_axis_cam,
	enum_transform_axis_x,
	enum_transform_axis_y,
	enum_transform_axis_z,
	enum_transform_axis_all,
	enum_transform_axis_none,
	enum_transform_axis_COUNT
};

enum enum_transform_mode{
	enum_transform_mode_local,
	enum_transform_mode_control,
};



class transform_control {
public:
	virtual						~transform_control		( ) = 0 {}
	virtual		void			initialize				( ) = 0;	
	virtual		void			destroy					( ) = 0;	
	virtual		void			set_transform			( math::float4x4 const& transform ) = 0;
	virtual		void			set_view_transform		( math::float4x4  const& view ) = 0;
	virtual		void			show					( bool show ) = 0;
	virtual		void			update					( ) = 0;
	virtual		void			start_input				( ) = 0;
	virtual		void			execute_input			( ) = 0;	
	virtual		void			end_input				( ) = 0;
	virtual		void			collide					( detail::collision_object_control const* object ) = 0;	
	virtual		void			draw					( math::float4x4  const& view_matrix ) = 0;
	virtual		void			set_draw_geomtry		( bool draw ) = 0;
	virtual		void			resize					( float size ) = 0;
	virtual		math::float4x4	create_object_transform	( math::float4x4 const& object_tranform, enum_transform_mode mode ) const = 0;
	virtual		bool			get_preview_mode		( ) const = 0;
	virtual		void			set_mode_modfiers		( bool plane_mode, bool free_mode ) = 0;
	
private:

}; // class transform_control

} //namespace editor 
} //namespace xray

#endif // #ifndef TRANSFORM_CONTROL_H_INCLUDED