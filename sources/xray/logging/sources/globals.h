////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef LOGGING_GLOBALS_H_INCLUDED
#define LOGGING_GLOBALS_H_INCLUDED

#include <xray/threading_reader_writer_lock.h>
#include "filter_tree_node.h"
#include "filter.h"

namespace xray {
namespace logging {

struct globals_struct
{
	threading::reader_writer_lock				rules_lock;
	uninitialized_reference<filter_tree::node>	root_rule;
	filter_tree::node *							initiator_tree;
	memory::base_allocator *					tree_allocator;
	filter_stack								filter_stack;
	xray::uninitialized_reference<fs_new::synchronous_device_interface>	synchronous_device;
	xray::uninitialized_reference<log_file>		log_file;
	threading::mutex							log_mutex;
	logging::log_file_usage						log_file_usage;
	log_format									format;

	globals_struct	();
};

extern globals_struct *					globals;

} // namespace logging
} // namespace xray

#endif // #ifndef LOGGING_GLOBALS_H_INCLUDED