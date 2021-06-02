////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "define_manager.h"
#include "utility_functions.h"

namespace xray {
namespace shader_compiler {

std::vector<std::string> define_manager::string_hash;

define_value::define_value(const std::string& value_name):
	m_value(value_name)
{
	m_value_hash = define_manager::hash_string(m_value);
}

defination::defination(const std::string& define_name):
	m_name(define_name),
	m_is_undefined_only(false)
{
	m_name_hash = define_manager::hash_string(m_name);
}

define_value* defination::add_value( const std::string& value )
{
	define_value* exists_value = has_value(value);

	if ( exists_value )
	{
		printf("defination::add_value(): define with name '%s' already exists",value.c_str());
		return exists_value;
	}

	define_value* new_value = NEW(define_value)( value );
	m_values.push_back( new_value );

	return new_value;
}


define_value* defination::has_value( const std::string& search_value ) const
{
	for ( std::vector<define_value*>::const_iterator it = m_values.begin(); it!=m_values.end(); ++it )
	{
		if ( (*it)->get_value()==search_value )
		{
			return *it;
		}
	}
	return NULL;
}

void defination::set_undefined_only(bool undef)
{
	m_is_undefined_only = undef;
}

bool defination::is_undefined_only()
{
	return m_is_undefined_only;//get_num_values()==1 && get_value(0)->get_value()=="UNDEFINED";
}

void defination::remove_values()
{
	for ( std::vector<define_value*>::iterator it = m_values.begin(); it!=m_values.end(); ++it )
	{
		define_value* v = *it;
		DELETE(v);
	}
	m_values.clear();
}

define_value* defination::get_value( u32 index )
{
	if ( index < m_values.size() )
		return m_values[index];

	return NULL;
}

defination* define_manager::add_define( const std::string& define_id )
{
	defination* result = NEW(defination)( define_id );
	m_defines.push_back( result );
	return result;
}

defination* define_manager::get_define( u32 index ) const
{
	if ( index < m_defines.size() )
		return m_defines[index];
	return NULL;
}

defination* define_manager::has_define(const std::string& define_id) const
{
	for ( std::vector<defination*>::const_iterator it = m_defines.begin(); it!=m_defines.end(); ++it)
	{
		if ( (*it)->get_name()==define_id )
		{
			return *it;
		}
	}
	return NULL;
}

void define_manager::on_load( resources::queries_result & in_result )
{
	if ( !in_result.is_successful() )
	{
		printf("Failed to load defines file!");
		return;
	}

	configs::lua_config_ptr config_ptr = static_cast_checked<configs::lua_config *>(in_result[0].get_unmanaged_resource().c_ptr());
	configs::lua_config_value const & config	=	config_ptr->get_root()["DEFINES"];

	if (config.get_type() != configs::t_table_named)
	{
		printf("\nDEFINES table not found in defines file!");
		return;
	}

	for ( configs::lua_config::const_iterator	it	=	config.begin();
		it	!=	config.end();
		++it )
	{
		configs::lua_config_value const & next_table	=	*it;
		fixed_string<260> next_table_name = it.key();

		defination* define = add_define(next_table_name.c_str());

		// its defination
		for ( configs::lua_config_value::const_iterator	it	=	next_table.begin();
			it	!=	next_table.end();
			++it )
		{
			configs::lua_config_value const & value	=	*it;

			if (value.get_type()==configs::t_string)
			{
				define_value* def_value = define->add_value((pcstr)value);
				def_value->set_undefined( false );
			}
			else if (value.get_type()==configs::t_integer)
			{
				define_value* def_value = define->add_value("UNDEFINED");
				def_value->set_undefined( true );
			}
			else
			{
				printf("Error: unknown value type");
				define_value* def_value = define->add_value("UNKNOWN_VALUE_TYPE");
				def_value->set_undefined( true );
			}
		}
	}


	configs::lua_config_value const & conflicts_config	=	config_ptr->get_root()["CONFLICTS"];
	u32 num_conflicts = 0;

	if (conflicts_config.get_type()==configs::t_table_indexed 
		|| conflicts_config.get_type()==configs::t_table_named)
	{
		for ( configs::lua_config::const_iterator	it	=	conflicts_config.begin();
			it	!=	conflicts_config.end();
			++it )
		{
			configs::lua_config_value const & conflict_table	= *it;

			fixed_string<260> define_names[2];
			s32 temp_index = 0;
			for ( configs::lua_config_value::const_iterator	conf_it  =	conflict_table.begin();
				conf_it	!=	conflict_table.end();
				++conf_it )
			{
				if (temp_index==2) break;
				define_names[temp_index++] = conf_it.key();
			}

			bool duplicated_conflict_pair = false;
			for (u32 c=0; c<m_conflicts.size(); c++)
			{
				if (m_conflicts[c].define[0]->get_name()==define_names[0].c_str() 
					&& m_conflicts[c].define[1]->get_name()==define_names[1].c_str())
				{
					duplicated_conflict_pair = true;
					break;
				}
			}
			if (duplicated_conflict_pair)
			{
				printf("\nERROR: founded duplicated conflict pair (%s, %s)", define_names[0].c_str(), define_names[1].c_str());
				continue;
			}

			num_conflicts++;
			u32 define_index = 0;

			conflict_pair temp_pair;
			m_conflicts.push_back(temp_pair);

			conflict_pair& pair = m_conflicts[m_conflicts.size() - 1];

			//per defines, must be 2
			for ( configs::lua_config_value::const_iterator	conf_it  =	conflict_table.begin();
				conf_it	!=	conflict_table.end();
				++conf_it )
			{
				configs::lua_config_value const & define_table	= *conf_it;
				fixed_string<260> define_table_name = conf_it.key();

				pair.define[define_index] = NEW(defination)(define_table_name.c_str());

				// table with values
				if (define_table.get_type()==xray::configs::t_table_indexed)
				{
					for ( configs::lua_config_value::const_iterator	define_values_it	=	define_table.begin();
						define_values_it	!=	define_table.end();
						++define_values_it )
					{
						configs::lua_config_value const & def_value	= *define_values_it;
						if (def_value.get_type()==xray::configs::t_integer)
						{
							define_value* def_v = pair.define[define_index]->add_value("UNDEFINED");
							def_v->set_undefined(true);
						}
						else if (def_value.get_type()==xray::configs::t_string)
						{
							define_value* def_v = pair.define[define_index]->add_value((pcstr)def_value);
							def_v->set_undefined(false);
						}
						else if (def_value.get_type()==xray::configs::t_string)
						{
							define_value* def_v = pair.define[define_index]->add_value("UNKNOWN_VALUE_TYPE");
							def_v->set_undefined(true);
						}
					}
				}
				define_index++;
				if (define_index>2)
					break;
			}
			if (define_index!=2)
			{
				printf("ERROR: count of conflicted defines must be 2");
			}
		}
	}
	m_valid = true;
}

bool define_manager::skip_set()
{
	if (!m_first_pass && m_next_set)
	{
		bool skip = true;
		for (u32 i=0; i<m_defines_set.size(); i++)
		{
			if ( m_defines_set[i].usable && m_defines_set[i].index_iter != m_prev_set[i].index_iter)
			{
				skip = false;
				break;
			}
		}
		if (skip)
			return true;
	}
	
	m_first_pass = false;

	return is_conflicted();
}

void define_manager::merge_with_declarated_defines( xray::render::shader_declarated_macroses_list const& declared_defines)
{
	for (defines_list_type::iterator def_it=m_defines.begin(); def_it!=m_defines.end(); ++def_it)
	{
		pcstr to_find  = (*def_it)->get_name().c_str();
		
		bool found = false;
		for (xray::render::shader_declarated_macroses_list::const_iterator decl_it=declared_defines.begin(); decl_it!=declared_defines.end(); ++decl_it)
		{
			pcstr current_decl_name = (*decl_it).c_str();
			
			if (xray::strings::compare(to_find, current_decl_name)==0)
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			(*def_it)->remove_values();
			(*def_it)->set_undefined_only( true );
			define_value* def_value = (*def_it)->add_value("UNDEFINED");
			def_value->set_undefined( true );
		}
	}
}

define_manager::define_manager( char const* alt_path_to_defines)
{
	(void)&alt_path_to_defines;

	char curr_dir[513];
	GetCurrentDirectory(512,(LPTSTR)curr_dir);
	
	m_need_bytecode_first_fill = false;
	m_next_set = true;
	m_valid = false;
	m_sorted = false;
	m_num_max_usable_defines = 0;
	m_num_usable_defines = 0;
	std::string command_line = GetCommandLine();


	std::string defines_file_name;

#if 0//NDEBUG
	s32 l_pos_app_name  = -1;
	l_pos_app_name  = command_line.find("\"",l_pos_app_name+1);
	l_pos_app_name	= command_line.find("\"",l_pos_app_name+1);

	get_text_in_quotes(command_line,defines_file_name,l_pos_app_name+1);
#else
	// application name
	s32 l_pos_app_name  = command_line.find("\"");
	l_pos_app_name		= command_line.find("\"",l_pos_app_name+1);

	// shader/directory name
	l_pos_app_name  = command_line.find("\"",l_pos_app_name+1);
	l_pos_app_name	= command_line.find("\"",l_pos_app_name+1);

	get_text_in_quotes(command_line,defines_file_name,l_pos_app_name+1);
#endif

	//s32 def_pos = command_line.find("-def");
	//if (def_pos!=-1 && get_text_in_quotes(command_line,defines_file_name,def_pos+1)==-1)
	//	printf( "Defines file \"%s\" not found", defines_file_name.c_str() );
	
	//if (!check_file_exists(defines_file_name.c_str()))
	//{
	//	printf("\nInvalid path to defines \"%s\"",defines_file_name.c_str());//file_name.c_str());
	//	_getch();
	//	return;
	//}
	
	m_define_set_index = 0;
	m_skip_used = true;
	m_first_pass = true;
	
	std::string new_file_name = "@" + defines_file_name;
	
	for (u32 i=0; i<new_file_name.length(); i++)
	{
		if (new_file_name[i]=='\\')
			new_file_name[i]='/';
	}
	xray::resources::query_resource_and_wait(
		new_file_name.c_str(),
		xray::resources::lua_config_class,
		boost::bind( &define_manager::on_load, this, _1 ),
		&g_allocator);
	//xray::resources::wait_and_dispatch_callbacks(true);
	
	m_prev_set = m_defines_set;
	
	SetCurrentDirectory(curr_dir);
}
u32 define_manager::hash_string(const std::string& str)
{
	for (u32 i=0; i<define_manager::string_hash.size(); i++)
	{
		if (define_manager::string_hash[i]==str)
		{
			return i;
		}
	}
	define_manager::string_hash.push_back(str);
	
	return define_manager::string_hash.size() - 1;
}
define_manager::~define_manager( )
{
	for (std::vector<defination*>::iterator it = m_defines.begin(); it!=m_defines.end(); ++it)
	{
		XRAY_DELETE_IMPL(	::xray::shader_compiler::g_allocator, *it );
	}
	for (std::vector<conflict_pair>::iterator it = m_conflicts.begin(); it!=m_conflicts.end(); ++it)
	{
		XRAY_DELETE_IMPL(	::xray::shader_compiler::g_allocator, it->define[0] );
		XRAY_DELETE_IMPL(	::xray::shader_compiler::g_allocator, it->define[1] );
	}
}

bool define_manager::is_conflicted( )
{
	if (m_define_set_index==0)
		return false;

	for (u32 c=0; c<m_conflicts.size(); c++)
	{
		bool conf[2]	= { false, false };
		bool values[2]	= { false, false };
		
		for (u32 dc=0; dc<2; dc++)
		{
			defination* conflict = m_conflicts[c].define[dc];
			
			for (u32 d=0; d<m_defines_set.size(); d++)
			{
				const define_set& set = m_defines_set[d];
				
				if (!set.usable || conflict->get_name_hash()!=set.define->get_name_hash())
					continue;
				
				conf[dc] = conflict->get_num_values()==0;
				for (u32 v=0; v<conflict->get_num_values(); v++)
				{
					if (conflict->get_value(v)->get_value_hash()==set.define->get_value(set.index_iter)->get_value_hash())
						values[dc] = true;
				}
			}
		}
		
		// value <-> value, define <-> value, define <-> define, value <-> value
		if (values[0] && values[1]
		||  conf[0]   && values[1] 
		||  conf[1]   && values[0] 
		||  conf[0]   && conf[1]  )
			return true;
	}
	return false;
}

static bool operator < (define_set A, define_set B)
{
	return A.real_index < B.real_index;
}

void define_manager::print_set( bool show_define_name)
{
	const char* devider = ", ";

	define_set_list_type temp = m_defines_set;
	std::sort(temp.begin(),temp.end());
	
	u32 max_length = 0;
	

	for (u32 i=0; i<temp.size(); i++)
	{
		if (!temp[i].usable)
			continue;

		defination* define = temp[i].define;
		
		u32 len = define->get_name().length();
		if (len>max_length)
			max_length = len;
	}


	for (u32 i=0; i<temp.size(); i++)
	{
		defination* define = temp[i].define;
		define_value* value = define->get_value(temp[i].index_iter);
		bool usable = temp[i].usable;
		
		std::string v_str = value->get_value().c_str();
		if (value->is_undefined())
		{
			v_str = "_";
		}
		if (show_define_name)
		{
			if (usable)
			{
				std::string add_spaces;
				add_spaces.append(max_length - define->get_name().length(), ' ');

				printf("\n      %s%s =   %s",define->get_name().c_str(),add_spaces.c_str(),v_str.c_str());
			}
		}
		else
			//if (usable)// && v_str.length())
			//	printf("%s%s",v_str.c_str(), (i==m_defines_set.size()-1) ? "":devider);
				printf("%s%s",usable ? v_str.c_str():"_", (i==m_defines_set.size()-1) ? "":devider);
	}

if (show_define_name)
	printf("\n");
}

bool define_manager::is_found_in_history()
{
	u32 num_skiped = 0;
	for (u32 i=0; i<m_defines_set_history.size(); i++)
	{
		define_set_list_type& current_define_set = m_defines_set_history[i];
		for (u32 d=0; d<current_define_set.size(); d++)
		{
			if ( !((!current_define_set[d].usable) || current_define_set[d].index_iter==m_defines_set[d].index_iter ))
			{
				num_skiped++;
				break;
			}
		}
	}

	if (num_skiped!=m_defines_set_history.size() && m_define_set_index!=0)
		return true;
	return false;
}

static inline void swap_set(define_set& A, define_set& B)
{
	define_set temp = A;
	A = B;
	B = temp;
}

void define_manager::sort_sets()
{
	for(;;)
	{
		if (!m_defines_set.size())
			break;
		u32 num_unusable = 0;
		for (u32 i=0; i<m_defines_set.size(); i++)
		{
			bool next = false;
			if (!m_defines_set[i].usable)
			{
				num_unusable++;
				continue;
			}
			bool found_unusable = false;
			for (u32 j=i; j<m_defines_set.size(); j++)
			{
				if (m_defines_set[j].usable)
					continue;
				found_unusable = true;
				swap_set(m_defines_set[i],m_defines_set[j]);
				swap_set(m_prev_set[i],m_prev_set[j]);
				for (u32 h=0; h<m_defines_set_history.size(); h++)
				{
					define_set_list_type& curr_set = m_defines_set_history[h];
					swap_set(curr_set[i],curr_set[j]);
				}
				next = true;
				break;
			}
			if (!found_unusable)
				return;
			if (next)
				break;
		}
		if (num_unusable==m_defines_set.size())
			break;
	}
}

u32 define_manager::calc_num_usable()
{
	u32 result = 0;
	for (u32 i=0; i<m_defines_set.size(); i++)
	{
		if (m_defines_set[i].usable)
		result++;
	}
	return result;
}

void define_manager::prepare()
{
	m_defines_set.clear();
	m_prev_set.clear();
	m_defines_set_history.clear();
	m_first_pass = true;
	m_define_set_index = 0;
	m_need_bytecode_first_fill = true;
}

bool define_manager::next_defines_set(define_set*& last_changed_set, shader_bytecode_buffer& bytecode)
{
	last_changed_set = 0;
	
	if (!m_first_pass)
	{
		m_prev_set				= m_defines_set;
		
		if (!is_found_in_history())
		{
			m_defines_set_history.push_back(m_defines_set);
		}
		
		m_num_usable_defines = calc_num_usable();

		if (m_num_usable_defines > m_num_max_usable_defines)
			m_num_max_usable_defines = m_num_usable_defines;
	}
	m_next_set = true;

	bool changed_only_unused = false;
	bool found_in_history = false;

	sort_sets();
	
	bool has_usable = false;
	
	for (u32 i=0; i<m_defines_set.size(); i++)
	{
		if (m_defines_set[i].usable)
		{
			has_usable = true;
			break;
		}
	}
	
	if (m_defines_set.size() && !has_usable)
		return false;
	
	while ( increase_defines_set( changed_only_unused, found_in_history, last_changed_set, bytecode ) )
	{
		if (changed_only_unused && !m_next_set || skip_set())
		{
			m_next_set = false;
			continue;
		}
		else
		{
			if (is_found_in_history())
				continue;


			m_define_set_index++;
			return true;
		}
	}

	return false;
}

bool define_manager::has_unusable_defines() const
{
	for (define_set_list_type::const_iterator it=m_defines_set.begin(); it!=m_defines_set.end(); ++it)
	{
		if (it->usable && !it->effect_to_shader)
		{
			return true;
		}
	}
	return false;
}
void define_manager::print_unusable_defines()
{
	bool first = true;
	for (define_set_list_type::iterator it=m_defines_set.begin(); it!=m_defines_set.end(); ++it)
	{
		if (it->usable && !it->effect_to_shader)
		{
			if (!first)
				printf(", ");
			printf("%s", it->define->get_name().c_str());
			first = false;
		}
	}
}

void define_manager::compare_bytecode( shader_bytecode_buffer& bytecode )
{
	if (m_need_bytecode_first_fill)
	{
		for (define_set_list_type::iterator it=m_defines_set.begin(); it!=m_defines_set.end(); ++it)
			it->bytecode_history.push_back( bytecode );
		m_need_bytecode_first_fill = false;
	}
	else
	{
		for (define_set_list_type::iterator it=m_defines_set.begin(); it!=m_defines_set.end(); ++it)
		{
			if (!it->usable)
				continue;

			if (it->index_iter==0)
			{
				it->bytecode_history.push_back(bytecode);
			}
			else
			{
				shader_bytecode_buffer* b = it->bytecode_history.begin();
				if (!it->effect_to_shader && !b->equal( bytecode ) || !bytecode.data)
					it->effect_to_shader = true;
				
				if (it->bytecode_history.begin()!=it->bytecode_history.end())
					it->bytecode_history.erase(it->bytecode_history.begin());
			}
		}
	}
}

bool define_manager::increase_defines_set( bool& changed_only_unused, bool& found_in_history, define_set*& last_changed_set, shader_bytecode_buffer& /*bytecode*/ )
{
	//first fill
	if ( m_defines_set.empty())
	{
		for ( u32 i=0; i<get_num_total_defines(); i++)
		{
			define_set d_set;
			
			d_set.define = get_define(i);
			
			d_set.usable = true;
			
			d_set.effect_to_shader = false;

			d_set.real_index = i;
			
			d_set.index_iter = 0;
			d_set.max_iter = d_set.define->get_num_values() ? d_set.define->get_num_values() - 1 : 0;
			
			m_defines_set.push_back(d_set);
		}
		return true;
	}

	
	define_set_list_type& increased_set = m_defines_set;
	
	//printf("\n");
	//for (u32 i=0; i<m_defines_set.size(); i++)
	//{
	//	if (m_defines_set[i].define->is_undefined_only())
	//		printf("%d",m_defines_set[i].index_iter);
	//	else
	//		printf("_(%s)%d_",m_defines_set[i].define->get_name().c_str(),m_defines_set[i].index_iter);
	//}
	//printf("\n");
	
	const u32 num_sets = increased_set.size();
	
	if ( !num_sets )
		return false;
	
	changed_only_unused = true;
	found_in_history = true;

	define_set& first_set = increased_set[num_sets-1];
	
	first_set.index_iter += 1;

	if (changed_only_unused && first_set.usable)
		changed_only_unused = false;


	if (num_sets==1 && first_set.index_iter > first_set.max_iter)
		return false;
	else
	if (num_sets>1)
	for (s32 set_index=num_sets-2; set_index>=0; set_index--)
	{
		define_set& curr_set = increased_set[set_index];
		define_set& prev_set = increased_set[set_index+1];

		if ( prev_set.index_iter && prev_set.index_iter > prev_set.max_iter )
		{
			if (changed_only_unused && (prev_set.usable || curr_set.usable))
				changed_only_unused = false;
			
			curr_set.index_iter += 1;
			
			prev_set.index_iter = 0;
	
			last_changed_set = &curr_set;
			
			for (u32 i=0; i<num_sets; i++)
			{
				if (increased_set[i].usable)
				{
					if (increased_set[i].index_iter>increased_set[i].max_iter && m_num_usable_defines == m_num_max_usable_defines)
						return false;
					break;
				}
			}
			
			if (set_index==0 && curr_set.index_iter > curr_set.max_iter)
				return false;
		}
		else
		{
			return true;
		}
	}
	return true;
}


} // namespace shader_compiler
} // namespace xray