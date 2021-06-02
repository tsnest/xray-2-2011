////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TERRAIN_CONTROL_CURSOR_H_INCLUDED
#define TERRAIN_CONTROL_CURSOR_H_INCLUDED

#include <xray/render/world.h>
#include <xray/render/facade/model.h>

namespace xray {
namespace editor {

ref class terrain_core;
ref class terrain_node;
ref class terrain_control_base;
ref class terrain_modifier_control_base;

//tmp ???
struct vert_struct
{
	float3	position;
	float2	uv;
};

public ref class terrain_control_cursor abstract
{
public:
						terrain_control_cursor( terrain_control_base^ o, render::scene_ptr const& scene );
		virtual			~terrain_control_cursor( );

		virtual void	show				( bool show )	= 0;
		virtual void	update				( )				= 0;
				void	pin					( bool bpin )	{m_pinned=bpin;}

		void		update_ring				( terrain_core^ terrain, 
											float3 const& picked_position_global,  
											float radius, 
											float width, 
											u16 segments_count, 
											vector<vert_struct>& vertices);

		float3*		m_last_picked_position;
protected:
	terrain_control_base^			m_owner;
	render::scene_ptr*				m_scene;
	bool							m_pinned;
};

public ref class terrain_painter_cursor : terrain_control_cursor
{
	typedef terrain_control_cursor		super;
public:
					terrain_painter_cursor	( terrain_modifier_control_base^ o, render::scene_ptr const& scene );
					~terrain_painter_cursor	( );

		virtual void update					( )						override;
		virtual void	show				( bool show )			override;
protected:
		void		update					( terrain_core^ terrain_core, float3 const& picked_position_global );
		void		generate_geometry		( );
		void		on_visual_ready			( resources::queries_result& data);


	render::render_model_instance_ptr*						m_visual_inner_ring;
	render::render_model_instance_ptr*						m_visual_outer_ring;
	render::render_model_instance_ptr*						m_visual_center_point;
	render::render_model_instance_ptr*						m_visual_power;

	vector<vert_struct>*					m_inner_vertices;
	vector<vert_struct>*					m_outer_vertices;
	vector<vert_struct>*					m_center_vertices;
	vector<vert_struct>*					m_power_vertices;

	bool									m_shown;
	bool									m_visual_inner_added;
	bool									m_visual_outer_added;
	bool									m_visual_center_added;
	bool									m_visual_power_added;
	
	float									m_inner_radius;
	float									m_inner_width;
	u16										m_inner_segments;
	float									m_outer_radius;
	float									m_outer_width;
	u16										m_outer_segments;
	float									m_center_radius;
	float									m_center_width;
	u16										m_center_segments;
	System::Drawing::Point					m_last_mouse_pos;
};

public ref class terrain_selector_cursor : terrain_control_cursor
{
	typedef terrain_control_cursor		super;
public:
					terrain_selector_cursor	( terrain_control_base^ o, render::scene_ptr const& scene);
					~terrain_selector_cursor	( );

		virtual void update					( )						override;
		virtual void	show				( bool show )			override;
protected:
		void		update					( terrain_core^ terrain_core, float3 const& picked_position_global );
		void		generate_geometry		( );
		void		on_visual_ready			( resources::queries_result& data);

	render::render_model_instance_ptr*						m_visual_outer_ring;
	vector<vert_struct>*					m_outer_vertices;

	bool									m_shown;
	bool									m_visual_added;
	
	float									m_outer_radius;
	float									m_outer_width;
	u16										m_outer_segments;

	System::Drawing::Point					m_last_mouse_pos;
};

} // namespace editor
} // namespace xray

#endif // #ifndef TERRAIN_CONTROL_CURSOR_H_INCLUDED