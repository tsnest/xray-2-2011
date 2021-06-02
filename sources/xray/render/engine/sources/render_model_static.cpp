////////////////////////////////////////////////////////////////////////////
//	Created		: 13.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "render_model_static.h"
#include <xray/render/facade/vertex_input_type.h>
#include <xray/render/core/decl_utils.h>
#include <xray/render/engine/model_format.h>
#include <xray/render/core/res_declaration.h>
#include <xray/render/core/resource_manager.h>
#include "model_manager.h"
#include <xray/render/core/custom_config.h>
#include <xray/render/core/untyped_buffer.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>

#pragma warning( push )
#pragma warning( disable : 4995 )
#include <d3dx9mesh.h>
#pragma warning( pop )

namespace xray {
namespace render {

static_render_surface::static_render_surface( )
{
	m_vertex_input_type=static_mesh_vertex_input_type;
}

void static_render_surface::load( configs::binary_config_value const& properties, memory::chunk_reader& chunk)
{
	super::load					( properties, chunk);

	D3DVERTEXELEMENT9 const*  vFormat = NULL;

	memory::reader reader		= chunk.open_reader( model_chunk_vertices);
	vFormat						= ( D3DVERTEXELEMENT9*) reader.pointer();
	u32 const dcl_len			= D3DXGetDeclLength( vFormat) + 1;

	vector<D3D_INPUT_ELEMENT_DESC> decl_code;
	decl_utils::convert_vertex_declaration( vector<D3DVERTEXELEMENT9>( vFormat,vFormat+dcl_len), decl_code);

	ref_declaration decl		= resource_manager::ref().create_declaration( &*decl_code.begin(), decl_code.size());
	reader.advance				( dcl_len*sizeof( D3DVERTEXELEMENT9));

	// The visuals are create with dynamic usage temporary.
	bool dynamic		= true;

	m_render_geometry.vertex_count	= reader.r_u32				();
	u32 vStride						= D3DXGetDeclVertexSize		( vFormat,0);
	untyped_buffer_ptr vb			= resource_manager::ref().create_buffer( m_render_geometry.vertex_count*vStride, reader.pointer(), enum_buffer_type_vertex, dynamic);

//--- read indices here

	reader								= chunk.open_reader( model_chunk_indices);
	m_render_geometry.index_count		= reader.r_u32();
	m_render_geometry.primitive_count	= m_render_geometry.index_count/3;

	untyped_buffer_ptr ib						= resource_manager::ref().create_buffer( m_render_geometry.index_count*2, reader.pointer(), enum_buffer_type_index, false);

	m_render_geometry.geom				= resource_manager::ref().create_geometry( &*decl, vStride, *vb, *ib);
}

static_render_model::~static_render_model()
{
	children::iterator			it = m_children.begin();
	children::const_iterator	en = m_children.end();
	for( ; it != en; ++it)
		DELETE( *it );

	if(m_locators)
		FREE(m_locators);
}

void static_render_model::append_surface( render_surface* child )
{
	m_children.push_back(static_cast<static_render_surface*>(child));
}

static_render_model_instance::static_render_model_instance( )
:m_surface_instances( NULL ),
m_instances_count	( 0 )
{}

static_render_model_instance::~static_render_model_instance( )
{
	DELETE_ARRAY		( m_surface_instances );
}

void static_render_model_instance::assign_original( static_render_model_ptr v )
{
	m_original			= v;
	m_instances_count	= (u16)m_original->m_children.size();
	m_surface_instances = NEW_ARRAY( render_surface_instance, m_instances_count );

	for( u16 i = 0; i < m_instances_count; ++i )
	{
		render_surface_instance& info = m_surface_instances[i];
		info.m_parent				= this;
		info.m_render_surface		= m_original->m_children[i];
		info.m_transform			= &m_transform;
		info.m_visible				= true;
	}
}

void static_render_model_instance::get_surfaces( render_surface_instances& list, bool visible_only )
{
	list.reserve(list.size()+m_instances_count);

	for( u32 i = 0; i < m_instances_count; ++i )
	{
		render_surface_instance* inst = m_surface_instances+i;
		if( !visible_only || inst->m_visible )
			list.push_back		( inst );
	}
}

bool static_render_model_instance::get_locator( pcstr locator_name, model_locator_item& result ) const
{
	return m_original->get_locator( locator_name, result );
}

} // namespace render 
} // namespace xray 
