////////////////////////////////////////////////////////////////////////////
//	Created		: 19.11.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "render_particle_emitter_instance.h"
#include <xray/render/core/resource_manager.h>
#include <xray/render/core/backend.h>
#include "material_manager.h"
#include <xray/render/core/effect_options_descriptor.h>
#include <xray/render/facade/particles.h>
#include "system_renderer.h"
#include <xray/render/core/render_target.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/destroy_data_helper.h>
#include "statistics.h"

namespace xray {
namespace render {

#pragma pack( push,1)
struct particle_beamtrail_vertex
{
	void set(float3	in_position, float4 in_color, float2 in_uv)
	{
		position[0]		= in_position.x;
		position[1]		= in_position.y;
		position[2]		= in_position.z;
		color[0]		= in_color.x;
		color[1]		= in_color.y;
		color[2]		= in_color.z;
		color[3]		= in_color.w;
		uv[0]			= in_uv.x;
		uv[1]			= in_uv.y;
	}
	
#pragma message ( XRAY_TODO("is such order of members is intended? if doesn't matter make it perfect, please") )
	float	position[3];
	float	color[4];
	float	uv[2];
}; // struct particle_beamtrail_vertex

struct particle_sprite_vertex
{
	void set(float3	in_position, float4 in_color, float2 in_uv, float2 in_size, float in_rotation, float3 in_old_position)
	{
		position	 = in_position;
		color		 = in_color;
		uv			 = in_uv;
		size		 = in_size;
		rotation	 = in_rotation;
		old_position = in_old_position;
	}

