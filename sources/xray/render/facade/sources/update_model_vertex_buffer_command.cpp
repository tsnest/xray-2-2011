////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/world.h>
#include <xray/render/engine/world.h>
#include <xray/memory_macros.h>
#include "update_model_vertex_buffer_command.h"

namespace xray {
namespace render {

update_model_vertex_buffer_command::update_model_vertex_buffer_command(
		engine::world& world, 
		render_model_instance_ptr object, 
		vectora< buffer_fragment > const& fragments, 
		memory::base_allocator& allocator
	) :
	m_world				( world ),
	m_object			( object ),
	m_allocator			( allocator ),
	m_fragments			( allocator )
{
	ASSERT				( m_object );
	m_fragments.resize	( fragments.size() );
	Fragments::iterator			it	= m_fragments.begin();
	Fragments::const_iterator	end	= m_fragments.end();

	Fragments::const_iterator it_o	= fragments.begin();

	for( ; it != end; ++it, ++it_o)
	{
		*it = *it_o;
		it->buffer = XRAY_NEW_ARRAY_IMPL( m_allocator, char, it_o->size);
		memory::copy( it->buffer, it_o->size, it_o->buffer, it_o->size);
	}
}

update_model_vertex_buffer_command::~update_model_vertex_buffer_command	()
{
	Fragments::iterator			it	= m_fragments.begin();
	Fragments::const_iterator	end	= m_fragments.end();

	for( ; it != end; ++it)
	{
		XRAY_DELETE_ARRAY_IMPL( m_allocator, it->buffer);
	}
}

void update_model_vertex_buffer_command::execute()
{
	ASSERT								( m_object );
	m_world.update_model_vertex_buffer	( m_object, m_fragments);
}

} // namespace render
} // namespace xray