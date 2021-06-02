////////////////////////////////////////////////////////////////////////////
//	Created		: 10.09.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_PAINTER_CONTROL_H_INCLUDED
#define OBJECT_PAINTER_CONTROL_H_INCLUDED

#include <xray/render/facade/model.h>
#include <xray/render/facade/editor_renderer.h>

namespace xray {
struct vert_static;
namespace editor {

ref class level_editor;
ref class object_painter_tool_tab;
ref class object_solid_visual;
ref class painted_object;

public ref class object_painter_control : public editor_base::editor_control_base
{
	typedef editor_base::editor_control_base	super;
public:
								object_painter_control	( level_editor^ le, object_painter_tool_tab^ tool_tab );
	virtual						~object_painter_control	( );

	virtual		void			initialize				( ) 						override;
	virtual		void			destroy					( ) 						override;
	virtual		void			activate				( bool b_activate )			override;
	virtual		void			update					( ) 						override;
	virtual		void			start_input				( int mode, int button, collision::object const* coll, math::float3 const& pos )		override;
	virtual		void			execute_input			( ) 						override;
	virtual		void			end_input				( ) 						override;
	virtual		void			draw					( render::scene_view_pointer scene, xray::render::render_output_window_pointer ) override;
	virtual		u32				acceptable_collision_type( )						override;
	virtual		void			load_settings			( RegistryKey^ key )		override;
	virtual		void			save_settings			( RegistryKey^ key )		override;
				
				void			set_source				( painted_object^ o ) {m_object=o;}
private:
		level_editor^				m_level_editor;
		object_painter_tool_tab^	m_tool_tab;
		painted_object^				m_object;
}; // class object_painter_control
 
ref class painted_object
{
public:
					painted_object			( );
					~painted_object			( );
	void			setup					( object_solid_visual^ src );
	void			reset					( );
	bool			ready					( );
	float3			get_position			( );
private:
	void			on_model_loaded			(  xray::resources::queries_result& data );
	
	object_solid_visual^	m_src_object;

	render::render_model_instance_ptr*	m_render_model_ptr;
	u32					m_vertices_count;
	vert_static*		m_vertices;
	u32*				m_paint_data;
}; //painted_object

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_PAINTER_CONTROL_H_INCLUDED