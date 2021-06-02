#include "pch.h"
#include "triangulation_tester_canvas.h"
#include <xray/ai_navigation/sources/delaunay_triangulator.h>
#include <xray/ai_navigation/sources/constrained_delaunay_triangulator.h>
#include <xray/ai_navigation/sources/navigation_mesh_types.h>
#include <xray/strings_stream.h>

using xray::editor::triangulation_tester_canvas;
using xray::ai::navigation::delaunay_triangulator;
using xray::ai::navigation::constrained_delaunay_triangulator;
using xray::ai::navigation::triangles_mesh_type;
using xray::ai::navigation::fill_coordinate_indices;

using namespace System::Drawing;
using namespace xray::configs;

triangulation_tester_canvas::triangulation_tester_canvas(void) :
	font("Aerial", 12 ),
	m_start( xray::math::float2(0.0f, 0.0f ) ),
	m_scale( xray::math::float2(1000000.0f, 1000000.0f ) ),
//	m_scale( xray::math::float2(10000.0f, 10000.0f ) ),
	m_circles_scale( xray::math::epsilon_5 ),
	m_locked_vertex_id ( u32(-1) ),
	m_normal ( float3( 0, 1, 0) ),
	m_delaunay_operation_id ( u32(-1) ),
	m_show_triangle_ids( true ),
	m_show_bounding_triangle( false ),
	m_show_errors( false ),
	m_indices( 0 ),
	m_triangulation_type ( 0 )
{
	m_vertices = NEW(vertices_type);
	m_edges = NEW(edges_type);
	m_error_vertices_indices = NEW(error_indices_type);
	m_error_edges_indices = NEW(error_indices_type);
	m_edges->push_back( constrained_delaunay_triangulator::constraint_edge( 0, 1 ) );
	DoubleBuffered = true;
	InitializeComponent();
}

triangulation_tester_canvas::~triangulation_tester_canvas()
{
	if (components)
	{
		delete components;
	}
	DELETE(m_vertices);
	DELETE(m_edges);
	DELETE(m_error_vertices_indices);
	DELETE(m_error_edges_indices);
	FREE( m_indices_buffer );
}

void triangulation_tester_canvas::remove_triangulation()
{
	if ( m_indices && m_show_bounding_triangle ) {
		m_vertices->resize( m_vertices->size() - 3 );
	}

	if ( m_indices != 0 ) {
		DELETE( m_indices );
		FREE( m_indices_buffer );
		m_indices = 0;
	}

}

void triangulation_tester_canvas::triangulate_vertices ( )
{
	remove_triangulation( );

	m_error_vertices_indices->clear();
	m_error_edges_indices->clear();

	if ( m_vertices->size() < 3 )
		return;

	if ( m_show_errors )
		scan_for_errors ( );

	u32 const total_indices_count = 3 * delaunay_triangulator::get_maximum_triangle_count_in_triangulation( m_vertices->size() );
	m_indices_buffer = MALLOC( total_indices_count * sizeof( delaunay_triangulator::indices_type::value_type ), "Delaunay Indices");
	m_indices = NEW( indices_type )(
		m_indices_buffer,
		total_indices_count
	);

	if ( m_triangulation_type == 0 ) {
		delaunay_triangulator::error_vertices_indices_type error_vertices_indices;
		delaunay_triangulator (
			*m_vertices,
			*m_indices,
			float3( m_normal.x, m_normal.y, m_normal.z ),
			&error_vertices_indices,
			m_delaunay_operation_id,
			m_show_bounding_triangle
		);
		R_ASSERT ( error_vertices_indices.empty() );
	} else {
		constrained_delaunay_triangulator (
			*m_vertices,
			*m_indices,
			float3( m_normal.x, m_normal.y, m_normal.z ),
			*m_edges,
			0,
			m_delaunay_operation_id,
			m_show_bounding_triangle
		);
	}
}

