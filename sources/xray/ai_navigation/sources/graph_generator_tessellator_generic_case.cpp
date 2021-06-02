////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_tessellator.h"

using xray::ai::navigation::graph_generator_tessellator;
using xray::ai::navigation::triangles_mesh_type;
using xray::math::float3;

bool graph_generator_tessellator::subdivide_triangle ( 
		u32 const triangle_id, 
		float3 (&segment_vertices)[2]
	)
{
	u32 const* indices = &*m_triangles_mesh.indices.begin() + 3 * triangle_id;

	float3 const triangle_vertices_original[] = {
		m_triangles_mesh.vertices[ indices[0] ],
		m_triangles_mesh.vertices[ indices[1] ],
		m_triangles_mesh.vertices[ indices[2] ],
	};

	float3 const* triangle_vertices[]	= {
		&triangle_vertices_original[0],
		&triangle_vertices_original[1],
		&triangle_vertices_original[2],
	};

	float3 const& triangle_normal	= m_triangles_mesh.data[triangle_id].plane.normal;

	u32 success_count = 0;
	float3 const* last_vertex_on_line_with_segment = 0;
	for (u32 iteration = 0; ; ++iteration ) {
		float3 const& s0_minus_t0 = segment_vertices[0] - *triangle_vertices[0];
		float3 const& s1_minus_t0 = segment_vertices[1] - *triangle_vertices[0];
		float3 s0_cross_s1_t0	= s0_minus_t0 ^ s1_minus_t0;
		if ( xray::math::is_zero( s0_cross_s1_t0.length() ) ) {
			if ( !last_vertex_on_line_with_segment ) {
				last_vertex_on_line_with_segment	= triangle_vertices[0];

				R_ASSERT_CMP		( iteration, <, 1 );

				R_ASSERT			( triangle_vertices[0] == &triangle_vertices_original[0] );
				R_ASSERT			( triangle_vertices[1] == &triangle_vertices_original[1] );
				R_ASSERT			( triangle_vertices[2] == &triangle_vertices_original[2] );

				triangle_vertices[0] = &triangle_vertices_original[1];
				triangle_vertices[1] = &triangle_vertices_original[2];
				triangle_vertices[2] = &triangle_vertices_original[0];
				continue;
			}

			// segment lies completely on the edge
			// here we make this edge TV0 => TV1
			// and S0 will be closer to TV0 than S1
			if ( triangle_vertices[1] != last_vertex_on_line_with_segment ) {
				R_ASSERT			( triangle_vertices[0] != last_vertex_on_line_with_segment );
				R_ASSERT			( triangle_vertices[2] == last_vertex_on_line_with_segment );

				last_vertex_on_line_with_segment	= triangle_vertices[0];

				std::swap			( triangle_vertices[2], triangle_vertices[0] );
				std::swap			( triangle_vertices[2], triangle_vertices[1] );

				if ( s0_minus_t0.squared_length() < s1_minus_t0.squared_length() )
					std::swap		( segment_vertices[0], segment_vertices[1] );
			}
			else {
				if ( s0_minus_t0.squared_length() > s1_minus_t0.squared_length() )
					std::swap		( segment_vertices[0], segment_vertices[1] );
			}

			float3 const triangle_vertices_values[]	= {
				*triangle_vertices[0],
				*triangle_vertices[1],
				*triangle_vertices[2],
			};

			success_count			+= add_output_triangle( triangle_id, triangle_vertices_values[0],segment_vertices[0],			triangle_vertices_values[2])	? 1 : 0;
			success_count			+= add_output_triangle( triangle_id, segment_vertices[0],		segment_vertices[1],			triangle_vertices_values[2])	? 1 : 0;
			success_count			+= add_output_triangle( triangle_id, segment_vertices[1],		triangle_vertices_values[1],	triangle_vertices_values[2])	? 1 : 0;
			R_ASSERT( success_count > 0 );
			return true;
		}

		s0_cross_s1_t0.normalize( );
		bool const is_t0		= math::is_zero( (triangle_normal | s0_cross_s1_t0) - 1.f, math::epsilon_3);
		R_ASSERT				( is_t0 || math::is_zero( (triangle_normal | -s0_cross_s1_t0) - 1.f, math::epsilon_3) );
		if ( !is_t0 )
			std::swap			( segment_vertices[0], segment_vertices[1] );

		float3 const& s0_minus_t1 = segment_vertices[0] - *triangle_vertices[1];
		float3 const& s1_minus_t1 = segment_vertices[1] - *triangle_vertices[1];
		float3 s0_cross_s1_t1	 = s0_minus_t1 ^ s1_minus_t1;
		if ( math::is_zero( s0_cross_s1_t1.length() ) ) {
			if ( !last_vertex_on_line_with_segment ) {
				last_vertex_on_line_with_segment	= triangle_vertices[1];

				R_ASSERT_CMP		( iteration, <, 1 );

				R_ASSERT			( triangle_vertices[0] == &triangle_vertices_original[0] );
				R_ASSERT			( triangle_vertices[1] == &triangle_vertices_original[1] );
				R_ASSERT			( triangle_vertices[2] == &triangle_vertices_original[2] );

				triangle_vertices[0] = &triangle_vertices_original[2];
				triangle_vertices[1] = &triangle_vertices_original[0];
				triangle_vertices[2] = &triangle_vertices_original[1];

				continue;
			}

			// segment lies completely on the edge
			// here we make this edge TV0 => TV1
			// and S0 will be closer to TV0 than S1
			R_ASSERT				( triangle_vertices[0] != last_vertex_on_line_with_segment );
			R_ASSERT				( triangle_vertices[2] == last_vertex_on_line_with_segment );

			if ( s0_minus_t1.squared_length() > s1_minus_t1.squared_length() )
				std::swap			( segment_vertices[0], segment_vertices[1] );

			float3 const triangle_vertices_values[]	= {
				*triangle_vertices[0],
				*triangle_vertices[1],
				*triangle_vertices[2],
			};

			success_count			+= add_output_triangle( triangle_id, triangle_vertices_values[1],segment_vertices[0],			triangle_vertices_values[0])	? 1 : 0;
			success_count			+= add_output_triangle( triangle_id, segment_vertices[0],		segment_vertices[1],			triangle_vertices_values[0])	? 1 : 0;
			success_count			+= add_output_triangle( triangle_id, segment_vertices[1],		triangle_vertices_values[2],	triangle_vertices_values[0])	? 1 : 0;
			R_ASSERT( success_count > 0 );
			return true;
		}

		s0_cross_s1_t1.normalize( );
		bool const is_t1		= math::is_zero( (triangle_normal | s0_cross_s1_t1) - 1.f, math::epsilon_3);
		R_ASSERT				( is_t1 || math::is_zero( (triangle_normal | -s0_cross_s1_t1) - 1.f, math::epsilon_3) );
		if ( is_t1 ) {
			triangle_vertices[0]			= &triangle_vertices_original[1];
			triangle_vertices[1]			= &triangle_vertices_original[2];
			triangle_vertices[2]			= &triangle_vertices_original[0];
		}
		else {
			triangle_vertices[0]			= &triangle_vertices_original[0];
			triangle_vertices[1]			= &triangle_vertices_original[1];
			triangle_vertices[2]			= &triangle_vertices_original[2];
		}

		R_ASSERT_CMP			( iteration, <, 4 );

		float3 const triangle_vertices_values[]	= {
			*triangle_vertices[0],
			*triangle_vertices[1],
			*triangle_vertices[2],
		};

		success_count			+= add_output_triangle( triangle_id, triangle_vertices_values[0],	triangle_vertices_values[1],	segment_vertices[0] )	? 1 : 0;
		success_count			+= add_output_triangle( triangle_id, triangle_vertices_values[1],	segment_vertices[1],			segment_vertices[0] )	? 1 : 0;
		success_count			+= add_output_triangle( triangle_id, triangle_vertices_values[1],	triangle_vertices_values[2],	segment_vertices[1] )	? 1 : 0;
		success_count			+= add_output_triangle( triangle_id, triangle_vertices_values[2],	triangle_vertices_values[0],	segment_vertices[1] )	? 1 : 0;
		success_count			+= add_output_triangle( triangle_id, triangle_vertices_values[0],	segment_vertices[0],			segment_vertices[1] )	? 1 : 0;
		
		R_ASSERT( success_count > 0 );
		return true;
	}
}
