////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SCENE_VIEW_PANEL_H_INCLUDED
#define SCENE_VIEW_PANEL_H_INCLUDED

using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#pragma managed( push, off )
#	include <xray/render/facade/model.h>
#	include <xray/render/engine/base_classes.h>
#pragma managed( pop )

namespace xray {
namespace editor_base {

	public delegate void camera_moved_delegate( void );
	public enum class enum_editor_cursors {default, cross, dragger};

	struct pick_cache;
	ref class collision_ray_objects;
	ref class collision_ray_triangles;
	ref class collision_objects;
	ref class gui_binder;
	ref class input_engine;
	ref class action;
	/// <summary>
	/// Summary for scene_view_panel
	/// </summary>

	public ref class scene_view_panel : public WeifenLuo::WinFormsUI::Docking::DockContent
	{
	public:
		scene_view_panel( )
		:m_editor_renderer	( NULL )
		{
			InitializeComponent();
			in_constructor();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~scene_view_panel()
		{
			in_destructor	( );
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Panel^				top_panel;
	private: System::Windows::Forms::ToolStripPanel^	m_top_toolstrip_panel;
	protected: 
	private: System::Windows::Forms::Panel^				view_panel;

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->top_panel = (gcnew System::Windows::Forms::Panel());
			this->view_panel = (gcnew System::Windows::Forms::Panel());
			this->SuspendLayout();
			// 
			// top_panel
			// 
			this->top_panel->BackColor = System::Drawing::Color::FromArgb(static_cast<System::Int32>(static_cast<System::Byte>(128)), static_cast<System::Int32>(static_cast<System::Byte>(64)), 
				static_cast<System::Int32>(static_cast<System::Byte>(64)));
			this->top_panel->Dock = System::Windows::Forms::DockStyle::Top;
			this->top_panel->Location = System::Drawing::Point(0, 0);
			this->top_panel->Name = L"top_panel";
			this->top_panel->Size = System::Drawing::Size(332, 30);
			this->top_panel->TabIndex = 0;
			// 
			// view_panel
			// 
			this->view_panel->BackColor = System::Drawing::Color::Cyan;
			this->view_panel->Dock = System::Windows::Forms::DockStyle::Fill;
			this->view_panel->Location = System::Drawing::Point(0, 30);
			this->view_panel->Name = L"view_panel";
			this->view_panel->Size = System::Drawing::Size(332, 304);
			this->view_panel->TabIndex = 1;
			this->view_panel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &scene_view_panel::view_panel_MouseMove);
			this->view_panel->Resize += gcnew System::EventHandler(this, &scene_view_panel::view_panel_Resize);
			// 
			// scene_view_panel
			// 
			this->AllowEndUserDocking = false;
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(332, 334);
			this->CloseButton = false;
			this->CloseButtonVisible = false;
			this->Controls->Add(this->view_panel);
			this->Controls->Add(this->top_panel);
			this->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(204)));
			this->HideOnClose = true;
			this->Name = L"scene_view_panel";
			this->Text = L"Scene View";
			this->Deactivate += gcnew System::EventHandler(this, &scene_view_panel::scene_view_panel_Leave);
			this->Activated += gcnew System::EventHandler(this, &scene_view_panel::scene_view_panel_Enter);
			this->Enter += gcnew System::EventHandler(this, &scene_view_panel::scene_view_panel_Enter);
			this->Leave += gcnew System::EventHandler(this, &scene_view_panel::scene_view_panel_Leave);
			this->ResumeLayout(false);

		}
#pragma endregion
// scene_view_control
			void					in_constructor			( );
			void					in_destructor			( );
public:
	System::Drawing::Point			get_mouse_position		( );
	void							set_mouse_position		( System::Drawing::Point );
	System::Drawing::Point			get_mouse_pos_infinite	( );
	void							set_infinite_mode		( bool );
	System::Drawing::Size			get_view_size			( );
	math::float4x4					get_inverted_view_matrix( );
	math::float4x4					get_projection_matrix	( );
	float							view_point_distance		( );
	void							set_view_point_distance	( float const );
	xray::render::debug::renderer*	debug_renderer			( );
	xray::render::editor::renderer*	editor_renderer			( );