bool triangulation_tester_canvas::is_constraint ( u32 const v0, u32 const v1 )
{
	u32 const constraints_count = m_edges->size();
	for ( u32 i = 0; i < constraints_count; ++i ) {
		if ( v0 == (*m_edges)[i].vertex_index0 && v1 == (*m_edges)[i].vertex_index1 )
			return true;
		if ( v0 == (*m_edges)[i].vertex_index1 && v1 == (*m_edges)[i].vertex_index0 )
			return true;
	}
	return false;
}

bool clip_line(
		u32 const (&coordinate_indices)[2],
		u32 clip_axis,
		float3& v0, 
		float3& v1, 
		float coord, 
		char sign 
	)
{
	u32 second_axis = coordinate_indices[ !clip_axis ];
	clip_axis = coordinate_indices[ clip_axis ];

	float sign0 = (v0[clip_axis] - coord);
	float sign1 = (v1[clip_axis] - coord);

	if ( sign0 * sign1 >= 0 ) {
		if ( sign0 * sign >= 0 )
			return true;
		else
			return false;
	}

	double diff0 = v1[clip_axis] - v0[clip_axis];
	double diff1 = v1[second_axis] - v0[second_axis];
	if ( sign0 * sign < 0 ) {
		v0[second_axis] = (float)(v0[second_axis] - (diff1 / diff0) * sign0);
		v0[clip_axis] = coord;
	} 
	if ( sign1 * sign < 0 ) {
		v1[second_axis] = (float)(v1[second_axis] - (diff1 / diff0) * sign1);
		v1[clip_axis] = coord;
	}

	return true;
}

bool triangulation_tester_canvas::clip_line_to_screen ( float3& v0, float3& v1)
{
	u32 coordinate_indices[2];
	fill_coordinate_indices ( float3( m_normal.x, m_normal.y, m_normal.z ), coordinate_indices );

	float half_width = ( Size.Width / 2 ) / m_scale.x;
	float half_height = ( Size.Height / 2 ) / m_scale.y;

	bool clipped = clip_line( coordinate_indices, 0, v0, v1, m_start.x - half_width,  1 );
	clipped &= clip_line( coordinate_indices, 0, v0, v1, m_start.x + half_width, -1 );
	clipped &= clip_line( coordinate_indices, 1, v0, v1, m_start.y - half_height,  1 );
	clipped &= clip_line( coordinate_indices, 1, v0, v1, m_start.y + half_height, -1 );
	
	return clipped;
}