	float3	position;		// 12
	float4	color;			// 4
	float2	uv;				// 8	// use index?
	float2	size;			// 8	// optimize size, short?
	float	rotation;		// 4	// short?
	float3	old_position;	// 12
	// Total				=  48+8	// 38
}; // struct particle_vertex

struct subuv_particle_sprite_vertex: public particle_sprite_vertex
{
	void set(float3	in_position, float4 in_color, float2 in_uv, float2 in_size, float in_rotation, float3 in_old_position, float2 in_size_uv, float4 in_blend_uv)
	{
		particle_sprite_vertex::set(in_position, in_color, in_uv, in_size, in_rotation, in_old_position);
		size_uv	 = in_size_uv;
		blend_uv = in_blend_uv;
	}
	float2	size_uv;
	float4  blend_uv;
}; // struct subuv_particle_vertex

#pragma pack( pop)

const D3D_INPUT_ELEMENT_DESC v_particle_sprite_fvf[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	1, DXGI_FORMAT_R32G32_FLOAT,		0, 28, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	2, DXGI_FORMAT_R32G32_FLOAT,		0, 36, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	3, DXGI_FORMAT_R32_FLOAT,			0, 44, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	4, DXGI_FORMAT_R32G32B32_FLOAT,		0, 48, 	D3D_INPUT_PER_VERTEX_DATA, 0},
};
const D3D_INPUT_ELEMENT_DESC v_subuv_particle_sprite_fvf[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	1, DXGI_FORMAT_R32G32_FLOAT,		0, 28, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	2, DXGI_FORMAT_R32G32_FLOAT,		0, 36, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	3, DXGI_FORMAT_R32_FLOAT,			0, 44, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	4, DXGI_FORMAT_R32G32B32_FLOAT,		0, 48, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	5, DXGI_FORMAT_R32G32_FLOAT,		0, 60, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	6, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 68, 	D3D_INPUT_PER_VERTEX_DATA, 0},
};
const D3D_INPUT_ELEMENT_DESC v_particle_beamtrail_fvf[] = 
{
	{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 12, 	D3D_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD",	1, DXGI_FORMAT_R32G32_FLOAT,		0, 28, 	D3D_INPUT_PER_VERTEX_DATA, 0},
};


render_particle_emitter_instance::render_particle_emitter_instance(
		particle::world& particle_world,
		particle::particle_emitter_instance& particle_emitter_instance,
		particle::particle_list_type const& particle_list,
		particle::billboard_parameters* billboard_parameters,
		particle::beamtrail_parameters* beamtrail_parameters,
		particle::enum_particle_locked_axis locked_axis,
		particle::enum_particle_screen_alignment screen_alignment,
		math::float4x4 const& transform,
		math::float4 const& instance_color
	) :
	m_max_particles		( 0 ),
	m_particle_world	( particle_world ),
	m_particle_emitter_instance	( particle_emitter_instance ),
	m_particle_list		( particle_list ),
	m_billboard_parameters	( billboard_parameters ),
	m_beamtrail_parameters	( beamtrail_parameters ),
	m_locked_axis		( locked_axis ),
	m_screen_alignment	( screen_alignment ),
	m_transform			( transform ),
	m_instance_color	( instance_color ),
	m_bbox				( math::create_zero_aabb() )
{
}

render_particle_emitter_instance::~render_particle_emitter_instance()
{
	if ( m_vertices.initialized() )
		XRAY_DESTROY_REFERENCE( m_vertices );
	
	if ( m_indices.initialized() )
		XRAY_DESTROY_REFERENCE( m_indices );
}


math::aabb const& render_particle_emitter_instance::get_aabb( ) const
{
	return m_bbox;
}

void render_particle_emitter_instance::update_render_buffers(
		particle::enum_particle_data_type datatype,
		bool use_subuv,
		u32 in_num_max_particles,
		u32 beamtrail_parameters_num_sheets
	)
{
	m_particle_sprite_geometry			= 0;
	m_subuv_particle_sprite_geometry	= 0;
	m_particle_beamtrail_geometry		= 0;
	m_num_vertices						= 0;
	m_num_indices						= 0;
	
	// TODO: One big buffer for all particles!
	m_max_particles						= xray::math::max<u32>(m_max_particles, in_num_max_particles);
	m_max_particles						= xray::math::min<u32>(m_max_particles, 2000);
	
	switch(datatype)
	{
		case xray::particle::particle_data_type_billboard:
		{
			if (use_subuv)
			{
				m_vertex_type = xray::particle::particle_vertex_type_billboard_subuv;
				m_num_vertices = sizeof(subuv_particle_sprite_vertex) * m_max_particles * 4;
				m_num_indices  = sizeof(subuv_particle_sprite_vertex) * m_max_particles * 6;
				// we do not need to do this at all!
				// we should use single buffer for all the particles
				if ( m_vertices.initialized() )
					XRAY_DESTROY_REFERENCE( m_vertices );
				XRAY_CONSTRUCT_REFERENCE( m_vertices, vertex_buffer ) ( m_num_vertices );

				if ( m_indices.initialized() )
					XRAY_DESTROY_REFERENCE( m_indices );
				XRAY_CONSTRUCT_REFERENCE( m_indices, index_buffer ) ( m_num_indices );

				m_subuv_particle_sprite_geometry	= resource_manager::ref().create_geometry( v_subuv_particle_sprite_fvf, sizeof(subuv_particle_sprite_vertex), m_vertices->buffer(), m_indices->buffer());
 				m_particle_sprite_geometry			= 0;
 				m_particle_beamtrail_geometry		= 0;
			}
			else
			{
				m_vertex_type = xray::particle::particle_vertex_type_billboard;
				m_num_vertices = sizeof(subuv_particle_sprite_vertex) * m_max_particles * 4;
				m_num_indices  = sizeof(subuv_particle_sprite_vertex) * m_max_particles * 6;
				// we do not need to do this at all!
				// we should use single buffer for all the particles
				if ( m_vertices.initialized() )
					XRAY_DESTROY_REFERENCE( m_vertices );
				XRAY_CONSTRUCT_REFERENCE( m_vertices, vertex_buffer ) ( m_num_vertices );

				if ( m_indices.initialized() )
					XRAY_DESTROY_REFERENCE( m_indices );
				XRAY_CONSTRUCT_REFERENCE( m_indices, index_buffer ) ( m_num_indices );

				m_particle_sprite_geometry			= resource_manager::ref().create_geometry( v_particle_sprite_fvf, sizeof(particle_sprite_vertex), m_vertices->buffer(), m_indices->buffer());
 				m_subuv_particle_sprite_geometry	= 0;
 				m_particle_beamtrail_geometry		= 0;
			}
			break;
		}
		case xray::particle::particle_data_type_trail:
		case xray::particle::particle_data_type_beam:
			{
				if (datatype==xray::particle::particle_data_type_trail)
					m_vertex_type = xray::particle::particle_vertex_type_trail;
				else
					m_vertex_type = xray::particle::particle_vertex_type_beam;
				
				u32 num_sheets = xray::math::max<u32>(beamtrail_parameters_num_sheets, 1);
				num_sheets = xray::math::min<u32>(num_sheets, 10000);
				
				m_num_vertices = sizeof(subuv_particle_sprite_vertex) * m_max_particles * 2 * num_sheets;
				m_num_indices  = sizeof(subuv_particle_sprite_vertex) * m_max_particles * 6 * num_sheets;
				// we do not need to do this at all!
				// we should use single buffer for all the particles
				if ( m_vertices.initialized() )
					XRAY_DESTROY_REFERENCE( m_vertices );
				XRAY_CONSTRUCT_REFERENCE( m_vertices, vertex_buffer ) ( m_num_vertices );

				if ( m_indices.initialized() )
					XRAY_DESTROY_REFERENCE( m_indices );
				XRAY_CONSTRUCT_REFERENCE( m_indices, index_buffer ) ( m_num_indices );

				m_particle_beamtrail_geometry		= resource_manager::ref().create_geometry( v_particle_beamtrail_fvf, sizeof(particle_beamtrail_vertex), m_vertices->buffer(), m_indices->buffer());	
 				m_particle_sprite_geometry			= 0;
 				m_subuv_particle_sprite_geometry	= 0;
				break;
			}
		case xray::particle::particle_data_type_decal:
			{
				m_vertex_type = xray::particle::particle_vertex_type_decal;
				break;
			}
		default:
			{
				m_vertex_type = xray::particle::particle_vertex_type_unknown;
				break;
			}
	};
}

u32 render_particle_emitter_instance::get_num_particles() const
{
	particle::base_particle* P = m_particle_list.front();
	
	u32 num_particles = 0;
	while (P)
	{
		P = m_particle_list.get_next_of_object(P);
		num_particles++;
	}
	
	return num_particles;
}

bool render_particle_emitter_instance::is_sprite_based_geometry() const
{
	switch (m_vertex_type)
	{
		case xray::particle::particle_vertex_type_billboard:
		case xray::particle::particle_vertex_type_billboard_subuv:
		case xray::particle::particle_vertex_type_trail:
		case xray::particle::particle_vertex_type_beam:
			return true;
	}
	return false;
}

void render_particle_emitter_instance::render_sprites()
{
	u32 num_particles = get_num_particles();
	
	if (!num_particles)
		return;
	
	u32 i_offset = 0, v_offset = 0;
	
	u32 const num_vertices_per_particle = 4;
	u32 const num_incides_per_particle = 6;
	
	u16*			 pib = m_indices->lock( num_particles * num_incides_per_particle, i_offset);
	
	particle_sprite_vertex* pv		= (particle_sprite_vertex*)m_vertices->lock( num_particles * num_vertices_per_particle, sizeof(particle_sprite_vertex), v_offset);
	
	u32 p = 0;
	xray::particle::base_particle* P = m_particle_list.front();
	while (P)
	{
		u32 v					= p * 4;
		u16 _v					= static_cast_checked<u16>(v);
		
		*pib++					= _v+0;
		*pib++					= _v+3;
		*pib++					= _v+2;
		*pib++					= _v+0;
		*pib++					= _v+2;
		*pib++					= _v+1;
		
		float4 final_color		= float4(
			P->color.x * m_instance_color.x, 
			P->color.y * m_instance_color.y, 
			P->color.z * m_instance_color.z, 
			P->color.w * m_instance_color.w
		);
		
		float2 size = float2(P->size.x, P->size.y);
		if (m_screen_alignment==particle::particle_screen_alignment_square)
			size.y = size.x;
		
		pv->set(P->position, final_color, float2(0.f, 0.f), size, P->rotation, P->old_position);	pv++;
		pv->set(P->position, final_color, float2(0.f, 1.f), size, P->rotation, P->old_position);	pv++;
		pv->set(P->position, final_color, float2(1.f, 1.f), size, P->rotation, P->old_position);	pv++;
		pv->set(P->position, final_color, float2(1.f, 0.f), size, P->rotation, P->old_position);	pv++;
		
		p++;
		P = m_particle_list.get_next_of_object(P);
	}
	
	m_vertices->unlock();
	m_indices->unlock();
	
	m_particle_sprite_geometry->apply();
	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, num_incides_per_particle * num_particles, i_offset, v_offset);
	statistics::ref().visibility_stat_group.num_triangles.value += num_incides_per_particle * num_particles / 3;
}