	void							set_view_matrix			( math::float4x4 const& );
	void							setup_view_matrix		( math::float3 const& eye_point, math::float3 const& look_at );
	enum_editor_cursors				set_view_cursor			( enum_editor_cursors );


			float3					screen_to_3d			( math::float2 const& p, float const z_dist );
			float2					float3_to_screen		( math::float3 const& p );
			void					set_view_matrix			( float2 const&, float const, float const, float const );
			float3					screen_to_3d_world		( float2 const& screen, float z );
			void					get_camera_props		( math::float3& p, math::float3& d );
			void					set_camera_props		( const math::float3& p, const math::float3& d );
			bool					is_mouse_in_view		( );
			float					mouse_sensitivity		( );
			void					set_mouse_sensitivity	( float const value );
//picker
public:
	void				get_mouse_ray		( math::float3& origin, math::float3& direction );
	void				get_screen_ray		( math::int2 const& screen_xy, math::float3& origin, math::float3& direction );

	bool 				ray_query			( xray::collision::object_type const mask, xray::collision::object const** object, float3* position );
	bool 				ray_query			( xray::collision::object_type const mask, System::Drawing::Point screen_xy, xray::collision::object const** object, float3* position );
	bool 				ray_query			( xray::collision::object_type const mask, math::float3 origin, math::float3 direction, xray::collision::object const** object, float3* position );
	bool 				ray_query			( xray::collision::object_type const mask, math::float3 origin, math::float3 direction, collision_ray_objects^ collision_results );
	bool 				ray_query			( xray::collision::object_type const mask, math::float3 origin, math::float3 direction, collision_ray_triangles^ collision_results );
	bool				ray_query			( xray::collision::object_type const mask, float3& position, float3& normal );
	bool 				frustum_query		( xray::collision::object_type const mask, System::Drawing::Point min_xy, System::Drawing::Point max_xy, collision_objects^ objects );
	void 				clear_picker_cache	( );
	void				tick				( );
	void				render				( );
	System::Int32		view_handle			( );
	void				update_projection_matrix	( );
	void				window_view_Activated		( );
	void				window_view_Deactivate		( );
	void				clip_cursor					( );
	void				unclip_cursor				( );
	void				subscribe_on_camera_move	( camera_moved_delegate^ d );
	void				unsubscribe_on_camera_move	( camera_moved_delegate^ d );

	void				set_camera_effector			( xray::camera_effector* e );
	inline	float		near_plane					( ) { return m_near_plane; }
	inline	float		far_plane					( ) { return m_far_plane; }
	inline	float		vertical_fov				( ) { return 57.5f; }
			float		aspect_ratio				( );
//private:
	xray::collision::space_partitioning_tree*		m_collision_tree;
//public:
	float											m_near_plane;
	float											m_far_plane;
	float											m_selection_rate;
	math::color*									m_selection_color;
	bool											m_mouse_in_view;
	System::Drawing::Rectangle						m_default_clip;

	ToolStrip^				create_tool_strip		( System::String^ name, int prio );
	ToolStrip^				get_tool_strip			( System::String^ name );
	void					remove_action_button_item( editor_base::gui_binder^ binder, System::String^ action_name, System::String^ strip_key );
	void					add_action_button_item	( editor_base::gui_binder^ binder, System::String^ action_name, System::String^ strip_key, int prio );
	void					add_action_button_item	( editor_base::gui_binder^ binder, action^ action, System::String^ strip_key, int prio );

	void					setup_scene				( render::scene_pointer scene, render::scene_view_pointer scene_view, render::editor::renderer& editor_renderer, bool query_axis );
	inline	render::scene_ptr const& scene			( ) { return *m_scene; }
	void					clear_scene				( );

	void					clear_resources			( );
	void					subscribe_on_drag_drop	( System::Windows::Forms::DragEventHandler^ e );
	void					subscribe_on_drag_enter	( System::Windows::Forms::DragEventHandler^ e );
	void					action_focus_to_origin	( );
	void					action_focus_to_point	( math::float3 const& point );
	void					action_focus_to_bbox	( math::aabb const& bbox );

