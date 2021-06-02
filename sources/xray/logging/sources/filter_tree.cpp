////////////////////////////////////////////////////////////////////////////
//	Created		: 30.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/intrusive_double_linked_list.h>
#include <xray/threading_reader_writer_lock.h>
#include <xray/memory_writer.h>
#include <xray/console_command.h>
#include "filter_tree_node.h"
#include "globals.h"

namespace xray {
namespace logging {

using namespace filter_tree;

void initialize_filters					()
{
	ASSERT									(!globals->initiator_tree);
	XRAY_CONSTRUCT_REFERENCE				(globals->root_rule, node) ("", trace);
	globals->initiator_tree				=	globals->root_rule.c_ptr();
}

void   finalize_filters					()
{
	while ( !globals->filter_stack.empty() )
		pop_filter							();

	XRAY_DESTROY_REFERENCE					(globals->root_rule);
}

static bool rule_is_overwritten			(initiator_filter * const rule)
{
	for ( initiator_filter *	it		=	globals->filter_stack.get_next_of_object(rule);
								it		!=	NULL;
								it		=	globals->filter_stack.get_next_of_object(it) )
	{
		if ( rule->initiator.find(it->initiator.c_str()) == 0 )
			return							true;
	}

	return									false;
}

static void build_tree					(memory::base_allocator * allocator)
{
	globals->initiator_tree->clean			(globals->tree_allocator);

	globals->tree_allocator				=	allocator;

	for ( initiator_filter *	it			=	globals->filter_stack.front();
							it			!=	NULL;
							it			=	globals->filter_stack.get_next_of_object(it) )
	{
		if ( !rule_is_overwritten(it) )
			globals->initiator_tree->set	(it->initiator.c_str(), it->verbosity, it->thread_id, allocator, NULL);
	}
}

void push_filter						(pcstr 								initiator, 
										 int const							verbosity, 
										 memory::base_allocator *	const	allocator,
										 u32 const							thread_id)
{
	debug_log_disable_raii					debug_log_disable;
	if ( !initiator )
		initiator						=	"";

	initiator_filter * const rule			=	(initiator_filter *)XRAY_NEW_IMPL( allocator, initiator_filter );
	
	rule->initiator						=	initiator;
	rule->allocator						=	allocator;
	rule->verbosity						=	verbosity;
	rule->thread_id						=	thread_id;

	threading::reader_writer_lock_raii	raii	(globals->rules_lock, threading::lock_type_write);

	globals->filter_stack.push_back			(rule);	

	build_tree								(allocator);
}

void logging::pop_filter					()
{
	debug_log_disable_raii					debug_log_disable;

	threading::reader_writer_lock_raii	raii	(globals->rules_lock, threading::lock_type_write);

	if ( globals->filter_stack.empty( ) )
		return;

	initiator_filter * rule				=	globals->filter_stack.pop_back();

	memory::base_allocator * const allocator	=	rule->allocator;
	XRAY_FREE_IMPL							(allocator, rule);

	build_tree								(globals->tree_allocator);
}

int get_tree_verbosity					(path_parts * const path)
{
	ASSERT									(globals->initiator_tree);

	threading::reader_writer_lock_raii	raii	(globals->rules_lock, threading::lock_type_read);

	return									globals->initiator_tree->get_verbosity(path);
}

class cc_logging_rule : public console_commands::console_command
{
	typedef console_commands::console_command	super;
public:
						cc_logging_rule			( pcstr name );
	virtual void		execute					( pcstr args );
	virtual void		save_to					( console_commands::save_storage& f, memory::base_allocator* a)	const;
} g_cc_logging_rule("logging_rule");// class cc_logging_rule

pcstr verbosity_to_str[] ={
	"", // reserved
	"silent",
	"error",
	"warning",
	"info",
	"debug",
	"trace",
	NULL,
};

verbosity string_to_value				( pcstr item )
{
	u32 idx = 1;
	while( verbosity_to_str[idx] )
	{
		if(0==strings::compare(verbosity_to_str[idx], item))
			return (verbosity)idx;

		++idx;
	}
	return invalid; // not found
}

cc_logging_rule::cc_logging_rule( pcstr name )
:super		( name, true, console_commands::command_type_user_specific, console_commands::execution_filter_early )
{
	m_need_args = true;
}

void cc_logging_rule::execute			(pcstr args)
{
	string512		initiator, verbosity;
	pcstr s								=	strings::get_token( args, initiator, ' ' );
	
	if ( s == NULL )
	{
		if ( strings::equal(initiator, "") )
 		{
			on_invalid_syntax				( args );
			return;
 		}
 		else
		{
			strings::copy					( verbosity, initiator );
			strings::copy					( initiator, "" );
		}
	}
	else
	{
		strings::get_token					( s, verbosity, ' ' );
	}

	xray::logging::verbosity t_verb		=	string_to_value(verbosity);
	if ( t_verb == xray::logging::invalid )
	{
		on_invalid_syntax					( args );
		return;
	}

	push_filter								(initiator, t_verb, & memory::g_mt_allocator);
}

struct rule_name_eq
{
	rule_name_eq ( pcstr s ) : name(s)	{}
	
	bool operator ()					(initiator_filter const & item) const 
	{ 
		return								item.initiator == name; 
	}

private:
	pcstr									name;
};

void cc_logging_rule::save_to			(console_commands::save_storage & f, memory::base_allocator * a) const
{
	typedef vectora<initiator_filter>		rules_vec;
	rules_vec								uniq(a);
	
	for ( initiator_filter *	it		=	globals->filter_stack.front();
								it		!=	NULL;
								it		=	globals->filter_stack.get_next_of_object(it) )
	{
		rules_vec::iterator found		=	std::find_if(uniq.begin(), uniq.end(), rule_name_eq(it->initiator.c_str()) );
		if ( found != uniq.end() )
		{
			initiator_filter & rule		=	* found;
			rule.verbosity				=	it->verbosity;
		} 
		else
			uniq.push_back					(*it);
	}
	
	u32 max_length						=	0;
	rules_vec::const_iterator uit_b		=	uniq.begin();
	rules_vec::const_iterator uit_e		=	uniq.end();
	for ( rules_vec::const_iterator uit =	uit_b; uit != uit_e; ++uit )
	{
		pcstr rule_str					=	(*uit).initiator.c_str();
		pcstr verbosity_str 			=	verbosity_to_str[(*uit).verbosity];
		u32 const length_to_test		=	strings::length(rule_str) + strings::length(verbosity_str);
		if ( length_to_test > max_length )
			max_length					=	length_to_test;
	}

	u32 const buffer_size				=	(max_length + strings::length( name() ) + 3)*sizeof(char);
	pstr const out_str					=	static_cast<pstr>( ALLOCA( buffer_size ) );
	for ( rules_vec::const_iterator uit = uit_b; uit != uit_e; ++uit )
	{
		pcstr rule_str					=	(*uit).initiator.c_str();
		pcstr verbosity_str 			=	verbosity_to_str[(*uit).verbosity];
		strings::join						( out_str, buffer_size, name(), " ", rule_str, " ", verbosity_str );
		f.add_line							( out_str );
	}
}

} // namespace logging
} // namespace xray