void triangulation_tester_canvas::OnPaint( System::Windows::Forms::PaintEventArgs^ e )
{
	Pen pen( Color::Black );

	u32 coordinate_indices[2];
	fill_coordinate_indices( float3( m_normal.x, m_normal.y, m_normal.z ), coordinate_indices );

	int center_x = Size.Width / 2;
	int center_y = Size.Height / 2;

	vertices_type::iterator i = m_vertices->begin();
	vertices_type::iterator const vend = m_vertices->end();
	for ( ; i != vend; ++i ) {
		int translated_x = center_x - static_cast<int>(( (*i)[coordinate_indices[0]] - m_start.x )  * m_scale.x);
		int translated_y = center_y - static_cast<int>(( (*i)[coordinate_indices[1]] - m_start.y )  * m_scale.y);

		int w = Size.Width;
		int h = Size.Height;
		if ( translated_x < w
			&& translated_y < h
			&& translated_x > 0
			&& translated_y > 0 ) 
		{
			e->Graphics->DrawEllipse( %pen, translated_x-5, translated_y-5, 10, 10 );
			e->Graphics->DrawString( (i-m_vertices->begin()).ToString(), %font, System::Drawing::Brushes::Blue, translate_to_window(*i) );
		}

	}

	if ( m_indices != 0 ) {
		u32 const triangles_count = m_indices->size() / 3;
		for ( u32 i = 0; i < triangles_count; ++i ) {
			float3 const& v0 = (*m_vertices)[ (*m_indices)[i*3+0] ];
			float3 const& v1 = (*m_vertices)[ (*m_indices)[i*3+1] ];
			float3 const& v2 = (*m_vertices)[ (*m_indices)[i*3+2] ];
			Point p = translate_to_window( (v0 + v1 + v2) / 3 );

			e->Graphics->DrawString( i.ToString(), %font, System::Drawing::Brushes::Black, p );

			for ( u32 j = 0; j < 3; ++j ) {
				float3 v0 = (*m_vertices)[ (*m_indices)[i*3+((j+0)%3)] ];
				float3 v1 = (*m_vertices)[ (*m_indices)[i*3+((j+1)%3)] ];
				if ( m_triangulation_type == 1 && is_constraint( (*m_indices)[i*3+((j+0)%3)], (*m_indices)[i*3+((j+1)%3)] ) )
					pen.Width = 4.0;
				else
					pen.Width = 1.0;

				if ( clip_line_to_screen( v0, v1 ) )
					e->Graphics->DrawLine( %pen, translate_to_window( v0 ), translate_to_window( v1 ) );			
			}
		}
	}

	if ( m_show_errors ) {
		Pen pen( Color::Red );
		pen.Width = 4.0f;
		for ( u32 i = 0; i < m_error_vertices_indices->size(); ++i ) {
			u32 const& vertex_index = (*m_error_vertices_indices)[ i ];
			int translated_x = center_x - static_cast<int>(( (*m_vertices)[vertex_index][coordinate_indices[0]] - m_start.x )  * m_scale.x);
			int translated_y = center_y - static_cast<int>(( (*m_vertices)[vertex_index][coordinate_indices[1]] - m_start.y )  * m_scale.y);
			e->Graphics->FillEllipse( Brushes::Green, translated_x-10, translated_y-10, 20, 20 );
		}
		for ( u32 i = 0; i < m_error_edges_indices->size(); ++i ) {
			u32 const edge_id = (*m_error_edges_indices)[i];
			float3 const& v0 = (*m_vertices)[ (*m_edges)[edge_id].vertex_index0 ];
			float3 const& v1 = (*m_vertices)[ (*m_edges)[edge_id].vertex_index1 ];

			e->Graphics->DrawLine( %pen, translate_to_window( v0 ), translate_to_window( v1 ) );			
		}
	}

	e->Graphics->DrawString( String::Format( "Center:({0:f2}, {1:f2}) Scale:({2:f2}, {3:f2})", m_start.x, m_start.y, m_scale.x, m_scale.y ), %font, System::Drawing::Brushes::Black, 0, 0 );

	if ( m_locked_vertex_id < m_vertices->size() ) {
		e->Graphics->DrawString( String::Format( "Vertex:({0:f5}, {1:f5})", (*m_vertices)[ m_locked_vertex_id ][coordinate_indices[0]], (*m_vertices)[ m_locked_vertex_id ][coordinate_indices[1]] ), %font, System::Drawing::Brushes::Black, 0, 20 );
	}
	e->Graphics->DrawString( String::Format( "Normal:({0:f2}, {1:f2}, {2:f2})", m_normal.x, m_normal.y, m_normal.z ), %font, System::Drawing::Brushes::Black, 0, 40 );
}

void triangulation_tester_canvas::add_vertex ( float3 const& v )
{
	remove_triangulation ( );
	m_vertices->push_back( v );
	triangulate_vertices( );
	Invalidate( );
}