static float frac(float f)
{
	return f - (u32)f;
}

void render_particle_emitter_instance::render_subuv_sprites()
 {
	u32 num_particles = get_num_particles();
	
	if (!num_particles)
		return;
	
	u32 i_offset = 0, v_offset = 0;
	
	u32 const num_vertices_per_particle = 4;
	u32 const num_incides_per_particle = 6;
	
	u16*			 pib = m_indices->lock( num_particles * num_incides_per_particle, i_offset);
	
	subuv_particle_sprite_vertex* pv	= (subuv_particle_sprite_vertex*)m_vertices->lock( num_particles * num_vertices_per_particle, sizeof(subuv_particle_sprite_vertex), v_offset);
	
	u32 p = 0;
	xray::particle::base_particle* P = m_particle_list.front();
	while (P)
	{
		float subimage_index = P->subimage_index;//lifetime * m_billboard_parameters->sub_image_horizontal * m_billboard_parameters->sub_image_vertical * m_billboard_parameters->sub_image_change_speed;
		
		float next_subimage_index = P->next_subimage_index;//subimage_index;
		
		//if (m_billboard_parameters->subuv_method == particle::particle_subuv_method_linear_smooth ||
		//	m_billboard_parameters->subuv_method == particle::particle_subuv_method_random_smooth)
		//		next_subimage_index += 1.0f;
		
		float posU = 0.0f;
		float posV = 0.0f;
		
		float next_posU = 0.0f;
		float next_posV = 0.0f;
		
		float sizeU = 1.0f;
		float sizeV = 1.0f;
		
		if (m_billboard_parameters->sub_image_horizontal && m_billboard_parameters->sub_image_horizontal)
		{
			posU = float(u32(subimage_index) % m_billboard_parameters->sub_image_horizontal);
			posV = float(u32(subimage_index) / m_billboard_parameters->sub_image_horizontal);
			
			next_posU = float(u32(next_subimage_index) % m_billboard_parameters->sub_image_horizontal);
			next_posV = float(u32(next_subimage_index) / m_billboard_parameters->sub_image_horizontal);
			
			sizeU = 1.0f / float(m_billboard_parameters->sub_image_horizontal);
			sizeV = 1.0f / float(m_billboard_parameters->sub_image_vertical);
		}
		
		float u_offset			= frac(subimage_index);
		
		if (m_billboard_parameters->subuv_method == particle::particle_subuv_method_random_smooth)
		{
			float const f_time = P->get_linear_lifetime() * m_billboard_parameters->sub_image_changes;
			float const i_time = float(math::floor(f_time));
			
			u_offset			= f_time - i_time;
		}
		
		u32 v					= p * 4;
		u16 _v					= static_cast_checked<u16>(v);
		
		*pib++					= _v+0;
		*pib++					= _v+3;
		*pib++					= _v+2;
		*pib++					= _v+0;
		*pib++					= _v+2;
		*pib++					= _v+1;
		
		float4 final_color		= float4(
			P->color.x * m_instance_color.x, 
			P->color.y * m_instance_color.y, 
			P->color.z * m_instance_color.z, 
			P->color.w * m_instance_color.w
		);
		
		float2 size = float2(P->size.x, P->size.y);
		if (m_screen_alignment==particle::particle_screen_alignment_square)
			size.y = size.x;
		
		pv->set(P->position, final_color, float2(sizeU*(posU+0), sizeV*(posV+0)), size, P->rotation, P->old_position, float2(0,0), float4(u_offset, sizeU*(next_posU+0), sizeV*(next_posV+0), 0)); pv++;
		pv->set(P->position, final_color, float2(sizeU*(posU+0), sizeV*(posV+1)), size, P->rotation, P->old_position, float2(0,1), float4(u_offset, sizeU*(next_posU+0), sizeV*(next_posV+1), 0)); pv++;
		pv->set(P->position, final_color, float2(sizeU*(posU+1), sizeV*(posV+1)), size, P->rotation, P->old_position, float2(1,1), float4(u_offset, sizeU*(next_posU+1), sizeV*(next_posV+1), 0)); pv++;
		pv->set(P->position, final_color, float2(sizeU*(posU+1), sizeV*(posV+0)), size, P->rotation, P->old_position, float2(1,0), float4(u_offset, sizeU*(next_posU+1), sizeV*(next_posV+0), 0)); pv++;
		
		p++;
		P = m_particle_list.get_next_of_object(P);
	}
	
	m_vertices->unlock();
	m_indices->unlock();
	
	m_subuv_particle_sprite_geometry->apply();
	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, num_incides_per_particle * num_particles, i_offset, v_offset);
	statistics::ref().visibility_stat_group.num_triangles.value += num_incides_per_particle * num_particles / 3;
}

