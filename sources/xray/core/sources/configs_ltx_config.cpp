////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "configs_ltx_config.h"
#include "configs_binary_config.h"
#include <xray/fs/synchronous_device_interface.h>
#include <xray/fs/file_type_pointer.h>
#include <xray/core_entry_point.h>
#include <xray/strings_functions.h>
#include <xray/configs.h>
#include <ctype.h>
#include <boost/crc.hpp>

using namespace xray;
using namespace xray::configs;
using math::float2;
using math::float3;
using xray::core::configs::binary_config;
using xray::configs::binary_config_value;

typedef	xray::vectora<binary_config_value*>	values_type;

static bool parse_item_value(memory::base_allocator* allocator, binary_config_value& itm, pcstr const value)
{
	R_ASSERT			( value );

	itm.type			= t_string;

	u16 comma_count		= 0;
	u16 dot_count		= 0;

	pcstr p				= value;
	while( (p=strchr(++p, ',')) != 0 ) ++comma_count;

	p					= value;
	while( (p=strchr(++p, '.')) != 0 ) ++dot_count;

	if(strings::length(value)==0)
		return			false;
	else
	if(comma_count==2)
		itm.type		= t_float3;
	else
	if(comma_count==1)
		itm.type		= t_float2;
	else
	if(dot_count==1)
		itm.type		= t_float;
	else
	{
		p = value;
		while(*p && isdigit(static_cast<u8>(*p))) ++p;
		
		if(*p==0)
			itm.type		= t_integer;
		else
			itm.type		= t_string;
	}

	switch(itm.type)
	{
	case t_boolean:
	{
		u32* val			= (u32*)&itm.data;
		*val				= u32(0);
		itm.count			= sizeof(u32);
		break;
	}
	case t_string:
	{
		u16 length			= (u16)strings::length(value)+1;
		itm.data			= (pbyte)XRAY_ALLOC_IMPL(allocator, char, length);
		memory::copy		(const_cast<pvoid>(static_cast<void const*>(itm.data)), length*sizeof(char), value, length); //inclusive zero
		itm.count			= length;
		break;
	}
	case t_float:
	{
		float* val			= (float*)&itm.data;
		*val				= float(atof(value));
		itm.count			= sizeof(float);
		break;
	}
	case t_float2:
	{
		itm.data			= (pbyte)XRAY_ALLOC_IMPL( *allocator, float2, 1);
		float2* val			= (float2*)(static_cast<void const*>(itm.data));
		val->set			(0.0f, 0.0f);
		XRAY_SSCANF			(value, "%f,%f", &val->x, &val->y);
		itm.count			= sizeof(float2);
		break;
	}
	case t_float3:
	{
		itm.data			= (pbyte)XRAY_ALLOC_IMPL( *allocator, float3, 1);
		float3* val			= (float3*)(static_cast<void const*>(itm.data));
		val->set			(0.0f, 0.0f, 0.0f);
		XRAY_SSCANF			(value, "%f,%f,%f", &val->x, &val->y, &val->z);
		itm.count			= sizeof(float3);
		break;
	}
	case t_integer:
	{
		u32* val			= (u32*)&itm.data;
		*val				= u32(atoi(value));
		itm.count			= sizeof(u32);
		break;
	}
	default:
		NODEFAULT();
	};

	return					true;
}