void triangulation_tester_canvas::OnMouseUp( System::Windows::Forms::MouseEventArgs^ e )
{
	if ( (e->Button & System::Windows::Forms::MouseButtons::Right) == System::Windows::Forms::MouseButtons::Right ) {
		if ( (Control::ModifierKeys & System::Windows::Forms::Keys::Control) == System::Windows::Forms::Keys::Control ) {
			if ( m_locked_vertex_id != u32(-1) ) {
				m_vertices->erase( m_vertices->begin() + m_locked_vertex_id );
				this->Invalidate();
				m_locked_vertex_id = u32(-1);
				triangulate_vertices( );
				return;
			}
		}
		return;
	}

	if ( e->Button == System::Windows::Forms::MouseButtons::Left ) {
		if ( Control::ModifierKeys != System::Windows::Forms::Keys::None )
			return;

		if ( m_locked_vertex_id != u32(-1) )
			return;

		u32 coordinate_indices[2];
		fill_coordinate_indices( float3( m_normal.x, m_normal.y, m_normal.z ), coordinate_indices );
		int center_x = Size.Width / 2;
		int center_y = Size.Height / 2;

		float real_x = m_start.x + ( center_x - e->X ) / m_scale.x;
		float real_y = m_start.y + ( center_y - e->Y ) / m_scale.y;
		float3 vertex = float3( 0, 0, 0 );
		vertex[coordinate_indices[0]] = real_x;
		vertex[coordinate_indices[1]] = real_y;
		add_vertex( vertex );
	}
}

void triangulation_tester_canvas::OnMouseMove( System::Windows::Forms::MouseEventArgs^ e )
{

	if ( e->Button == System::Windows::Forms::MouseButtons::Right) {
		float diff_x = ( e->X - m_locked_point.X ) / m_scale.x;
		float diff_y = ( e->Y - m_locked_point.Y ) / m_scale.y;
		m_start.x = m_locked_start.x + diff_x;
		m_start.y = m_locked_start.y + diff_y;
		this->Invalidate();
	}
	if ( e->Button == System::Windows::Forms::MouseButtons::Left) {
		if ( ( Control::ModifierKeys & System::Windows::Forms::Keys::Control ) == System::Windows::Forms::Keys::Control ) {
			if ( m_locked_vertex_id != u32(-1) ) {
				u32 coordinate_indices[2];
				fill_coordinate_indices( float3( m_normal.x, m_normal.y, m_normal.z ), coordinate_indices );
				Float2 real_vertex = translate_to_real( e->X, e->Y );
				(*m_vertices)[ m_locked_vertex_id ][coordinate_indices[0]] = real_vertex.x;
				(*m_vertices)[ m_locked_vertex_id ][coordinate_indices[1]] = real_vertex.y;
				triangulate_vertices( );
				this->Invalidate();
			}
		}
	}

}

u32 triangulation_tester_canvas::pick_nearest_vertex ( Point p )
{
	vertices_type::iterator i = m_vertices->begin();
	vertices_type::iterator const vend = m_vertices->end();
	for ( ; i != vend; ++i ) {
		Point translated = translate_to_window( (*i) );
		float distance = math::sqrt( math::sqr( static_cast<float>(p.X-translated.X ) ) + math::sqr( static_cast<float>(p.Y-translated.Y) ) );
		if (  distance < 10 )
			return i - m_vertices->begin();
	}

	return u32(-1);
}

Float2 triangulation_tester_canvas::translate_to_real( int x, int y)
{
	int center_x = Size.Width / 2;
	int center_y = Size.Height / 2;
	return Float2(float2( m_start.x + ( center_x - x ) / m_scale.x, m_start.y + ( center_y - y ) / m_scale.y ));
}

Point triangulation_tester_canvas::translate_to_window( float3 const& v )
{
	u32 coordinate_indices[2];
	fill_coordinate_indices( float3( m_normal.x, m_normal.y, m_normal.z ), coordinate_indices );
	int center_x = Size.Width / 2;
	int center_y = Size.Height / 2;
	return Point( center_x - static_cast<int>(( v[coordinate_indices[0]] - m_start.x )  * m_scale.x) , center_y - static_cast<int>(( v[coordinate_indices[1]] - m_start.y )  * m_scale.y));
}

void triangulation_tester_canvas::OnMouseDown( System::Windows::Forms::MouseEventArgs^ e )
{
	this->Focus();
	m_locked_point = e->Location;
	m_locked_start = m_start;
	m_locked_vertex_id = pick_nearest_vertex ( Point(e->X, e->Y ) );
}