void render_particle_emitter_instance::render(xray::math::float3 const& view_location, u32 const num_particles)
{
	switch (m_vertex_type)
	{
		case particle::particle_vertex_type_billboard:
			{
				BEGIN_CPUGPU_TIMER(statistics::ref().particles_stat_group.sprites_execute_time);
				render_sprites();
				END_CPUGPU_TIMER;
			}
		break;
		
		case particle::particle_vertex_type_billboard_subuv:
			{
				BEGIN_CPUGPU_TIMER(statistics::ref().particles_stat_group.sprites_execute_time);
				render_subuv_sprites();
				END_CPUGPU_TIMER;
			}
		break;

		case particle::particle_vertex_type_trail:
			{
				BEGIN_CPUGPU_TIMER(statistics::ref().particles_stat_group.beamtrails_execute_time);
				render_trails(view_location, m_particle_list.front(), num_particles);
				END_CPUGPU_TIMER;
			}
		break;
		
		case particle::particle_vertex_type_beam:
			{
				BEGIN_CPUGPU_TIMER(statistics::ref().particles_stat_group.beamtrails_execute_time);
				render_beams(view_location, num_particles);
				END_CPUGPU_TIMER;
			}
		break;
	};
}

void render_particle_emitter_instance::render_beams(xray::math::float3 const& view_location, u32 num_particles)
{
	u32 const num_particle_per_beam = num_particles / m_beamtrail_parameters->num_beams;
	
	for (u32 beam_index=0; beam_index<m_beamtrail_parameters->num_beams; beam_index++)
	{
		particle::base_particle* P = m_particle_list.front();
		u32 particle_index = 0;
		while (P)
		{
			if (particle_index == beam_index * num_particle_per_beam)
				break;

			particle_index++;
			P = m_particle_list.get_next_of_object(P);
		}
		render_trails(view_location, P, num_particle_per_beam);
	}
}

