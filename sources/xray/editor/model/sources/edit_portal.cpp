////////////////////////////////////////////////////////////////////////////
//	Created		: 01.06.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "edit_portal.h"
#include "input_actions.h"
#include "edit_object_base.h"
#include "unique_name_helper.h"
#include "portal_vertex_shift.h"
#include "model_editor_object.h"
#include "model_editor.h"

#pragma managed( push, off )
#include <xray/render/facade/debug_renderer.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/editor/base/collision_object_types.h>
#include <numeric>
#include <functional>
#include "portal_vertex_collision.h"
#include "portal_collision_object.h"
#include "float3_similar.h"
#pragma managed( pop )


namespace xray {
namespace model_editor {

static const float c_vertex_size = 0.1f;
static const float3 c_vetrex_collision_coord[] = {
	float3 ( -c_vertex_size, 0.0f, c_vertex_size ), //0
	float3 ( c_vertex_size, 0.0f, c_vertex_size ),	//1
	float3 ( c_vertex_size, 0.0f, -c_vertex_size ),	//2
	float3 ( -c_vertex_size, 0.0f, -c_vertex_size ),	//3
	float3 ( 0.0f, math::sqrt( 2.0f ) * c_vertex_size, 0.0f ),			//4
	float3 ( 0.0f, -math::sqrt( 2.0f ) * c_vertex_size, 0.0f ),			//5
};
static const u32 c_vertices_count = sizeof (c_vetrex_collision_coord) / sizeof( c_vetrex_collision_coord[0] );

static const u32 c_vertex_collision_indices[] = {
	0, 1, 4,
	1, 2, 4,
	2, 3, 4,
	3, 0, 4,
	0, 5, 1, 
	1, 5, 2, 
	2, 5, 3, 
	3, 5, 0 
};
static const u32 c_indices_count = sizeof (c_vertex_collision_indices) / sizeof( c_vertex_collision_indices[0] );

static void draw_vertex_collision( render::scene_ptr const& scene, render::debug::renderer& r, float3 const& pos, math::color const& line_color, math::color const& plane_color)
{
	for ( u32 i = 0; i < c_indices_count - 2; i += 3 )
	{
		float3 const& first		=	pos + c_vetrex_collision_coord[	c_vertex_collision_indices[ i ]		];
		float3 const& second	=	pos + c_vetrex_collision_coord[	c_vertex_collision_indices[ i + 1 ] ];
		float3 const& third		=	pos + c_vetrex_collision_coord[	c_vertex_collision_indices[ i + 2 ] ];

		r.draw_triangle		( scene, first, second, third, plane_color );
		r.draw_line			( scene, first, second, line_color );
		r.draw_line			( scene, second, third, line_color );
		r.draw_line			( scene, third, first, line_color );
	}
}

struct vertex_similar: public std::binary_function<portal_vertex const, portal_vertex const, bool>
{
	bool operator() ( portal_vertex const& first, portal_vertex const& second ) const
	{
		return math::is_similar( first.coord, second.coord );
	}
};

template<typename Cont, typename EqPred >
bool get_three_different_elements( Cont const& cont, typename Cont::size_type& first, typename Cont::size_type& second, typename Cont::size_type& third, EqPred eq )
{
	R_ASSERT( cont.size() >= 3 );
	typename Cont::size_type i = 0;
	first = i;
	
	while ( 
		++i < cont.size() && 
		eq( cont[first], cont[i] ) 
		);
	if ( i == cont.size() )
		return false;
	second = i;

	while (
		++i < cont.size() && 
		( eq( cont[first], cont[i] ) ||  eq( cont[second], cont[i] ) )   
		);
	if ( i == cont.size() )
		return false;
	third = i;
	return true;
}

////////////////////////////////////////////////////////////////////////////
//constructor for portal that is loaded from config
edit_portal::edit_portal(  edit_object_mesh^ parent, unique_name_helper^ name_helper, portal_ilist^ portals, configs::lua_config_value const& cfg  )
:m_parent_object( parent ),
m_unique_name_helper( name_helper ),
m_instances( portals ),
m_name( System::String::Empty ),
m_vertices( NULL ),
m_indices( NULL ),
m_model_to_world( NULL ),
m_world_to_model( NULL ),
m_plane( NULL ),
m_vertex_collisions( NULL ),
m_vertex_collision_mesh( NULL ),
m_portal_collision( NULL ),
m_portal_collision_mesh( NULL ),
m_picked_vertex_index( static_cast<u32>( -1 ) ),
m_selected( false )
{
	in_constructor( );
	load_portal( cfg );
	this->initialise_plane( );
	this->update_collision_mesh( );
}

//constructor for portal that is created manually
edit_portal::edit_portal( edit_object_mesh^ parent, unique_name_helper^ name_helper, portal_ilist^ portals, System::String^ name )
:m_parent_object( parent ), 
m_unique_name_helper( name_helper ),
m_instances( portals ),
m_name( name ),
m_vertices( NULL ),
m_indices( NULL ),
m_model_to_world( NULL ),
m_world_to_model( NULL ),
m_plane( NULL ),
m_vertex_collisions( NULL ),
m_vertex_collision_mesh( NULL ),
m_portal_collision( NULL ),
m_portal_collision_mesh( NULL ),
m_picked_vertex_index( u32(-1) ),
m_selected( false )
{
	in_constructor( );
	init_default_vertices( );
	this->initialise_plane( );
	this->update_collision_mesh( );
}

void edit_portal::in_constructor( )
{
	m_vertices			=	NEW	( vertex_vector )	( );
	m_indices			=	NEW	( index_triple_vector )	( );
	m_model_to_world	=	NEW	( math::float4x4 )	();
	m_world_to_model	=	NEW	( math::float4x4 )	();
	m_plane				=	NEW	( math::plane )		();
	m_vertex_collisions	=	NEW ( vector<portal_vertex_collision*> ) ( );
	m_vertex_collision_mesh = &*collision::new_triangle_mesh_geometry( g_allocator, 
		c_vetrex_collision_coord, c_vertices_count, 
		c_vertex_collision_indices, c_indices_count );
}

void edit_portal::init_default_vertices( )
{
	m_vertices->reserve( 4 );
	m_vertices->push_back( portal_vertex( float3( -1, -1, 0 ) ) );
	m_vertices->push_back( portal_vertex( float3( -1,  1, 0 ) ) );
	m_vertices->push_back( portal_vertex( float3(  1,  1, 0 ) ) );
	m_vertices->push_back( portal_vertex( float3(  1, -1, 0 ) ) );

	m_indices->reserve( 2 );
	m_indices->push_back( triple<u32>( 0, 1, 2 ) );
	m_indices->push_back( triple<u32>( 0, 2, 3 ) );

	m_model_to_world->identity();
	m_world_to_model->identity();
}

edit_portal::~edit_portal( )
{
	m_parent_object->get_model_editor()->collision_tree->erase( m_portal_collision );
	DELETE ( m_portal_collision );
	collision::delete_geometry( g_allocator, m_portal_collision_mesh );
	remove_vertices_from_collision_tree();
	DELETE ( m_vertex_collisions );
	collision::delete_geometry( g_allocator, m_vertex_collision_mesh);
	DELETE ( m_plane );
	DELETE ( m_world_to_model );		
	DELETE ( m_model_to_world );		
	DELETE ( m_indices );		
	DELETE ( m_vertices );
}


void edit_portal::set_selected( bool selected )
{
	m_selected = selected;
	if ( selected )
	{
		this->insert_vertices_to_collision_tree( );
		this->deselect_other_portals();
		
		m_parent_object->get_model_editor()->m_transform_control_helper->m_object = 
			gcnew editor_base::transform_value_object( m_model_to_world, gcnew editor_base::value_modified( this, &edit_portal::on_matrix_modified ), true );

		m_parent_object->get_model_editor()->m_transform_control_helper->m_changed = true;
	}
	else
	{
		this->remove_vertices_from_collision_tree( );
	}
	for ( vertex_vector::iterator it = m_vertices->begin(); it != m_vertices->end(); ++it )
		it->set_selected( false );
}

void edit_portal::on_edit_vertices( )
{
	if ( std::find_if( m_vertices->begin(), m_vertices->end(), std::mem_fun_ref( &portal_vertex::is_selected ) ) != m_vertices->end() )
	{
		me_transform_control_helper^ helper = m_parent_object->get_model_editor()->m_transform_control_helper;
		if ( helper->m_object != nullptr )
			delete helper->m_object;

		m_parent_object->get_model_editor()->execute_action("select translate");
		
		helper->m_object = 
			gcnew portal_vertex_shift( this,
			gcnew portal_vertex_shift::on_modified_delegate(this, &edit_portal::on_vertex_modified),
			gcnew portal_vertex_shift::on_modified_delegate( this, &edit_portal::on_vertex_end_modify ) );

		helper->m_changed = true;
	}
}

void edit_portal::load_portal( configs::lua_config_value const& cfg )
{
	m_name	= gcnew System::String( cfg.get_field_id() );
	m_unique_name_helper->add( m_name );
	configs::lua_config_value const& vertices_cfg		= cfg["vertices"];
	configs::lua_config::const_iterator vertex_it		= vertices_cfg.begin( );
	configs::lua_config::const_iterator vertex_end_it	= vertices_cfg.end( );
	
	
	coord_vector world_coord;
	for ( ; vertex_it != vertex_end_it; ++vertex_it )
	{
		world_coord.push_back( static_cast<math::float3>(*vertex_it) );
	}

	configs::lua_config_value const& indices_cfg		= cfg["indices"];
	configs::lua_config::const_iterator index_it		= indices_cfg.begin( );
	configs::lua_config::const_iterator index_end_it	= indices_cfg.end( );
	vector<u32> indices;
	for ( ; index_it != index_end_it; ++index_it )
	{
		u32 index = static_cast<u32>( *index_it );
		indices.push_back( index );
	}
	R_ASSERT( indices.size() % 3 == 0 );
	for ( u32 i = 0; i < indices.size() - 2; i += 3 )
	{
		m_indices->push_back( triple<u32>( indices[ i ], indices[ i + 1 ], indices[ i + 2 ] ) );
	}
	m_vertices->resize( world_coord.size() );
	this->restore_model_and_matrix_from_world( world_coord );
}

void edit_portal::restore_model_and_matrix_from_world( coord_vector const& world_coords )
{
	R_ASSERT( world_coords.size() >= 3 && world_coords.size() == m_vertices->size() );
	coord_vector::size_type first, second, third;
	if ( !get_three_different_elements( world_coords, first, second, third, float3_similar() ) )
	{
		R_ASSERT( false, "There are no three different vertices in portal." );
	}
	float3 normal = get_plane_normal( world_coords[first], world_coords[second], world_coords[third] );
	float3 model_z_axis = ( ( normal | float3( 0.0f, 0.0f, 1.0f ) ) >= 0.0f ) ? normal : -normal;
	float3 model_x_axis = float3( 0.0f, 1.0f, 0.0f ) ^ model_z_axis;
	if ( math::is_zero( model_x_axis.length() ) )
		model_x_axis  = float3( 1.0f, 0.0f, 0.0f );
	float3 model_y_axis = model_z_axis ^ model_x_axis;
	model_x_axis.normalize_safe( float3( 1.0f, 0.0f, 0.0f ) );
	model_y_axis.normalize_safe( float3( 0.0f, 1.0f, 0.0f ) );
	model_z_axis.normalize_safe( float3( 0.0f, 0.0f, 1.0f ) );
	
	m_model_to_world->identity();
	m_model_to_world->i.xyz() = model_x_axis;
	m_model_to_world->j.xyz() = model_y_axis;
	m_model_to_world->k.xyz() = model_z_axis;
	float3 center_point = std::accumulate( world_coords.begin(), world_coords.end(), float3( 0.0f, 0.0f, 0.0f ) );
	center_point /= static_cast<float>( world_coords.size() );
	m_model_to_world->c.xyz() = center_point;
	R_ASSERT( m_model_to_world->valid() );
	m_world_to_model->try_invert( *m_model_to_world );
	R_ASSERT( m_world_to_model->valid() );

	for ( u32 i = 0; i < world_coords.size(); ++i )
	{
		(*m_vertices)[i].coord = m_world_to_model->transform_position( world_coords[i] );
	}
	this->initialise_plane( );
}


void edit_portal::save_portal( configs::lua_config_value& cfg )
{
	configs::lua_config_value vertices_cfg = cfg["vertices"];
	for ( u32 i = 0; i < m_vertices->size(); ++i )
	{
		vertices_cfg[i] = m_model_to_world->transform_position( ( *m_vertices )[i].coord );
	}

	configs::lua_config_value indices_cfg = cfg["indices"];
	u32 i = 0;
	const index_triple_vector::const_iterator end_it = m_indices->end();
	for ( index_triple_vector::const_iterator it = m_indices->begin(); it != end_it; ++it )
	{
		indices_cfg[ i++ ] = ( *it )[0];
		indices_cfg[ i++ ] = ( *it )[1];
		indices_cfg[ i++ ] = ( *it )[2];
	}
}

void edit_portal::render( render::scene_ptr const& scene, render::debug::renderer& r )
{
	static const math::color portal_line_color				=	math::color( 0.0f, 0.0f, 1.0f, 1.0f );
	static const math::color selected_portal_line_color		=	math::color( 0.0f, 1.0f, 1.0f, 1.0f );	
	static const math::color portal_color					=	math::color( 0.0f, 0.0f, 1.0f, 0.5f );
	static const math::color selected_portal_color			=	math::color( 0.0f, 1.0f, 1.0f, 0.5f );

	math::color const& line_color	= m_selected ? selected_portal_line_color : portal_line_color;
	math::color const& plane_color	= m_selected ? selected_portal_color : portal_color;

	const index_triple_vector::const_iterator end_it = m_indices->end();
	for ( index_triple_vector::const_iterator it = m_indices->begin(); it != end_it; ++it )
	{
		float3 const& first		=	m_model_to_world->transform_position( ( *m_vertices )[ ( *it )[0] ].coord );
		float3 const& second	=	m_model_to_world->transform_position( ( *m_vertices )[ ( *it )[1] ].coord );
		float3 const& third		=	m_model_to_world->transform_position( ( *m_vertices )[ ( *it )[2] ].coord );

		r.draw_triangle		( scene, first, second, third, plane_color );
		r.draw_triangle		( scene, first, third, second, plane_color );
		r.draw_line			( scene, first, second, line_color );
		r.draw_line			( scene, second, third, line_color );
		r.draw_line			( scene, third, first, line_color );
	
		//float3 const& first_m	=	( *m_vertices )[ it->first ].coord;
		//float3 const& second_m	=	( *m_vertices )[ it->second ].coord;
		//float3 const& third_m	=	( *m_vertices )[ it->third].coord;
		//r.draw_line			( scene, first_m, second_m, line_color );
		//r.draw_line			( scene, second_m, third_m, line_color );
		//r.draw_line			( scene, third_m, first_m, line_color );
	}
	
	//float3 const& middle_point = std::accumulate( m_vertices->begin(), m_vertices->end(), float3( 0.0f, 0.0f, 0.0f ) );
	//r.draw_arrow( scene, middle_point, middle_point + m_plane->normal * 5.0f, portal_line_color );
	if ( this->is_selected() )
	{
		static const math::color picked_color					=	math::color( 1.0f, 1.0f, 0.0f, 0.5f );
		static const math::color picked_line_color				=	math::color( 1.0f, 1.0f, 0.0f, 1.0f );
		m_picked_vertex_index = this->get_picked_vertex_index( );
		for ( u32 i = 0; i < m_vertex_collisions->size(); ++i )
		{
			if ( is_vertex_selected( i ) )
				draw_vertex_collision( scene, r, ( *m_vertex_collisions )[i]->get_aabb().center(), selected_portal_line_color, selected_portal_color );
			else if ( i == m_picked_vertex_index )
				draw_vertex_collision( scene, r, ( *m_vertex_collisions )[i]->get_aabb().center(), picked_line_color, picked_color );			
			else
				draw_vertex_collision( scene, r, ( *m_vertex_collisions )[i]->get_aabb().center(), portal_line_color, portal_color );
		}
	}
}

u32 edit_portal::get_picked_vertex_index( )
{
	me_transform_control_helper^ helper = m_parent_object->get_model_editor()->m_transform_control_helper;
	float3 origin, direction;
	helper->get_mouse_ray( origin, direction );

	collision::ray_objects_type	result_objects(g_allocator);
	if( helper->m_collision_tree->ray_query(
		editor_base::collision_object_type_touch,
		origin,
		direction,
		1000.0f,
		result_objects,
		xray::collision::objects_predicate_type() ))
	{
		for ( collision::ray_objects_type::const_iterator it = result_objects.begin(); it != result_objects.end(); ++it )
		{
			portal_vertex_collision const* collision = dynamic_cast<portal_vertex_collision const*>( it->object );
			if ( collision != NULL )
				return collision->get_index();
		}
	}
	return static_cast<u32>( -1 );
}

bool edit_portal::is_selected( )
{
	return m_selected;
}

void edit_portal::on_matrix_modified( cli::array<System::String^>^ /*unused*/ )
{
	m_world_to_model->try_invert	( *m_model_to_world );
	R_ASSERT						( m_world_to_model->valid() );
	m_parent_object->get_model_editor()->collision_tree->move(
		m_portal_collision,
		*m_model_to_world
	);
	update_vertex_collisions		( );
	m_parent_object->set_modified	( );
}
	

void edit_portal::on_vertex_modified()
{
	m_parent_object->set_modified();
}

void edit_portal::on_vertex_end_modify( )
{
	coord_vector world_coord;
	world_coord.reserve( m_vertices->size() );
	for ( vertex_vector::const_iterator it = m_vertices->begin(); it != m_vertices->end(); ++it )
		world_coord.push_back( m_model_to_world->transform_position( it->coord ) );
	this->restore_model_and_matrix_from_world( world_coord );
	this->update_collision_mesh();
	this->update_vertex_collisions();
	m_parent_object->set_modified();
}

void edit_portal::update_vertex_collision( u32 index )
{
	portal_vertex_collision* collision = ( *m_vertex_collisions )[ index ];
	m_parent_object->get_model_editor()->collision_tree->move(
		collision,
		math::create_translation( m_model_to_world->transform_position( ( *m_vertices )[ index ].coord ) )
	);
}

void edit_portal::Name::set( System::String^ value )
{
	if ( m_unique_name_helper->contains( value ) || value->Length == 0 )
		return;

	m_unique_name_helper->remove( m_name );
	m_unique_name_helper->add( value );
	m_name = value;
	m_parent_object->set_modified();
	m_parent_object->update_portals_edit_property_view();
}

edit_portal::indices_collection^ edit_portal::get_selected_vertices( )
{
	indices_collection^ selected_vertices = gcnew indices_collection();
	for ( u32 i = 0; i < m_vertices->size(); ++i )
	{
		if ( ( *m_vertices )[ i ].is_selected( ) )
			selected_vertices->Add( i );
	}
	return selected_vertices;
}

void edit_portal::initialise_plane( )
{
	ASSERT( m_vertices->size() >= 3 );
	vertex_vector::size_type first, second, third;
	if ( get_three_different_elements( *m_vertices, first, second, third, vertex_similar() ) )
	{
		*m_plane = create_plane( (*m_vertices)[first].coord, (*m_vertices)[second].coord, (*m_vertices)[third].coord );
	}
	else
	{
		R_ASSERT( false, "There are no three different vertices in portal" );
	}
}

void edit_portal::on_vertex_touch( int index )
{
	for ( vertex_vector::iterator it = m_vertices->begin(); it != m_vertices->end(); ++it )
		it->set_selected( false );
	( *m_vertices )[ index ].set_selected( true );
	this->on_edit_vertices();
}

bool edit_portal::is_vertex_selected( int index )
{
	return ( *m_vertices )[ index ].is_selected();
}

void edit_portal::insert_vertices_to_collision_tree( )
{
	R_ASSERT( m_vertex_collisions != NULL );
	if( !m_vertex_collisions->empty() )
		return;
	m_vertex_collisions->reserve( m_vertices->size() );
	for ( u32 i = 0; i < m_vertices->size(); ++i )
	{
		portal_vertex_collision* collision = NEW ( portal_vertex_collision ) ( m_vertex_collision_mesh, this, i );
		m_vertex_collisions->push_back( collision );
		m_parent_object->get_model_editor()->collision_tree->insert(
			collision,
			math::create_translation( m_model_to_world->transform_position( ( *m_vertices )[ i ].coord ) )
		);
	}
}

void edit_portal::remove_vertices_from_collision_tree( )
{
	R_ASSERT( m_vertex_collisions != NULL );
	for ( u32 i = 0; i < m_vertex_collisions->size(); ++i )
	{
		m_parent_object->get_model_editor()->collision_tree->erase( ( *m_vertex_collisions )[ i ] );
		DELETE ( ( *m_vertex_collisions )[ i ] );
	}
	m_vertex_collisions->clear();
}

u32 edit_portal::get_collisions( System::Collections::Generic::List<editor_base::collision_object_wrapper>^% collisions )
{
	editor_base::collision_object_wrapper wrapper;
	wrapper.m_collision_object = m_portal_collision;
	collisions->Add( wrapper );
	for ( vertex_collisions::const_iterator it = m_vertex_collisions->begin(); it != m_vertex_collisions->end(); ++it )
	{
		wrapper.m_collision_object = *it;
		collisions->Add( wrapper );
	}
	return collisions->Count;
}

void edit_portal::update_collision_mesh( )
{
	if ( m_portal_collision_mesh )
	{
		collision::delete_geometry( g_allocator, m_portal_collision_mesh );
	}
	buffer_vector<u32> indices( _alloca( sizeof(u32) * m_indices->size() * 3 ), m_indices->size() * 3 );
	for ( index_triple_vector::const_iterator it = m_indices->begin(); it != m_indices->end(); ++it )
	{
		indices.push_back( ( *it )[0] );
		indices.push_back( ( *it )[1] );
		indices.push_back( ( *it )[2] );
	}
	buffer_vector<float3> vertices( _alloca( sizeof(float3) * m_vertices->size() ), m_vertices->size() );
	for ( vertex_vector::const_iterator it = m_vertices->begin(); it != m_vertices->end(); ++it )
	{
		vertices.push_back( it->coord );
	}
	m_portal_collision_mesh = 
		&*collision::new_triangle_mesh_geometry( g_allocator, &*(vertices.begin()), vertices.size(), &*(indices.begin()), indices.size() );

	if ( m_portal_collision )
	{
		m_parent_object->get_model_editor()->collision_tree->erase( m_portal_collision );
		DELETE ( m_portal_collision );
	}

	m_portal_collision = NEW ( portal_collision_object ) ( m_portal_collision_mesh, this );
	m_parent_object->get_model_editor()->collision_tree->insert( m_portal_collision, *m_model_to_world );
}

void edit_portal::deselect_other_portals( )
{
	for each ( edit_portal^ portal in m_instances )
	{
		if ( portal != this )
			portal->set_selected( false );
	}
}

bool edit_portal::are_vertices_coplanar( )
{
	R_ASSERT( m_vertices->size() >= 3 );
	if ( m_vertices->size() == 3 )
		return true;
	this->initialise_plane( );
	for ( u32 i = 3; i < m_vertices->size(); ++i )
	{
		if ( !math::is_zero( m_plane->distance( ( *m_vertices )[ i ].coord ) ) )
			return false;
	}
	return true;
}

void edit_portal::make_vertices_coplanar( )
{
	R_ASSERT( m_vertices->size() >= 3 );
	if (  m_vertices->size() == 3 )
		return;
	this->initialise_plane( );
	float3 projection( 0.0f, 0.0f, 0.0f );
	for ( u32 i = 3; i < m_vertices->size(); ++i )
	{
		if ( math::is_zero( m_plane->distance( ( *m_vertices )[ i ].coord ) ) )
			continue;
		projection = m_plane->project( ( *m_vertices )[ i ].coord );
		( *m_vertices )[ i ].coord = projection;
	}
	R_ASSERT( this->are_vertices_coplanar() );
	this->update_vertex_collisions();
	this->update_collision_mesh();
	m_parent_object->set_modified();
}

math::aabb edit_portal::get_aabb( )
{
	math::aabb bbox = math::create_invalid_aabb();
	vertex_vector::const_iterator first_selected = std::find_if( m_vertices->begin(), m_vertices->end(), std::mem_fun_ref( &portal_vertex::is_selected ) );
	if ( first_selected != m_vertices->end() )
	{
		float3 const & first_selected_coord = m_model_to_world->transform_position( first_selected->coord );
		bbox = math::create_aabb_min_max( first_selected_coord, first_selected_coord );
		for ( vertex_vector::const_iterator it = first_selected + 1; it != m_vertices->end(); ++it )
		{
			if ( it->is_selected() )
				bbox.modify( m_model_to_world->transform_position( it->coord ) );
		}
		bbox += float3(0.5f, 0.5f, 0.5f );
	}
	else
	{
		float3 first_vertex = m_model_to_world->transform_position( m_vertices->front().coord );
		bbox = math::create_aabb_min_max( first_vertex, first_vertex );
		for ( vertex_vector::const_iterator it = m_vertices->begin() + 1; it != m_vertices->end(); ++it )
		{
			bbox.modify( m_model_to_world->transform_position( it->coord ) );
		}
		bbox += float3(1.0f, 1.0f, 1.0f );
	}
	return bbox;
}

math::float4x4 const& edit_portal::get_transformation( )
{
	return *m_model_to_world;
}

float3 const& edit_portal::get_untransformed_vertex( int index )
{
	return ( *m_vertices )[ index ].coord;
}

void edit_portal::shift_vertex( int index, float3 const& shift )
{
	float3 old_value = ( *m_vertices )[ index ].coord;
	( *m_vertices )[ index ].coord += shift ;
	
	vertex_vector::size_type first, second, third;
	if ( get_three_different_elements( *m_vertices, first, second, third, vertex_similar() ) )
	{
		this->update_vertex_collision( index );
		this->update_collision_mesh( );
	}
	else
	{
		( *m_vertices )[ index ].coord = old_value;
	}
}

void edit_portal::add_vertex()
{
	float3 mouse_origin, mouse_direction;
	m_parent_object->get_model_editor()->m_transform_control_helper->get_mouse_ray( mouse_origin, mouse_direction );
	
	float3 current_point;
	if ( !editor_base::transform_control_base::plane_ray_intersect( 
		m_model_to_world->transform_position( m_vertices->front().coord ), 
		m_model_to_world->transform_direction( m_plane->normal ), 
		mouse_origin, mouse_direction, current_point ) )
		return;

	int first = -1, second = -1;
	if ( !this->get_two_nearest( m_world_to_model->transform_position( current_point ), first, second ) )
	{
		R_ASSERT( false, "Can't get two nearest vertices" );
		return;
	}

	m_indices->push_back( triple<u32>( m_vertices->size(), first, second ) );
	m_vertices->push_back( portal_vertex( m_world_to_model->transform_position( current_point ) ) );

	portal_vertex_collision* collision = NEW ( portal_vertex_collision ) ( m_vertex_collision_mesh, this, m_vertex_collisions->size() );
	m_vertex_collisions->push_back( collision );
	m_parent_object->get_model_editor()->collision_tree->insert( collision, create_translation( current_point ) );
	this->update_collision_mesh();
	m_parent_object->set_modified();
}

bool edit_portal::delete_selected_vertices()
{
	if ( m_vertices->size() <= 4 || std::find_if( m_vertices->begin(), m_vertices->end(), std::mem_fun_ref( &portal_vertex::is_selected ) ) == m_vertices->end() )
		return false;

	while ( this->delete_first_selected_vertex() );
	while ( this->delete_unused_vertex() );
	this->update_collision_mesh();
	m_parent_object->set_modified();
	return true;
}

bool edit_portal::delete_first_selected_vertex( )
{
	for ( u32 i = 0; i < m_vertices->size(); ++i )
	{
		if ( !( *m_vertices )[ i ].is_selected() )
			continue;
		m_indices->erase( std::remove_if( m_indices->begin(), m_indices->end(), std::bind2nd( std::mem_fun_ref( &triple<u32>::contains), i ) ), m_indices->end() );
		delete_vertex_impl( i );

		if ( m_parent_object->get_model_editor()->m_transform_control_helper->m_object != nullptr )
		{
			m_parent_object->get_model_editor()->m_transform_control_helper->m_object = nullptr;
			m_parent_object->get_model_editor()->m_transform_control_helper->m_changed = true;
		}
		return true;
	}
	return false;
}


bool edit_portal::get_two_nearest( float3 const& point, int& first, int& second )
{
	first = second = -1;
	float first_dist = math::infinity, second_dist = math::infinity;
	for ( u32 i = 0; i < m_vertices->size(); ++i )
	{
		float3 const& current = ( *m_vertices )[ i ].coord;
		float dist = squared_length( current - point );
		if ( dist < first_dist )
		{
			second_dist = first_dist;
			second = first;
			first_dist = dist;
			first = i;
		} 
		else if ( dist < second_dist )
		{
			second_dist = dist;
			second = i;
		}
	}
	return first != -1 && second != -1;
}

void edit_portal::delete_vertex_impl( u32 index )
{
	m_vertices->erase		( m_vertices->begin()		+ index );
	if ( !m_vertex_collisions->empty() )
	{
		vertex_collisions::iterator it = m_vertex_collisions->begin() + index;
		m_parent_object->get_model_editor()->collision_tree->erase( *it );
		DELETE ( *it );
		m_vertex_collisions->erase( it );
	}
	for ( index_triple_vector::iterator it = m_indices->begin(); it != m_indices->end(); ++it )
	{
		index_triple& current = *it;
		if ( current[0] > index )
			--current[0];
		if ( current[1] > index )
			--current[1];
		if ( current[2] > index )
			--current[2];
	}
	for ( vertex_collisions::iterator it = m_vertex_collisions->begin(); it != m_vertex_collisions->end(); ++it )
	{
		u32 i = ( *it )->get_index();
		if ( i > index )
			( *it )->set_index( --i );
	}
}

bool edit_portal::delete_unused_vertex( )
{
	for ( u32 i = 0; i < m_vertices->size(); ++i )
	{
		if ( std::find_if( m_indices->begin(), m_indices->end(), std::bind2nd( std::mem_fun_ref( &triple<u32>::contains), i ) ) != m_indices->end() )
			continue;
		delete_vertex_impl( i );
		return true;
	}
	return false;
}

void edit_portal::update_vertex_collisions( )
{
	for ( u32 i = 0; i < m_vertex_collisions->size(); ++i )
		this->update_vertex_collision( i );
}


sector_generator_portal edit_portal::to_sector_generator_portal ()
{
	sector_generator_portal::vertices_type vertices;
	vertices.reserve( m_vertices->size() );
	vertex_vector::const_iterator const vertices_end = m_vertices->end();
	for ( vertex_vector::const_iterator it = m_vertices->begin(); it != vertices_end; ++it )
	{
		vertices.push_back( m_model_to_world->transform_position( it->coord ) );
	}
	sector_generator_portal::indices_type indices;
	index_triple_vector::const_iterator index_end	= m_indices->end();
	for ( index_triple_vector::const_iterator it = m_indices->begin(); it != index_end; ++it )
	{
		indices.push_back( ( *it )[ 0 ] );
		indices.push_back( ( *it )[ 1 ] );
		indices.push_back( ( *it )[ 2 ] );
	}
	math::plane const& p = math::create_plane( vertices[ indices[ 0 ] ], vertices[ indices[ 1 ] ], vertices[ indices[ 2 ] ] );
	return sector_generator_portal( vertices, indices, p );
}

} // namespace model_editor
} // namespace xray