void triangulation_tester_canvas::OnMouseWheel( System::Windows::Forms::MouseEventArgs^ e )
{
	float diff = 1.1f;
	if ( ( Control::ModifierKeys & System::Windows::Forms::Keys::Control)  == System::Windows::Forms::Keys::Control )
		diff *= 10;
	if ( ( Control::ModifierKeys & System::Windows::Forms::Keys::Shift)  == System::Windows::Forms::Keys::Shift )
		diff /= 10;
	if ( ( Control::ModifierKeys & System::Windows::Forms::Keys::Alt)  == System::Windows::Forms::Keys::Alt ) {
		m_circles_scale += diff;
		Invalidate();
		return;
	}

	int center_x = Size.Width / 2;
	int center_y = Size.Height / 2;

	float real_x = m_start.x + (float)( center_x - e->X ) / m_scale.x;
	float real_y = m_start.y + (float)( center_y - e->Y ) / m_scale.y;

	if ( e->Delta > 0 ) {
		m_scale.x *= diff;
		m_scale.y *= diff;
	} else {
		m_scale.x /= diff;
		m_scale.y /= diff;
	}

	if ( m_scale.x < .001f )
		m_scale.x = .001f;
	if ( m_scale.y < .001f )
		m_scale.y = .001f;

	m_start.x = real_x - ( center_x - e->X ) / m_scale.x;
	m_start.y = real_y - ( center_y - e->Y ) / m_scale.y;
	
	m_locked_start.x -= ( center_x - e->X ) / m_scale.x;
	m_locked_start.y -= ( center_y - e->Y ) / m_scale.y;

	this->Invalidate();
}


void triangulation_tester_canvas::zoom_in ( float diff )
{
	m_scale.x += diff;
	m_scale.y += diff;
	Invalidate();
}

void triangulation_tester_canvas::zoom_out ( float diff )
{
	m_scale.x -= diff;
	m_scale.y -= diff;
	Invalidate();
}

void triangulation_tester_canvas::OnKeyPress(System::Windows::Forms::KeyPressEventArgs^ e) 
{
	if ( e->KeyChar == '+' )
		zoom_in( 1000 );

	if ( e->KeyChar == '-' )
		zoom_out( 1000 );

	if ( e->KeyChar == 'R' ) {
		m_vertices->clear();
		triangulate_vertices ( );
		m_scale.x = 0;
		m_scale.y = 0;
		m_circles_scale = 1.0f;
		m_start.x = 0;
		m_start.y = 0;
	}

	if ( e->KeyChar == '0' ) {
		m_delaunay_operation_id = 0;
		triangulate_vertices( );
		Invalidate( );
	}

	if ( e->KeyChar == '9' ) {
		++m_delaunay_operation_id;
		triangulate_vertices();
		Invalidate();
	}

	if ( e->KeyChar == '8' ) {
		if ( m_delaunay_operation_id > 0 )
			--m_delaunay_operation_id;
		triangulate_vertices();
		Invalidate();
	}

	if ( e->KeyChar == '7' ) {
		m_delaunay_operation_id = u32(-1);
		triangulate_vertices();
		Invalidate();
	}

	if ( e->KeyChar == 3  ) {
		lua_config_ptr config = create_lua_config();
		build_lua_config			( config );
		strings::stream stream		(g_allocator);
		config->save				( stream );
		stream.append				( "%c", 0 );

		os::copy_to_clipboard		( (pcstr)stream.get_buffer() );	
	}

	if ( e->KeyChar == 'f' ) {
		center_view					( );
	}

	if ( e->KeyChar == 't' ) {
		remove_triangulation ( );
		m_vertices->clear();

		/**/
		m_vertices->push_back( math::float3( 12.5359859467f, 0.6659072638f, 0.7667285204f ) );
		m_vertices->push_back( math::float3( 12.5359859467f, 0.6659074426f, 2.0721290112f ) );
		m_vertices->push_back( math::float3( 12.5359859467f, 0.6658009291f, 6.1649851799f ) );
		m_vertices->push_back( math::float3( 12.5359811783f, 0.6658009291f, -5.3566160202f ) );
		m_vertices->push_back( math::float3( 12.5359859467f, 0.6659072638f, 2.1221289635f ) );
		m_vertices->push_back( math::float3( 12.5359859467f, 2.5474681854f, 0.7667285204f ) );
		m_vertices->push_back( math::float3( 12.5359859467f, 2.5474681854f, 2.0721290112f ) );
		m_vertices->push_back( math::float3( 12.5359859467f, 4.2740011215f, -5.3566160202f ) );
		m_vertices->push_back( math::float3( 12.5359859467f, 4.2740011215f, 6.1649851799f ) );
		m_vertices->push_back( math::float3( 12.5359811783f, 4.2740011215f, 5.9113974571f ) );
		m_vertices->push_back( math::float3( 12.5359706879f, 4.2740011215f, -5.5301074982f ) );
		m_vertices->push_back( math::float3( 12.5359640121f, 0.6661109924f, -5.5301074982f ) );
		m_vertices->push_back( math::float3( 12.5367069244f, 0.6659045219f, 2.2578983307f ) ); 
		/**/
		
		center_view				( );
		triangulate_vertices	( );
		Invalidate				( );
	}
}