void render_particle_emitter_instance::render_trails(xray::math::float3 const& view_location, xray::particle::base_particle* start_particle, u32 num_particles)
{
	if (!m_beamtrail_parameters)
		return;
	
	//u32 num_particles = get_num_particles();
	
	if (!num_particles)
		return;
	
	u32 const num_sheets					= xray::math::max<u32>(m_beamtrail_parameters->num_sheets, 1);
	u32 const num_vertices_per_particle		= 2;
	u32 const num_indices_per_particle		= 6;
	u32 const texture_tiles					= xray::math::max<u32>(m_beamtrail_parameters->num_texture_tiles, 1);
	u32 const num_quads						= num_particles - 1;
	
	u32 i_offset = 0,
		v_offset = 0;
	
	u16* pib						= m_indices->lock( num_sheets * num_indices_per_particle * num_particles, i_offset);
	particle_beamtrail_vertex* pv	= (particle_beamtrail_vertex*)m_vertices->lock( num_sheets * num_vertices_per_particle * num_particles, sizeof(particle_beamtrail_vertex), v_offset);
	
	float distance_from_first_to_last = 0.0f;
	
	xray::particle::base_particle* P = start_particle;
	for (u32 p=0; p<num_quads; p++)
	{
		xray::particle::base_particle* nextP = m_particle_list.get_next_of_object(P);
		float s = 1.0f;
		
		if (p==num_quads-1)
		{
			//s = (1.0f - nextP->lifetime);
		}
		distance_from_first_to_last += s * float3(nextP->position-P->position).squared_length();
		P = m_particle_list.get_next_of_object(P);
	}
	
	
	float current_angle = 0.0f;
	float const angle_incrase = math::pi / float(num_sheets);
	
	for (u32 s=0; s<num_sheets; s++)
	{
		float current_uv_start = 0.0f;
		
		xray::particle::base_particle* P = start_particle;
		xray::particle::base_particle* prevP = 0;
		float3 prev_particle_dir;
		
		for (u32 p=0; p<num_quads; p++)
		{	
			xray::particle::base_particle* nextP = m_particle_list.get_next_of_object(P);
			
			float s = 1.0f;
			
			if (p==num_quads-1)
			{
				//s = (1.0f - nextP->lifetime);
			}
			float const distance_from_current_to_next = s * float3(nextP->position-P->position).squared_length();
			
			float const scaleUV		= distance_from_current_to_next / distance_from_first_to_last;
			
			float3 view_dir  	 	= math::normalize_safe(view_location - P->position);
			float3 particle_dir 	= math::normalize_safe(nextP->position - P->position);
			
			if (p!=0)
			{
				particle_dir 		= math::normalize_safe(nextP->position - prevP->position);
			}
			
			float4 final_color		= float4(P->color.x * m_instance_color.x, P->color.y * m_instance_color.y, P->color.z * m_instance_color.z, P->color.w * m_instance_color.w);
			
			float const current_v_pos = current_uv_start * float(texture_tiles);
			
			float3	right_a			= math::normalize_safe(math::cross_product(view_dir, particle_dir));
			
			if (s!=0)
			{
				if (num_particles!=2 && p == num_particles - 2)
				{
					particle_dir = prev_particle_dir;
				}
				float4x4 rot_matrix		= create_rotation(particle_dir.normalize_safe(particle_dir), current_angle);
				right_a					= rot_matrix.transform_position(right_a);
			}
			
			pv->set(P->position + right_a * P->size.y, final_color, float2(0,current_v_pos)); pv++;
			pv->set(P->position - right_a * P->size.y, final_color, float2(1,current_v_pos)); pv++;
			
			if (num_particles==2 || p == num_particles - 2)
			{
				current_uv_start += scaleUV;
				pv->set(nextP->position + right_a * P->size.y, final_color, float2(0,current_uv_start * float(texture_tiles))); pv++;
				pv->set(nextP->position - right_a * P->size.y, final_color, float2(1,current_uv_start * float(texture_tiles))); pv++;
			}
			
			current_uv_start += scaleUV;
			prev_particle_dir = particle_dir;
			prevP = P;
			P = m_particle_list.get_next_of_object(P);
		}
		current_angle += angle_incrase;
	}
	
	for (u16 s=0; s<num_sheets; s++)
	{
		for (u32 p=0; p<num_quads; p++)
		{
			u16 v					= static_cast_checked<u16>(num_particles * num_vertices_per_particle * s + p * 2);
			
			*pib++					= v+0;
			*pib++					= v+1;
			*pib++					= v+3;
			*pib++					= v+0;
			*pib++					= v+3;
			*pib++					= v+2;
		}
	}
	
	m_vertices->unlock();
	m_indices->unlock();
	
	m_particle_beamtrail_geometry->apply();
	
	backend::ref().render_indexed( D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, num_sheets * num_indices_per_particle * num_quads, i_offset, v_offset);
	statistics::ref().visibility_stat_group.num_triangles.value += num_sheets * num_indices_per_particle * num_quads / 3;
}

