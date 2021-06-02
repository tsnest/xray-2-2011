////////////////////////////////////////////////////////////////////////////
//	Created		: 25.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include <xray/fs/watcher_utils.h>
#include <xray/fs/path_string_utils.h>
#include <xray/fs/file_type.h>
#include <xray/intrusive_double_linked_list.h>

namespace xray {
namespace fs_new {

#if XRAY_FS_NEW_WATCHER_ENABLED

struct skip_deassociation_node
{
	skip_deassociation_node	() : next(NULL), prev(NULL) {}

	skip_deassociation_node *	next;
	skip_deassociation_node *	prev;

	#pragma warning (push)
#pragma warning (disable:4200)
	char						relative_path[];
#pragma warning (pop)
};

intrusive_double_linked_list<	skip_deassociation_node, skip_deassociation_node *,
								& skip_deassociation_node::prev, 
								& skip_deassociation_node::next, 
								threading::simple_lock	>	s_skip_deassociation_nodes;


static
void   skip_deassociation_notification (native_path_string const & relative_path)
{
	pvoid const buffer					=	DEBUG_MALLOC(sizeof(skip_deassociation_node) + relative_path.length() + 1,
											 "skip_deassociation_node");
	skip_deassociation_node * const new_node	=	new (buffer) skip_deassociation_node;
	strings::copy							(new_node->relative_path, relative_path.length() + 1, relative_path.c_str());

	s_skip_deassociation_nodes.push_back	(new_node);	
}

void   skip_deassociation_notifications	(file_type * file, u32 const notifications_to_skip)
{
	skip_deassociation_notifications		(native_path_string((pcstr)file->file_name), notifications_to_skip);
}

static
bool   try_remove_deassociation_notification (native_path_string const & relative_path)
{
	for ( skip_deassociation_node *	it_node	=	s_skip_deassociation_nodes.back();
									it_node;
									it_node	=	s_skip_deassociation_nodes.get_prev_of_object(it_node) )
	{
		if ( relative_path != it_node->relative_path )
			continue;

		s_skip_deassociation_nodes.erase	(it_node);
		it_node->~skip_deassociation_node	();
		DEBUG_FREE							(it_node);
		return								true;		
	}

	return									false;
}

#else // #if XRAY_FS_NEW_WATCHER_ENABLED

void skip_deassociation_notifications		(file_type *, u32 const)
{ 
}

static void skip_deassociation_notification			(native_path_string const & relative_path)
{ 
	XRAY_UNREFERENCED_PARAMETER				(relative_path);
}

static bool try_remove_deassociation_notification	(native_path_string const & relative_path)
{
	XRAY_UNREFERENCED_PARAMETER				(relative_path);
	return									true;
}

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

void   skip_deassociation_notifications	(native_path_string const & absolute_physical_path, u32 notifications_to_skip)
{
	R_ASSERT								(is_absolute_path(absolute_physical_path));

	native_path_string						relative_path;
	convert_to_relative_path				(& relative_path, absolute_physical_path);

#pragma message(XRAY_TODO("Lain for new vfs: implement commented below"))
// 	if ( !folder_is_watched(relative_path.c_str()) )
// 		return;

	for ( u32 i=0; i<notifications_to_skip; ++i )
		skip_deassociation_notification		(relative_path);
}

bool   try_remove_deassociation_notifications (native_path_string const &	physical_path, 
											   u32 const					notifications_to_skip)
{
	native_path_string						relative_path;
	convert_to_relative_path				(& relative_path, physical_path);

	for ( u32 i=0; i<notifications_to_skip; ++i )
		if ( !try_remove_deassociation_notification(relative_path) )
			return							false;

	return									true;
}

} // namespace fs_new
} // namespace xray