void triangulation_tester_canvas::build_lua_config ( xray::configs::lua_config_ptr& config )
{
	config->get_root()["normal"] = float3( m_normal.x, m_normal.y, m_normal.z );
	xray::configs::lua_config_value vertices_value = config->get_root()["vertices"];
	
	u32 const vertices_count = m_vertices->size();
	for ( u32 i = 0 ; i < vertices_count; ++i ) {
		vertices_value[i] = (*m_vertices)[i];
	}
}

void triangulation_tester_canvas::load_from_file ( String^ filename )
{
	FILE* fd = fopen( unmanaged_string( filename ).c_str(), "rb");
	if ( !fd )
		return;

	fseek(fd, 0, SEEK_END );
	u32 const file_size = ftell( fd );
	char* buffer = static_cast<char*>(ALLOCA( file_size + 1));
	fseek( fd, 0, SEEK_SET );
	fread( buffer, file_size, 1, fd );
	fclose(fd);
	buffer[ file_size ] = 0;
	lua_config_ptr config = create_lua_config_from_string( buffer );
	load_lua_config( config );
}

void triangulation_tester_canvas::center_view( )
{
	if ( m_vertices->empty() )
		return;

	u32 coordinate_indices[2];
	fill_coordinate_indices( float3( m_normal.x, m_normal.y, m_normal.z ), coordinate_indices );

	float min_x = (*m_vertices)[0][coordinate_indices[0]];
	float min_y = (*m_vertices)[0][coordinate_indices[1]];
	float max_x = min_x;
	float max_y = min_y;

	float3 middle = float3(0, 0, 0);
	u32 const vertices_count = m_vertices->size();
	for ( u32 i = 0; i <vertices_count; ++i ) {
		float cx = (*m_vertices)[i][coordinate_indices[0]];
		float cy = (*m_vertices)[i][coordinate_indices[1]];

		if ( cx < min_x )
			min_x = cx;
		if ( cx > max_x )
			max_x = cx;
		if ( cy < min_y )
			min_y = cy;
		if ( cy > max_y )
			max_y = cy;
	}

	m_start.x = (min_x + max_x) * 0.5f;
	m_start.y = (min_y + max_y) * 0.5f;

	if ( max_y - min_y > max_x - min_x ) {
		m_scale.x = ( Height * 0.5f ) / (( max_y - min_y ) * 0.5f);
		m_scale.y = ( Height * 0.5f ) / (( max_y - min_y ) * 0.5f);
	} else {
		m_scale.x = ( Width * 0.5f ) / (( max_x - min_x ) * 0.5f);
		m_scale.y = ( Width * 0.5f ) / (( max_x - min_x ) * 0.5f);
	}

	Invalidate();
}