void render_particle_emitter_instance::render_meshes()
{
}

xray::render::enum_vertex_input_type render_particle_emitter_instance::get_vertex_input_type( )
{
	switch (m_vertex_type)
	{
		case particle::particle_vertex_type_billboard: 
			return particle_vertex_input_type; 
		case particle::particle_vertex_type_billboard_subuv: 
			return particle_subuv_vertex_input_type; 
		case particle::particle_vertex_type_beam : 
		case particle::particle_vertex_type_trail : 
			return particle_beamtrail_vertex_input_type; 
	}
	return null_vertex_input_type;
}

material_effects& render_particle_emitter_instance::get_material_effects()
{
	// TODO: fix it!!!
	if (!m_material_effects_ptr.c_ptr())
	{
		return material::nomaterial_material( get_vertex_input_type() );
	}
	else
	{
		return m_material_effects_ptr->get_material_effects();
	}
}

void render_particle_emitter_instance::change_material	( resources::unmanaged_resource_ptr const& material )
{
//	enum_vertex_input_type	vertex_input_type = null_vertex_input_type;
// 	switch (m_vertex_type)
// 	{
// 		case particle::particle_vertex_type_billboard: 
// 			vertex_input_type = particle_vertex_input_type; 
// 			break;
// 		case particle::particle_vertex_type_billboard_subuv: 
// 			vertex_input_type = particle_subuv_vertex_input_type; 
// 			break;
// 		case particle::particle_vertex_type_beam : 
// 		case particle::particle_vertex_type_trail : 
// 			vertex_input_type = particle_beamtrail_vertex_input_type; 
// 			break;
// 	}
	// TODO: particle decal!
	m_material_effects_ptr	= xray::static_cast_resource_ptr<material_effects_instance_ptr>(material);
	//material_manager::ref().initialize_material_effects	( m_material_effects, m_material, vertex_input_type, false );
}

