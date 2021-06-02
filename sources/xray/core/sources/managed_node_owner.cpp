////////////////////////////////////////////////////////////////////////////
//	Created		: 31.01.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/managed_node_owner.h>
#include "managed_node.h"
#include <xray/managed_allocator.h>

namespace xray		{
namespace memory	{

//----------------------------------------------------------
// managed_node_owner
//----------------------------------------------------------

managed_node_owner::managed_node_owner(managed_allocator * allocator) 
	: m_node(NULL), m_allocator(allocator) 
{
	ASSERT									(m_allocator);
}

managed_node_owner::~managed_node_owner ()
{
	ASSERT									(!m_node);
}

void   managed_node_owner::resize_down (u32 const new_size)				
{
	R_ASSERT								(new_size);
	on_managed_node_resized					(new_size);

	u32 const node_new_size				=	sizeof(managed_node) + new_size;

	m_allocator->resize_down				(m_node, node_new_size);
	m_allocator->check_consistency			();
}

void   managed_node_owner::set_is_unmovable (bool const is_unmovable)
{
	ASSERT									(m_node);
	managed_node * node					=	m_node;

	if ( node->is_unmovable() == is_unmovable )
		return;

	threading::mutex & mutex			=	m_allocator->get_defragmentation_mutex ();
	if ( is_unmovable )
		mutex.lock							();

	node->set_is_unmovable					(is_unmovable);
	R_ASSERT								(& m_allocator->get_node_arena(node) == m_allocator);
#pragma message(XRAY_TODO("Lain 2 Lain: commented while defragmentation is turned off"))
	//m_allocator->notify_unmovable_changed	(node);

	if ( is_unmovable )
		mutex.unlock						();
}

bool   managed_node_owner::is_unmovable () const
{
	ASSERT									(m_node);
	return									m_node->is_unmovable();
}

pcbyte   managed_node_owner::pin ()
{
	// copy of m_node to work with, because m_node can be changed at any moment
	managed_node *	node				=	m_node;
	ASSERT									(node);
 	pbyte			data				=	(pbyte)node + sizeof(managed_node);

// 	fs::path_string							full_path;
// 	get_virtual_path						(full_path);
// 	LOGI_INFO("resources:managed_resource", "pinned managed_resource \"%s\", addr = 0x%08x", 
// 											 full_path.c_str(), 
// 											 data);

	threading::interlocked_increment		(node->m_pin_count);
	return									data;
}

void   managed_node_owner::unpin (pcbyte const_pinned_data)
{
	pbyte			pinned_data			=	const_cast<pbyte>(const_pinned_data);
	managed_node*	node				=	(managed_node*)
											(pinned_data - sizeof(managed_node));

	threading::interlocked_decrement		(node->m_pin_count);

	if ( node->m_unpin_notify_allocator && !node->m_pin_count )
	{
		node->m_unpin_notify_allocator->notify_unpinned_object();
		node->m_unpin_notify_allocator	=	NULL;
	}

// 	fs::path_string							full_path;
// 	get_virtual_path						(full_path);
// 	LOGI_INFO("resources:managed_resource", "unpinned managed_resource \"%s\", addr = 0x%08x", 
// 											 full_path.c_str(), 
// 											 (pbyte)pinned_data);
}

u32   managed_node_owner::get_buffer_size () const
{
	ASSERT									(m_node);
	return									(u32)m_node->m_size;
}

managed_node *   managed_node_owner::grab_managed_node ()
{ 
	managed_node * out					=	m_node;
	m_node								=	NULL;
	return									out;
}

} // namespace memory
} // namespace xray
