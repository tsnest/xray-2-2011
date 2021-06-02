////////////////////////////////////////////////////////////////////////////
//	Created		: 30.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef TRINAGULATION_TESTER_CANVAS_H_INCLUDED
#define TRINAGULATION_TESTER_CANVAS_H_INCLUDED

#include <xray/ai_navigation/sources/constrained_delaunay_triangulator.h>

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace xray {
	namespace editor {

	/// <summary>
	/// Summary for trinagulation_tester_canvas
	/// </summary>
	public ref class triangulation_tester_canvas : public System::Windows::Forms::UserControl
	{
	public:
		triangulation_tester_canvas(void);

		void	build_lua_config		( xray::configs::lua_config_ptr& config );
		void	load_from_file			( String^ filename);
		void	show_bounding_triangle	( bool flag );
		void	clear_vertices			( );
		void	set_triangulation_type	( u32 type );
		u32		get_triangulation_type	( ) { return m_triangulation_type; }
		void	set_show_errors			( bool flag );

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~triangulation_tester_canvas();

		virtual void OnPaint( System::Windows::Forms::PaintEventArgs^ e ) override;

		virtual void OnMouseDown(System::Windows::Forms::MouseEventArgs^ e) override;
		virtual void OnMouseMove(System::Windows::Forms::MouseEventArgs^ e) override;
		virtual void OnMouseUp(System::Windows::Forms::MouseEventArgs^ e) override;
		virtual void OnMouseWheel(System::Windows::Forms::MouseEventArgs^ e) override;
		virtual void OnKeyPress(System::Windows::Forms::KeyPressEventArgs^ e) override;
		virtual void OnMouseEnter(System::EventArgs^ e) override;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
		System::Drawing::Font font;

		Float2	translate_to_real		( int x, int y);
		Point	translate_to_window		( float3 const& v );
		u32		pick_nearest_vertex		( Point p );
		void	triangulate_vertices	( );
		void	zoom_in					( float diff );
		void	zoom_out				( float diff );
		void	add_vertex				( float3 const& v );
		void	load_lua_config			( xray::configs::lua_config_ptr& config );
		void	center_view				( );
		void	remove_triangulation	( );
		bool	is_constraint			( u32 const v0, u32 const v1 );
		void	scan_for_errors			( );
		bool	clip_line_to_screen		( float3& v0, float3& v1);

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->SuspendLayout();
			// 
			// triangulation_tester_canvas
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Name = L"triangulation_tester_canvas";
			this->ResumeLayout(false);
		}
#pragma endregion
	private:
		typedef xray::debug::vector< xray::math::float3 > vertices_type;
		typedef xray::ai::navigation::delaunay_triangulator::indices_type indices_type;
		typedef xray::ai::navigation::constrained_delaunay_triangulator::constraint_edges_type edges_type;

		u32				m_locked_vertex_id;
		Float3			m_normal;
		Float2			m_locked_start;
		Float2			m_start;
		Float2			m_scale;
		float			m_circles_scale;
		u32				m_delaunay_operation_id;
		u32				m_triangulation_type;
		bool			m_show_bounding_triangle;
		bool			m_show_errors;
		bool			m_show_triangle_ids;

		indices_type*	m_indices;
		void*			m_indices_buffer;
		Point			m_locked_point;

		typedef	vector< u32 > error_indices_type;
		error_indices_type*		m_error_vertices_indices;
		error_indices_type*		m_error_edges_indices;

	public:
		vertices_type*	m_vertices;
		edges_type*		m_edges;

	}; // class triangulation_tester_canvas

} // namespace editor
} // namespace xray

#endif // #ifndef TRINAGULATION_TESTER_CANVAS_H_INCLUDED