void render_particle_emitter_instance::set_transform				(
		math::float4x4 const& transform
	)
{
	m_transform				= transform;
}

void render_particle_emitter_instance::set_aabb( math::aabb const& bbox )
{
	m_bbox					= bbox;
}

void render_particle_emitter_instance::draw_debug(xray::math::float4x4 const& view_matrix, xray::particle::enum_particle_render_mode debug_mode)
{
	xray::math::float4x4 camera_to_world;
	camera_to_world.try_invert(view_matrix);
	
	xray::math::float3 up_vector	 = camera_to_world.transform_direction(float3(0,1000,0)).normalize();
	xray::math::float3 right_vector  = camera_to_world.transform_direction(float3(1000,0,0)).normalize();
	
	xray::math::float3 view_location = camera_to_world.c.xyz();
	
	switch (debug_mode)
	{
		case particle::dots_particle_render_mode:
		{
			particle::base_particle* P = m_particle_list.front();
			while (P)
			{
				system_renderer::ref().draw_3D_point(P->position, 3, xray::math::color(1.0f,1.0f,0.0f,1.0f), false);
				P = m_particle_list.get_next_of_object(P);
			}
		}
		break;
		
		case particle::size_particle_render_mode:
		{
			particle::base_particle* P = m_particle_list.front();
			while (P)
			{
				xray::math::float3 points[2];
				
				points[0] = xray::math::float3(P->position - right_vector * P->size.x * 0.5f);
				points[1] = xray::math::float3(P->position + right_vector * P->size.x * 0.5f);
				
				system_renderer::ref().draw_screen_lines(points, sizeof(points)/sizeof(points[0]), xray::math::color(1.0f,1.0f,0.0f,1.0f), 1, 0xffffffff, false, false);
				
				points[0] = xray::math::float3(P->position - up_vector * P->size.y * 0.5f);
				points[1] = xray::math::float3(P->position + up_vector * P->size.y * 0.5f);
				
				system_renderer::ref().draw_screen_lines(points, sizeof(points)/sizeof(points[0]), xray::math::color(1.0f,1.0f,0.0f,1.0f), 1, 0xffffffff, false, false);
				
				P = m_particle_list.get_next_of_object(P);
			}
		}
		break;
	}
}

pcstr render_particle_emitter_instance::material_name	( ) const
{
	return m_material_effects_ptr->get_material_name().c_str();
}

} // namespace render
} // namespace xray
