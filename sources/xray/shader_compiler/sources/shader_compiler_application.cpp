////////////////////////////////////////////////////////////////////////////
//	Created		: 18.05.2010
//	Author		: Armen Abroyan, Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "shader_compiler_application.h"
#include "shader_compiler_memory.h"
#include <xray/core/core.h>
#include <xray/core/simple_engine.h>
#include <xray/os_include.h>		// for GetCommandLine
#include <xray/configs_lua_config.h>
#include "shader_compiler.h"
#include "define_manager.h"
#include "resource_parser.h"
#include "file_batcher.h"
#include "utility_functions.h"
#include <xray/configs.h>
#include <string>
#include <stack>
#include <xray/render/core/shader_configuration.h>
#include <xray/fs_utils.h>
#include <xray/fs_path.h>

static xray::command_line::key   s_vs_output	( "vs_output",	"", "", "outputs to file (for Visual Studio)");

namespace xray {
namespace shader_compiler {

namespace colorize {
	//12	- red
	//14	- yellow
	//10	- green
	//7, 8  - gray
	//15	- white
	void set_text_color(unsigned short color)
	{
		HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hCon,color);
	}
	void set_base_color		()	{ set_text_color(7);  }
	void set_gray_color		()	{ set_text_color(8); }
	void set_error_color	()	{ set_text_color(12); }
	void set_warning_color	()	{ set_text_color(14); }
	void set_sucesses_color	()	{ set_text_color(10); }

} // namespace colorize

using xray::shader_compiler::application;

xray::memory::doug_lea_allocator_type	xray::shader_compiler::g_allocator;

class shader_compiler_core_engine : public xray::core::simple_engine
{
public:
	//virtual	pcstr	get_mounts_path			( ) const	{ return "../../mounts"; }
	virtual	pcstr	get_mounts_path			( ) const	{ return NULL; }
	virtual	pcstr	get_resources_xray_path	( ) const	{ return "../../resources/sources"; }
};

typedef shader_compiler_core_engine						core_engine_type;
static xray::uninitialized_reference< shader_compiler_core_engine >	s_core_engine;

using xray::configs::lua_config_ptr;
using xray::configs::lua_config_value;
using xray::configs::lua_config_iterator;

void application::initialize( )
{
	m_exit_code				= 0;

	XRAY_CONSTRUCT_REFERENCE( s_core_engine, core_engine_type );

	pstr new_command_line	= 0;
	STR_JOINA				( new_command_line, GetCommandLine(), " -enable_crt_memory_allocator -no_memory_usage_stats -max_resources_size=0 -debug_allocator=1 -log_to_stdout -log_verbosity=silent -no_warning_on_page_file_size" );

	core::preinitialize		(
		& * s_core_engine,
		core::create_log, 
		new_command_line,
		command_line::contains_application_true,
		"shader_compiler",
		__DATE__
	);

	g_allocator.do_register	( 4*1024*1024, "shader_compiler" );

//	memory::lock_process_heap	( );

	memory::allocate_region	( );

	core::initialize		( "../../resources/sources/", "shader_compiler", core::perform_debug_initialization );

	logging::push_filter		( "", logging::silent );
	logging::push_filter		( "shader_compiler", logging::trace );

	LOG_INFO				(
		logging::settings (
			"",
			logging::settings::flags_log_only_user_string | logging::settings::flags_log_to_console
		),
		""
	);
}

void application::finalize	( )
{
	core::finalize			( );

//	memory::unlock_process_heap	( );

	XRAY_DESTROY_REFERENCE	( s_core_engine );
}


void on_load( resources::queries_result & in_result);

struct message_holder {
	std::string		message;
	u32				run;
	u32				order;
}; // struct message

struct message_holder_predicate_message_only {
	inline	bool	operator ( )	( message_holder const& left, message_holder const& right ) const
	{
		return				left.message < right.message;
	}
}; // struct message_holder_predicate_message_only

struct message_holder_predicate {
	inline	bool	operator ( )	( message_holder const& left, message_holder const& right ) const
	{
		return				left.order < right.order;
	}
}; // struct value_predicate

void application::execute	( )
{
	//static bool switcher = true;
	//while( switcher)
	//{
	//	Sleep(200);
	//}
	
	if (command_line::show_help() || shader_compiler_key::is_no_one_set())
	{
		printf("\n Help\n\n");
		shader_compiler_key::print_parameters();
	}
	
	std::string command_line = GetCommandLine();
	std::string entry_point = "main";
	
	files_batcher	f_batcher		(	 "ps,vs,gs,hs,cs"	);
	if ( f_batcher.error_occured() ) {
		m_exit_code					= 1;
		return;
	}
	define_manager  define_manager(f_batcher.get_general_path().c_str());
	
	if ( !define_manager.get_valid())
	{
		m_exit_code					= 2;
		return;
	}
	
	float	total_compile_time		= 0.0f;
	u32		total_failed_shaders	= 0;
	u32		total_with_warnings		= 0;
	u32		total_success_shaders	= 0;
	float	total_shaders_size		= 0.0f;
	u32		compiled_shader_index	= 0;
//	bool show_define_name = g_show_define_names.is_set();
	
	while ( ++f_batcher )
	{
		bool legacy = false;
		
		std::string new_shader_path = f_batcher.get_file_name();
		std::string file_data = f_batcher.get_file_data();
		
		if (g_use_declarated_defines.is_set())
		{
			xray::render::shader_declarated_macroses_list found_declared_macroses;
			xray::render::found_shader_declarated_macroses(file_data.c_str(), found_declared_macroses);
			
			define_manager.merge_with_declarated_defines(found_declared_macroses);
		}
		
		char* buffer	= file_data.begin();
		u32 file_size	= file_data.length();		
		
//		bool is_debug = g_debug_mode.is_set();

//		printf("\n\n\n%d. Processing shader '%s'%s", compiled_shader_index+1, get_file_name(new_shader_path.c_str()).c_str(),is_debug?"  (debug)":"");
		
		u32 define_set_index = 0;
		
		define_set* changed_set = 0;
		
		shader_bytecode_buffer bytecode;
		bytecode.data = 0;
		bytecode.size = 0;
		
		define_manager.prepare();
		
		typedef std::set<message_holder, message_holder_predicate_message_only>	messages_type;
		messages_type	messages;

		for (u32 run=0; define_manager.next_defines_set(changed_set, bytecode); ++run )
		{
			pvoid shader_byte_code	= 0;
			u32 shader_size			= 0;
			u32 instruction_number	= 0;
			
			compiler comp(
				new_shader_path,
				buffer,
				file_size,
				"main",
				define_manager.get_defines_set()
				);
			
			compiler::messages_type errors, 
									 warnings;
			
//			printf("\n   %d.%d.( ",compiled_shader_index+1,define_manager.get_define_set_index());
			
			DWORD	start_time		= timeGetTime	( );
			bool	result			= comp.compile	( legacy, shader_byte_code, shader_size, instruction_number, errors, warnings );
			DWORD	compile_time	= timeGetTime	( ) - start_time;

			bytecode.data			  = shader_byte_code;
			bytecode.size			  = shader_size;
			
			if (g_show_not_affect_defines.is_set())
				define_manager.compare_bytecode( bytecode );
			
//			printf(".");
			//printf("   "); define_manager.print_set				( show_define_name); printf("  "); 
			//printf("%s): ...",show_define_name?"   ":" ");
			

			//if (result)
			//{
			//	colorize::set_sucesses_color		( );
			//	printf("Succeeded");
			//}
			//else
			//{
			//	colorize::set_error_color			( );
			//	printf								( "Failed" );
			//}

			printf("\n");

			colorize::set_base_color				( );
			
			define_set_index++;
			
			total_compile_time+=compile_time/1000.0f;
			
			if ( warnings.size() || errors.size() )
				printf("\n");

			struct message {
				char file_name[260];
				char error_message[4096];
				u32 line;
			}; // struct message

			string4096					temp;
			if ( !errors.empty() )
			{
				colorize::set_error_color();
				for (u32 index=0; index<errors.size(); index++) {
					message	current;
					current.error_message[0] = 0;
					u32 column;
					sscanf_s( errors[index].second.c_str(), "%[^(](%d,%d): %[^\r\n]", current.file_name, sizeof(current.file_name), &current.line, &column, current.error_message, sizeof(current.error_message) );
					if ( !current.error_message[0] ) {
						strings::copy		( current.file_name, new_shader_path.c_str() );
						current.line		= 1;
						strings::copy		( current.error_message, errors[index].second.c_str() );
					}
					sprintf_s				( temp, "%s%s(%d) : %s", ".\\"/**f_batcher.get_general_path().c_str()/**/, xray::fs_new::file_name_from_path( fs_new::virtual_path_string(current.file_name) ), current.line, current.error_message );

					message_holder			holder;
//					holder.file_name		= current.file_name;
//					holder.line_number		= current.line;
					holder.run				= run;
					holder.order			= index;
					holder.message			= temp;
					messages.insert			( holder );
				}
			}
			
			if ( !warnings.empty() )
			{
				colorize::set_warning_color();
				for (u32 index=0; index<warnings.size(); index++) {
					message	current;
					u32 column;
					sscanf_s( warnings[index].second.c_str(), "%[^(](%d,%d): %[^\r\n]", current.file_name, sizeof(current.file_name), &current.line, &column, current.error_message, sizeof(current.error_message) );
					if ( !current.error_message[0] ) {
						strings::copy		( current.file_name, new_shader_path.c_str() );
						current.line		= 1;
						strings::copy		( current.error_message, warnings[index].second.c_str() );
					}

					sprintf_s				( temp, "%s%s(%d) : %s", ".\\"/**f_batcher.get_general_path().c_str()/**/, 
											  xray::fs_new::file_name_from_path(xray::fs_new::virtual_path_string::convert(current.file_name)), 
											  current.line, 
											  current.error_message );

					message_holder			holder;
//					holder.file_name		= current.file_name;
//					holder.line_number		= current.line;
					holder.run				= run;
					holder.order			= warnings[index].first;
					holder.message			= temp;
					messages.insert			( holder );
				}

				total_with_warnings++;
			}

			if ( warnings.size() || errors.size() )
				printf("\n");
			
			colorize::set_base_color();
			
			total_shaders_size += (float)shader_size/1024.0f;
			
			bool show_size  = g_size.is_set();
			bool show_time  = g_time.is_set();
			bool show_ni	= g_instruction_number.is_set();
			
			if ( show_size )
				printf("   shader size: %.2f KB", (float)shader_size/1024.0f);
			
			if ( show_time )
				printf("   time: %.3f sec.",(float)compile_time/1000.0f);
			
			if ( show_ni )
				printf("   instructions: %d ",instruction_number);
			
			if (show_size || show_time || show_ni)
				printf("\n");
			
			colorize::set_base_color();
			
			total_failed_shaders+=(result?0:1);
			total_success_shaders+=(result?1:0);

			if ( !errors.empty() )
				break;
		}
		
		typedef std::vector< message_holder >	message_holders_type;
		message_holders_type					message_holders;
		message_holders.insert					( message_holders.begin(), messages.begin(), messages.end() );
		message_holders_type::iterator i		= message_holders.begin( );
		message_holders_type::iterator const e	= message_holders.end( );
		for ( ; i != e; ) {
			message_holders_type::iterator j	= i;

			for ( j = i + 1; j != e && (*j).run == (*i).run; ++j ) ;

			std::sort							( i, j, message_holder_predicate() );

			for ( ; i != j; ++i )
				printf							( "%s\r\n", (*i).message.c_str() );
		}

		if (g_show_not_affect_defines.is_set() && (total_success_shaders + total_failed_shaders)>1 && define_manager.has_unusable_defines())
		{
			colorize::set_warning_color();
				printf("\nWarning:\n     ");
				define_manager.print_unusable_defines();
				printf(" - do not affect the configuration.\n");
			colorize::set_base_color();
		}
		
//		printf("\n-----------------------------------------------------------");
		compiled_shader_index++;
	}
	
	//printf("\n\n> Total configurations: %d,", total_success_shaders+total_failed_shaders);
	//
	//if (!total_failed_shaders) colorize::set_sucesses_color();
	//printf(" Success:"); 
	//printf(" %d", total_success_shaders);
	//colorize::set_base_color();
	//printf(",");

	//if (total_failed_shaders) colorize::set_error_color();
	//printf(" With errors:");
	//printf(" %d", total_failed_shaders);
	//colorize::set_base_color();
	//printf(",");

	//if (total_with_warnings) colorize::set_warning_color();
	//printf(" With warnings:");
	//printf(" %d", total_with_warnings);
	//colorize::set_base_color();
	//printf(",");
	//
	//printf(" Total shaders size: %.2f %s, Total time: %.2f %s",
	//	(total_shaders_size>=1024.0f) ? total_shaders_size/1024.0f : total_shaders_size,
	//	(total_shaders_size>=1024.0f) ? "MB" : "KB",
	//	(total_compile_time>=60.0f) ? ((total_compile_time>=3600.0f) ? total_compile_time/3600.0f : total_compile_time/60.0f) : total_compile_time,
	//	(total_compile_time>=60.0f) ? ((total_compile_time>=3600.0f) ? "hours" : "min.") : "sec."
	//	);
	
//	_getch();

	fixed_string<260>	string;
	if ( s_vs_output.is_set_as_string( &string ) ) {

		using namespace fs_new;
		synchronous_device_interface const & device	=	resources::get_synchronous_device();
		
		file_type* file;
		if ( device->open( &file, fs_new::native_path_string::convert(string.get_buffer()), file_mode::create_always, file_access::write, assert_on_fail_false ) ) {

			char string[] = "this is simple output from shader compiler for Visual Studio";
			device->write	( file, string, sizeof(string) );
			device->close	( file );
		}
	}
	
	m_exit_code				= s_core_engine->get_exit_code();
}

} // namespace shader_compiler
} // namespace xray