static void parse_item_name(memory::base_allocator* allocator, binary_config_value& itm, pcstr id)
{
	if(id)
	{
		u32 str_len				= strings::length(id)+1;
		itm.id					= XRAY_ALLOC_IMPL( *allocator, char, str_len);
		memory::copy			( const_cast<pstr>(static_cast<char const*>(itm.id)), str_len*sizeof(char), id, str_len);
		boost::crc_32_type		processor;
		processor.process_block	(itm.id, itm.id + str_len -1);
		itm.id_crc				= processor.checksum();
	}else
	{
		itm.id					= NULL;
		itm.id_crc				= 0;
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
	xray::strings::copy_n(dest, sizeof(dest), src, sz);
}


static pstr _TrimLeft( pstr str )
{
	pstr p 	= str;
	while( *p && (u8(*p)<=u8(' ')) ) p++;
    if (p!=str)
	{
		pstr t	= str;
        for(; *p; t++,p++) *t=*p;
        
		*t = 0;
    }
	return str;
}

static pstr _TrimRight( pstr str )
{
	pstr p 	= str+strings::length(str);
	while( (p!=str) && (u8(*p)<=u8(' ')) ) 
		p--;

    *(++p) 		= 0;
	return str;
}

static pstr _Trim( pstr str )
{
	_TrimLeft( str );
	_TrimRight( str );
	return str;
}

static bool _parse(pstr dest, pcstr src)
{
	R_ASSERT		( src );

	bool bInsideSTR = false;
	if (src) 
	{
		while (*src) 
		{
			if (isspace((u8)*src)) 
			{
				if (bInsideSTR)
				{
					*dest++ = *src++;
					continue;
				}
				while (*src && isspace(static_cast<u8>(*src)))
				{
					++src;
				}
				continue;
			} else if (*src=='"') 
			{
				bInsideSTR = !bInsideSTR;
			}
			*dest++ = *src++;
		}
	}
	*dest = 0;
	return bInsideSTR;
}

static bool	is_empty_line_now(memory::reader F) 
{ 
	char* a0 = (char*)F.pointer()-4;
	char* a1 = (char*)(F.pointer())-3;
	char* a2 = (char*)F.pointer()-2;
	char* a3 = (char*)(F.pointer())-1;
 	return (*a0==13) && ( *a1==10) && (*a2==13) && ( *a3==10); 
};
static bool item_less (binary_config_value const* it1, binary_config_value const* it2)
{
	return it1->id_crc < it2->id_crc;
}

struct table_item {
	binary_config_value*	p_table;
	values_type				data;

	inline table_item	( memory::base_allocator* allocator ) :
		data	( *allocator )
	{
	}
	
	void		sort()
	{std::sort	(data.begin(), data.end(), item_less);}
	
	bool operator < (table_item const& other) const
	{return item_less(this->p_table, other.p_table);}
};

static void optimize(memory::base_allocator* allocator,values_type& load_data, binary_config_value* root)
{
	R_ASSERT			(root->count>0);

	vectora<table_item>	tables(*allocator);

	values_type::const_iterator it		= load_data.begin();
	values_type::const_iterator it_e	= load_data.end();

	for(; it!=it_e; ++it)
	{
		binary_config_value* itm		= *it;
		if(itm==root)
			continue;

		if(itm->type==t_table_named || itm->type==t_table_indexed)
		{
			table_item		t(allocator);
			t.p_table		= itm;
			t.data.assign	(it+1, it+itm->count+1);
			
			if(itm->type==t_table_named)
				t.sort			();

			tables.push_back(t);
			std::advance	(it, (size_t)itm->count);
		}
		std::sort(tables.begin(), tables.end());
	}

	load_data.clear						();
	load_data.push_back					(root);
	*(u32*)&root->data				= 1; //first child

	vectora<table_item>::iterator tit	= tables.begin();
	vectora<table_item>::iterator tit_e	= tables.end();
	
	for( ;tit!=tit_e; ++tit)
	{
		table_item& table		= *tit;
		load_data.push_back		(table.p_table);
	}
	
	tit	= tables.begin();
	for( ;tit!=tit_e; ++tit)
	{
		table_item& table		= *tit;
		u32* val				= (u32*)&table.p_table->data;
		*val					= load_data.size();// index of first child
		load_data.insert		(load_data.end(), table.data.begin(), table.data.end());
	}

	it			= load_data.begin();
	it_e		= load_data.end();
}
static u32 calc_mem_usage(values_type const& data)
{
	u32 result			= 0;
	values_type::const_iterator it		= data.begin();
	values_type::const_iterator it_e		= data.end();

	for(; it!=it_e; ++it)
	{
		binary_config_value* itm		= *it;
		
		if(itm->id)
			result		+= strings::length(itm->id)+1;//+zero

		if(itm->type > t_embedded_types)
			result		+= itm->count;
	}
	return result;
}

static void save_bin(memory::base_allocator* allocator, pbyte* dest, u32& dest_size, values_type const& data, binary_config_value* root)
{
	XRAY_UNREFERENCED_PARAMETER	(root);
	u32 data_mem_size		= calc_mem_usage(data);
	pbyte plain_data_begin	= (pbyte)XRAY_MALLOC_IMPL(*allocator,data_mem_size, "ltx_plain_data");
	pbyte data_ptr			= plain_data_begin;

	values_type::const_iterator it		= data.begin();
	values_type::const_iterator it_e	= data.end();
	u32 count				= data.size();

	u32 const fat_mem_size	= count*sizeof(binary_config_value);
	u32 out_ptr_size		= fat_mem_size+data_mem_size+2*sizeof(u32);
	*dest					= (pbyte)XRAY_MALLOC_IMPL(*allocator,out_ptr_size, "ltx_plain_data");
	pbyte out_ptr			= *dest;
	memory::copy			(out_ptr, out_ptr_size, &count, sizeof(u32)*1);
	out_ptr					+= sizeof(u32)*1;
	out_ptr_size			-= sizeof(u32)*1;

	memory::copy			(out_ptr, out_ptr_size,&data_mem_size, sizeof(u32)*1);
	out_ptr					+= sizeof(u32)*1;
	out_ptr_size			-= sizeof(u32)*1;

	u32 data_size			= data_mem_size;
	binary_config_value					stored;
	for(; it!=it_e; ++it)
	{
		binary_config_value* itm				= *it;
		stored					= *itm; //store pointers

		u32 sz_write			= 0;
		if(itm->id)
		{
			u32 const string_length	= strings::length(itm->id)+1;
			memory::copy			(data_ptr, data_size, (pvoid)(static_cast<char const*>(itm->id)), string_length);
			u32* v					= (u32*)&(itm->id);
			*v						= u32(data_ptr-plain_data_begin); //offset
			data_ptr				+= string_length;
			data_size				-= string_length;
		}

		if(itm->type > t_embedded_types)
		{
			sz_write			= itm->count;
			memory::copy		(data_ptr, data_size, itm->data, sz_write);
			u32* v				= (u32*)&(itm->data);
			*v					= u32(data_ptr-plain_data_begin); //offset
			data_ptr			+=sz_write;
			data_size			-= sz_write;
		}
		memory::copy			(out_ptr, out_ptr_size, itm, sizeof(binary_config_value));
		out_ptr					+= sizeof(binary_config_value);
		out_ptr_size			-= sizeof(binary_config_value);

		*itm					= stored; //restore pointers
	}

	u32 sz_written				= u32(data_ptr-plain_data_begin);
	XRAY_UNREFERENCED_PARAMETER	( sz_written );
	R_ASSERT					(sz_written==data_mem_size);

	memory::copy				(out_ptr, out_ptr_size, plain_data_begin, data_mem_size);
	out_ptr						+= data_mem_size;
//	out_ptr_size				-= data_mem_size;
	
	dest_size					= u32(out_ptr-data_ptr);
	XRAY_FREE_IMPL				(*allocator,plain_data_begin);
}

void xray::core::configs::parse_string_data(memory::reader F, memory::base_allocator* allocator, pbyte* dest, u32& dest_size)
{
	values_type				load_data(*allocator);
	binary_config_value* root				= XRAY_NEW_IMPL( *allocator, binary_config_value)();
	root->type				= t_table_named;
	parse_item_name			(allocator, *root, "root");
	load_data.push_back		(root);

	binary_config_value* Current			= NULL;
	string4096				str;
	string4096				str2;
	
	bool bInsideSTR			= false;

	while (!F.eof())
	{
		r_string			(F, str);
		_Trim				(str);
		if(strings::length(str)==0 || str[0]==';')
			continue;

		pstr comm			= strchr(str,';');
		pstr comm_1			= strchr(str,'/');
		
		if(comm_1 && (*(comm_1+1)=='/') && ((!comm) || (comm && (comm_1<comm) )) )
		{
			comm			= comm_1;
		}

		if (comm) 
		{
			char quot		= '"';
			bool in_quot	= false;
			
			pcstr q1		= strchr(str,quot);
			if(q1 && q1<comm)
			{
				pcstr q2	= strchr(++q1,quot);
				if(q2 && q2>comm)
					in_quot = true;
			}

			if(!in_quot)
				*comm		= 0;
		}
		if (str[0] && (str[0]=='[')) //new section ?
		{
			// start new section
			R_ASSERT(strchr(str,']'), "Bad ini section found: %s",str);
			*strchr(str,']') 	= 0;
			xray::strlwr			(str+1, sizeof(str)-1);

			Current					= XRAY_NEW_IMPL( *allocator, binary_config_value)();
			Current->type			= t_max;
			parse_item_name			(allocator, *Current, str+1);
			load_data.push_back		(Current);
			++root->count;

		} 
		else // id = value
		{
			if (Current)
			{
				string4096			value_raw;
				char*		id	= str;
				char*		t		= strchr(id,'=');
				if (t)		
				{
					*t				= 0;
					_Trim			(id);
					++t;
					strings::copy		(value_raw, sizeof(value_raw), t);
					bInsideSTR		= _parse(str2, value_raw);
					if(bInsideSTR)	//multiline str value
					{
						while(bInsideSTR)
						{
							strings::append	(value_raw, sizeof(value_raw),"\r\n");
							string4096		str_add_raw;
							r_string		(F, str_add_raw);
							R_ASSERT		(
								strings::length(value_raw) + strings::length(str_add_raw) < sizeof(value_raw),
									"Incorrect inifile format: section[%s], variable[%s]. Odd number of quotes (\") found, but should be even.",
									Current->id,
									id
							);
							strings::append	(value_raw, sizeof(value_raw),str_add_raw);
							bInsideSTR		= _parse(str2, value_raw);
                            if(bInsideSTR)
                            {
                            	if( is_empty_line_now(F) )
									strings::append		(value_raw, sizeof(value_raw),"\r\n");
                            }
						}
					}
				} else // no id
				{
					strings::copy(str2,str);
					id = NULL;
				}
				
				binary_config_value* value_itm		= XRAY_NEW_IMPL( *allocator, binary_config_value)();
				parse_item_name			(allocator, *value_itm, id);
				if ( !parse_item_value(allocator, *value_itm, str2) )
					continue;

				load_data.push_back		(value_itm);
				if(Current->count==0)
				{
					Current->type		= (id)? (u16)t_table_named : (u16)t_table_indexed;
				}else
					R_ASSERT			(Current->type==(id? t_table_named : t_table_indexed));

				++Current->count;
			}
		}
	}
	optimize		(allocator, load_data, root);
	save_bin		(allocator, dest, dest_size, load_data, root);

	values_type::iterator it = load_data.begin();
	values_type::iterator it_e = load_data.end();

	for(; it!=it_e; ++it)
	{
		binary_config_value* t = *it;
		if(t->id)
		{
			pvoid p = (pvoid)(static_cast<char const*>(t->id));
			XRAY_FREE_IMPL( *allocator, p);
		}
		if(t->type>t_embedded_types)
			XRAY_FREE_IMPL( *allocator, const_cast<pvoid&>((pcvoid&)(t->data)));
		
		XRAY_DELETE_IMPL( *allocator, t);
	}
}

xray::core::configs::binary_config* xray::core::configs::create_from_ltx_text(pcstr text, memory::base_allocator* allocator, pcstr fn)
{
	xray::memory::reader R		((u8 const*)text, xray::strings::length(text));
	pbyte						binary_buff = NULL;
	u32							binary_buff_size = 0;

	xray::core::configs::parse_string_data(R, allocator, &binary_buff, binary_buff_size);

	if ( fn )
	{
		using namespace		fs_new;
		synchronous_device_interface const &	device	=	core::get_core_synchronous_device( );
		file_type_pointer	f				(fn, device, file_mode::open_existing, file_access::read, assert_on_fail_false);
		
		if ( !f )
			LOG_INFO						("unable to open file [%s]", fn);
		else
			device->write					(f, binary_buff, binary_buff_size);
	}

	xray::core::configs::binary_config*	result	= XRAY_NEW_IMPL(*allocator, xray::core::configs::binary_config) (binary_buff, binary_buff_size, allocator);
	XRAY_FREE_IMPL	(*allocator, binary_buff);
	return			result;
}