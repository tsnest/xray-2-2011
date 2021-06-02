////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/console_command.h>
#include <xray/console_command_processor.h>
#include <xray/fs/device_utils.h>
#include <xray/fs/file_type_pointer.h>
#include <xray/core_entry_point.h>

namespace xray {
namespace console_commands{

extern console_command* s_console_command_root;


static void show_help(console_command* command)
{
	console_command::info_str	info_buff;
	command->info				(info_buff);
	pcstr out_str				= NULL;
	STR_JOINA					(out_str, command->name(), ":", info_buff);
	LOG_INFO					(out_str);
}

console_command* find(pcstr str)
{
	console_command* result = NULL;

	console_command* current = s_console_command_root;

	while(current)
	{
		if( strings::equal(current->name(), str) )
			break;

		current = current->prev();
	}
	result	= current;
	return	result;
}

u32 get_similar(pcstr starts_from, console_command** dst, u32 dst_size)
{
	u32 result = 0;
	console_command* current = s_console_command_root;

	while(current && result<dst_size)
	{
		pcstr current_name = current->name();
		if( current_name==strstr(current_name, starts_from) )
		{
			*dst	= current;
			++dst;
			++result;
		}
		current = current->prev();
	}

	return		result;
}

void execute(pcstr command_to_execute, execution_filter const filter)
{
	pcstr pos	= strchr(command_to_execute, ' ');

	pcstr cmd	= NULL;
	if(pos)
	{
		size_t len	= pos-command_to_execute+1;//+zero

		pstr p		= ( pstr )ALLOCA(len);
		strings::copy_n	(p, len, command_to_execute, len-1);
		cmd			= p;
	}else
		cmd = command_to_execute;

	pcstr args	= (pos) ? pos+1 : NULL;

	console_command* command	= find(cmd);
	if(!command)
	{
		if ( filter != execution_filter_early )
			LOG_WARNING("unknown command [%s]", cmd);
		return;
	}

	if ( filter != execution_filter_all && command->get_execution_type() != filter )
		return;

	if( (args==NULL || 0==strings::length(args)) && command->need_args())
	{
		console_command::status_str	buff;
		command->status				( buff );
		LOG_INFO					( buff );
	}else
		command->execute	(args);
}


void show_help(pcstr str)
{
	if(str)
	{
		console_command* command	= find(str);
		if(!command)
		{
			LOG_ERROR	("unknown command [%s]", str);
			return;
		}else
			show_help	(command);
		return;
	}

	console_command* current = s_console_command_root;
	while(current)
	{
		show_help		(current);
		current			= current->prev();
	}
}
save_storage::save_storage( memory::base_allocator& a )
:m_lines(a), 
m_allocator(a)
{}

void save_storage::add_line( pcstr str )
{
	m_lines.push_back( strings::duplicate(m_allocator, str));
}

void save_storage::save_to		( memory::writer& f )
{
	std::sort(m_lines.begin(), m_lines.end(), strings::less);
	vectora<pcstr>::const_iterator it = m_lines.begin();
	vectora<pcstr>::const_iterator it_e = m_lines.end();
	for( ;it!=it_e; ++it)
		f.write_string_CRLF( *it );
}

save_storage::~save_storage()
{
	vectora<pcstr>::iterator it = m_lines.begin();
	vectora<pcstr>::iterator it_e = m_lines.end();
	for( ;it!=it_e; ++it)
		XRAY_FREE_IMPL( m_allocator, *it );
}

void save_impl( pcstr file_name, command_type const command_type, memory::base_allocator& a )
{
	memory::writer			f(&a);
	console_command* current = s_console_command_root;
	save_storage			s(a);

	while(current)
	{
		if(current->is_serializable() && command_type==current->get_command_type())
			current->save_to( s, &a );

		current				= current->prev();
	}
	s.save_to				( f );
	f.save_to				( file_name );
}

static void save( pcstr const name, pcstr const path, command_type const command_type, memory::base_allocator& a )
{
	pcstr file_name		= 0;
	STR_JOINA			( file_name, path, "/", name );
	save_impl			( file_name, command_type, a );
}

void save( pcstr name, command_type const command_type, memory::base_allocator& a )
{
	switch ( command_type ) {
		case command_type_engine_internal : {
#ifndef MASTER_GOLD
			save		( name ? name : "startup.cfg", "../../resources/sources", command_type_engine_internal, a );
#endif // #ifndef MASTER_GOLD
			break;
		}
		case command_type_user_specific : {
			save		( name ? name : "user.cfg", core::user_data_directory( ), command_type_user_specific, a );
			break;
		}
		default : NODEFAULT();
	}
}

static bool is_line_term (char a) {	return (a==13)||(a==10); };

static u32	advance_term_string(memory::reader& F)
{
	u32 sz		= 0;
	while (!F.eof()) 
	{
		F.advance(1);				//Pos++;
        sz++;
		if (!F.eof() && is_line_term(*(char*)F.pointer()) ) 
		{
        	while(!F.eof() && is_line_term(*(char*)F.pointer())) 
				F.advance(1);		//Pos++;
			break;
		}
	}
    return sz;
}

static void r_string(memory::reader& F, string4096& dest)
{
	char *src 	= (char *) F.pointer();
	u32 sz 		= advance_term_string(F);
	xray::strings::copy_n	(dest, sizeof(dest), src, sz);
}

void load(memory::reader& F, execution_filter const filter)
{
	string4096				str;
	while (!F.eof())
	{
		r_string			(F, str);
		execute				(str, filter);
	}
}

bool execute_console_commands			( fs_new::native_path_string cfg_file_path, execution_filter const filter	)
{
	using namespace  fs_new;
	synchronous_device_interface const & device	=	core::get_core_synchronous_device( );
	
	file_size_type	file_size			=	0;
	if ( !calculate_file_size(device, & file_size, cfg_file_path, assert_on_fail_false) )
		return								false;

	file_type_pointer	file				(cfg_file_path, device, file_mode::open_existing, file_access::read);
	if ( !file )
		return								false;

	mutable_buffer	buffer					(ALLOCA((u32)file_size), (u32)file_size);
	device->read							(file, buffer.c_ptr(), (u32)file_size);

	memory::reader				F			((pbyte)buffer.c_ptr(), (u32)file_size);
	load									(F, filter);
	return									true;
}

} //namespace console_commands
} //namespace xray