void triangulation_tester_canvas::load_lua_config ( xray::configs::lua_config_ptr& config )
{
	remove_triangulation( );
	m_vertices->clear();
	m_edges->clear();
	lua_config_value root = config->get_root();
	lua_config_value value = root["vertices"];

	m_normal =Float3( root["normal"] );
	u32 const vertices_count = value.size();
	for ( u32 i = 0; i < vertices_count; ++i )
		m_vertices->push_back( value[i] );

	m_triangulation_type = 0;
	if ( root.value_exists( "edges" ) ) {
		m_triangulation_type = 1;
		value = root["edges"];
		u32 const edges_count = value.size();
		for ( u32 i = 0; i < edges_count; ++i ) {
			lua_config_value edge_info = value[i];
			m_edges->push_back( xray::ai::navigation::constrained_delaunay_triangulator::constraint_edge( edge_info[0], edge_info[1] ) );
		}
	}

	center_view				( );
	triangulate_vertices	( );
	Invalidate				( );
}

void triangulation_tester_canvas::show_bounding_triangle( bool flag )
{
	remove_triangulation		( );
	m_show_bounding_triangle	= flag;
	triangulate_vertices		( );
	Invalidate					( );
}

void triangulation_tester_canvas::clear_vertices( )
{
	remove_triangulation		( );
	m_vertices->clear			( );
	Invalidate					( );
}

void triangulation_tester_canvas::set_triangulation_type( u32 type )
{
	remove_triangulation		( );
	m_triangulation_type		= type;
	triangulate_vertices		( );
	Invalidate					( );
}

void triangulation_tester_canvas::OnMouseEnter(System::EventArgs^)
{
	this->Focus();
}

void triangulation_tester_canvas::set_show_errors( bool flag )
{
	m_show_errors				= flag;
	scan_for_errors				( );
	Invalidate();
}

void triangulation_tester_canvas::scan_for_errors ( )
{
	u32 coordinate_indices[2];
	fill_coordinate_indices( float3( m_normal.x, m_normal.y, m_normal.z ), coordinate_indices );

	u32 const constraint_count = (*m_edges).size();
	for ( u32 i = 0; i < constraint_count; ++i ) {
		u32 vertex_index0 = (*m_edges)[i].vertex_index0;
		u32 vertex_index1 = (*m_edges)[i].vertex_index1;
		if ( vertex_index0 > m_vertices->size() || vertex_index1 > m_vertices->size() )
			continue;

		for ( u32 j = 0; j < (*m_vertices).size(); ++j ) {
			if ( j == vertex_index0 || j == vertex_index1 )
				continue;
			if ( is_on_segment( coordinate_indices, (*m_vertices)[ j ], (*m_vertices)[ vertex_index0 ], (*m_vertices)[ vertex_index1 ] ) ) {
				m_error_vertices_indices->push_back( j );
				m_error_edges_indices->push_back( i );
			}
		}
	}

	for ( u32 i = 0; i < constraint_count; ++i ) {
		float3 v0 = (*m_vertices)[ (*m_edges)[i].vertex_index0 ];
		float3 v1 = (*m_vertices)[ (*m_edges)[i].vertex_index1 ];

		for ( u32 j = i + 1; j < constraint_count; ++j ) {
			float3 u0 = (*m_vertices)[ (*m_edges)[j].vertex_index0 ];
			float3 u1 = (*m_vertices)[ (*m_edges)[j].vertex_index1 ];

			float3 intersections[2];
			bool are_on_same_line;
			u32 result = ai::navigation::segment_intersects_segment(
				coordinate_indices,
				v0, v1,
				u0, u1,
				intersections[0],
				intersections[1],
				are_on_same_line
			);

//			R_ASSERT_CMP( result, ==, 0);
			if ( result ) {
				float squared_length0 = squared_length( u0 - v0 );
				float squared_length1 = squared_length( u1 - v0 );
				if ( squared_length1 < squared_length0 )
					std::swap( u0, u1 );

				if ( math::is_similar( v0, u0 ) || math::is_similar(v1, u0) || math::is_similar( v1, u1 ) )
					continue;
				m_error_edges_indices->push_back( i );
				m_error_edges_indices->push_back( j );
			}

		}
	}


}
