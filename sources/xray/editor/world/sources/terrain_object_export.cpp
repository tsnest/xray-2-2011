////////////////////////////////////////////////////////////////////////////
//	Created		: 29.01.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_object.h"
#include "terrain_object.h"
#include <d3d9types.h>
#include <FreeImage/FreeImage.h>

namespace xray {
namespace editor {

extern bool g_block_visual_loading;

void vertices_db::export_vertices( math::rectangle<math::uint2> const& rect, 
								  vectora<render::terrain_data>& dest_buffer )
{
	u32 size					= rect.width() * rect.height();
	dest_buffer.resize			( size );

	u32 current=0;
	for(u32 y=rect.min.y; y<rect.max.y; ++y)
		for(u32 x=rect.min.x; x<rect.max.x; ++x)
		{
			u16	vidx				= collision::terrain_vertex_id(m_dimension+1, x, y);
			terrain_vertex% v		= m_vertices[vidx];
			render::terrain_data& data = dest_buffer[current];

			data.height				= v.height;
			data.tex_id0			= v.t0_index;
			data.tex_id1			= v.t1_index;
			data.tex_id2			= v.t2_index;

			data.alpha0				= v.t0_infl;
			data.alpha1				= v.t1_infl;
			data.alpha2				= v.t2_infl;
			data.color				= v.vertex_color;

			data.tex_shift			= float2( v.tc_shift_x, v.tc_shift_z);
			++current;
		}
}

void terrain_core::do_export( terrain_import_export_settings^ settings )
{
	int const dim			= 65;
	u32 const bitmap_width		= settings->size_x*(dim-1) + 1;
	u32 const bitmap_height		= settings->size_z*(dim-1) + 1;

	u32 const bitmap_size = bitmap_width*bitmap_height;
	FIBITMAP* fb			= FreeImage_AllocateT( FIT_FLOAT, bitmap_width, bitmap_height, 16 );
	float* src_bits			= (float*)FreeImage_GetBits(fb);

	float min_height = math::float_max;
	float max_height = math::float_min;
	
	for(int ix = 0; ix<settings->size_x; ++ix)
		for(int iz = 0; iz<settings->size_z; ++iz)
		{
			terrain_node_key	key(settings->lt_x+ix, settings->lt_z-iz);
			
			u32 start_lt = iz*( (dim-1)*bitmap_width) + ix*(dim-1);

			if(!m_nodes.ContainsKey(key))
			{
				// fill default points
				for(u32 _x=0;_x<dim;++_x)
					for(u32 _y=0;_y<dim;++_y)
					{
						u32 const offset	= (start_lt + bitmap_width*_y) +_x;
						R_ASSERT			(offset<bitmap_size);
						float* pixel_ptr	= src_bits + offset;
						float h				= 0.5f;
						*pixel_ptr			= h;

						min_height			= math::min( min_height, h );
						max_height			= math::max( max_height, h );
					}
			}else
			{
				terrain_node^	terrain		= m_nodes[key];
				bool terrain_was_loaded		= !terrain->m_vertices.empty();

				// force vertices load
				if(!terrain_was_loaded)
				{
					g_block_visual_loading	= true;
					terrain->load_contents	( );
				}

				// sync wait vertices
				while(terrain->m_vertices.empty())
				{
					resources::dispatch_callbacks	( );
					threading::yield				( 10 );
					LOG_ERROR("waiting for terrain node vertices");
				}

				for(int _y=0; _y<dim; ++_y)
					for(int _x=0; _x<dim; ++_x)
				{
					u32 idx				= terrain->vertex_id(_x, _y);
					terrain_vertex v	= terrain->m_vertices.get(idx);

					u32 const offset	= (start_lt + bitmap_width*_y) +_x;
					R_ASSERT			(offset<bitmap_size);
					float* pixel_ptr	= src_bits + offset;
					*pixel_ptr			= v.height;

					min_height			= math::min( min_height, v.height );
					max_height			= math::max( max_height, v.height );
				}

				if(!terrain_was_loaded)
				{
					terrain->unload_contents( false );
					g_block_visual_loading	= false;
				}
			}
		}
	// normalize

	float const k = max_height - min_height;
	for(u32 i =0; i<bitmap_size; ++i)
	{
		float* pixel_ptr	= src_bits + i;
		*pixel_ptr			= (*pixel_ptr-min_height) / k;
	}

	FreeImage_FlipVertical	( fb );

	//save
	System::String^ filename = System::String::Format("{0}\\{1}_min({2})_max({3}).tiff", 
														System::IO::Path::GetDirectoryName(settings->m_source_filename),
														System::IO::Path::GetFileNameWithoutExtension(settings->m_source_filename),
														min_height, 
														max_height );
	unmanaged_string fn		(filename);
	FREE_IMAGE_FORMAT	fif = FIF_TIFF;
	FreeImage_Save			( fif, fb, fn.c_str(), 0 );
	FreeImage_Unload		( fb );
}




} // namespace editor
} // namespace xray