	void					switch_render_grid		( )				{ m_render_grid = !m_render_grid; }
	bool					get_render_grid			( )				{ return m_render_grid; }
	void					set_render_grid			( bool value )	{ m_render_grid = value; }

	void					switch_camera_light		( );
	bool					get_camera_light		( )				{ return m_camera_light_id!=0; }

	void					switch_sun				( );
	bool					get_sun_active			( )				{ return m_sun_light_id>1; }

	void					switch_wsad_mode		( )		{ m_wsad_mode_enabled =!m_wsad_mode_enabled; }
	bool					get_wsad_mode			( )		{ return m_wsad_mode_enabled;}

	void					switch_postprocess		( );
	bool					get_postprocess			( )				{ return m_use_postprocess;}

	void					register_actions		( input_engine^ ie, gui_binder^ binder, bool camera_light );
	bool					context_fit				( );
	
	void					set_flying_camera_destination	( float4x4 view_tranform );

	void					load_settings			( RegistryKey^ owner_editor_key );
	void					save_settings			( RegistryKey^ owner_editor_key );

	int						render_view_mode		( ){ return m_render_view_mode; }
	void					set_render_view_mode	( int m ){ m_render_view_mode = m; }

	static float			m_grid_step				= 1.0f;
	static u32				m_grid_dim				= 100;

private:
	void					render_grid				( );

	System::Drawing::Point			get_view_center	( );
	void							update_camera_ligtht	( );
	
	xray::render::editor::renderer*	m_editor_renderer;
	xray::math::float4x4*			m_inverted_view;
	float 							m_view_point_distance;
	math::float4x4*					m_projection;
	u32								m_focus_enter_frame;
	pick_cache*						m_cached;

	u32								m_sun_light_id;
	u32								m_camera_light_id;
	
	static float					m_mouse_sensitivity		= 0.5f;

	System::Drawing::Size			m_prev_size;// temp
	event camera_moved_delegate^	m_camera_moved;
	xray::camera_effector*			m_camera_effector;
	bool							m_infinite_mode;
	System::Drawing::Point			m_mouse_position_infinite_mode;
	System::Drawing::Point			m_mouse_campure_point;
	render::static_model_ptr*		m_axis_model;
	bool							m_render_grid;
	bool							m_use_postprocess;
	bool							m_moved_last_frame;
	bool							m_wsad_mode_enabled;

	int								m_render_view_mode;
	ToolStripButton^				m_render_modes_button;


	bool							m_flying_camera_animating;
	float4x4* 						m_flying_camera_destination;
	float3*							m_flying_camera_destination_vector;
	float							m_flying_camera_speed_starting;
	float							m_flying_camera_speed;
	float							m_flying_camera_acceleration;
	float							m_flying_camera_acceleration_starting;
	float							m_flying_camera_acceleration_increase;
	float							m_flying_camera_distance_factor;
	float							m_flying_camera_distance;
	timing::timer*					m_flying_camera_timer;
	render::scene_ptr*				m_scene;
	render::scene_view_ptr*			m_scene_view;

	void							on_axis_model_ready		( resources::queries_result& data );
	void							on_camera_move			( );
	void							view_panel_MouseMove	( System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e );
	void							view_panel_Resize		( System::Object^  sender, System::EventArgs^ e );
	void							render_mode_btn_click	( System::Object^  sender, System::EventArgs^ e );
	void							render_mode_menu_click	( System::Object^, System::EventArgs^ );
	void							render_mode_btn_mouse_down( System::Object^  sender, System::Windows::Forms::MouseEventArgs^ e );
	void							apply_render_view_mode	( bool text_only );


	void 							scene_view_panel_Enter	( System::Object^  sender, System::EventArgs^ e);
	void 							scene_view_panel_Leave	( System::Object^  sender, System::EventArgs^ e);
};

} // namespace editor_base
} // namespace xray

#endif // #ifndef SCENE_VIEW_PANEL_H_INCLUDED