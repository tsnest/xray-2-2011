#include "pch.h"

namespace xray {
namespace render {


//#pragma warning(disable:4995)
//#include <d3dx9.h>
//#pragma warning(default:4995)

//#include "ResourceManager.h"
//
//#include "../../xrCore/xrPool.h"
//#include "r_constants.h"
//
//#include "../xrRender/dxRenderDeviceRender.h"

// pool
//.static	poolSS<R_constant,512>			g_constant_allocator;

//R_constant_table::~R_constant_table	()	{	dxRenderDeviceRender::Instance().Resources->_DeleteConstantTable(this);	}


res_const_table::~res_const_table	()	
{
//	resource_manager::get_ref().delete_ctable(this);
}


void res_const_table::fatal(LPCSTR msg)
{
	XRAY_UNREFERENCED_PARAMETER	( msg );
	FATAL(msg);
}

// predicates
inline bool	p_search	(ref_constant c, LPCSTR str)
{
	return strcmp(c->get_name(), str) < 0;
}

inline bool p_sort(ref_constant c1, ref_constant c2)
{
	return strcmp(c1->get_name(), c2->m_name.c_str()) < 0;
}

ref_constant res_const_table::get(LPCSTR name)
{
	c_table::iterator it = std::lower_bound(m_table.begin(), m_table.end(), name, p_search);

	if (it == m_table.end() || (0 != strcmp((*it)->get_name(), name)))
		return 0;
	else
		return *it;
}
ref_constant res_const_table::get(shared_string& str)
{
	 //linear search, but only ptr-compare
	c_table::iterator	it  = m_table.begin(),
						end = m_table.end();

	for (; it != end; ++it)
	{
		ref_constant c = *it;
		if (c->m_name == str) return c;
	}

	return	0;
}

#ifndef	USE_DX10
//!!!TODO : refactor this
bool res_const_table::parse(LPD3DXBUFFER shader_code, res_const_table::shader_type dest)
{
	LPD3DXSHADER_CONSTANTTABLE	constants_table	= NULL;

	HRESULT hr = D3DXFindShaderComment((DWORD*)shader_code->GetBufferPointer(),
		MAKEFOURCC('C','T','A','B'), (LPCVOID*)&constants_table, NULL);

	if (FAILED(hr) || !constants_table)
	{
		LOG_ERROR("!D3DXFindShaderComment hr == %08x", (int)hr);
		return false;
	} 
	LPBYTE ptr = LPBYTE(constants_table);
	D3DXSHADER_CONSTANTINFO* it = (D3DXSHADER_CONSTANTINFO*)(ptr + constants_table->ConstantInfo);
	for (u32 count = constants_table->Constants; count; --count, ++it)
	{
		// Name
		LPCSTR	name		=	LPCSTR(ptr+it->Name);

		// Type
		u16 type = rc_float;
		if (D3DXRS_BOOL == it->RegisterSet)
			type = rc_bool;
		if (D3DXRS_INT4 == it->RegisterSet)
			type = rc_int;

		// Rindex,Rcount
		u16	r_index		=	it->RegisterIndex;
		u16	r_type		=	u16(-1);

		// TypeInfo + class
		D3DXSHADER_TYPEINFO* tinfo = (D3DXSHADER_TYPEINFO*)(ptr+it->TypeInfo);
		bool skip = false;
		switch (tinfo->Class)
		{
		case D3DXPC_SCALAR:	r_type = rc_1x1; break;
		case D3DXPC_VECTOR:	r_type = rc_1x4; break;
		case D3DXPC_MATRIX_ROWS:
			{
				switch (tinfo->Columns)
				{
				case 4:
					switch (tinfo->Rows)
					{
					case 3:
						switch (it->RegisterCount)
						{
						case 2:	r_type = rc_2x4; break;
						case 3: r_type = rc_3x4; break;
						default:
							LOG_ERROR("Invalid matrix dimension:%dx%d in constant %s", it->RegisterCount, tinfo->Columns, name);
							fatal("MATRIX_ROWS: unsupported number of RegisterCount");
							break;
						}
						break;

					case 4: r_type = rc_4x4; ASSERT(4 == it->RegisterCount); break;

					default:
						fatal("MATRIX_ROWS: unsupported number of Rows");
						break;
					}
					break;
				default:
					fatal("MATRIX_ROWS: unsupported number of Columns");
					break;
				}
			}
			break;
		case D3DXPC_MATRIX_COLUMNS:
			fatal("Pclass MATRIX_COLUMNS unsupported");
			break;
		case D3DXPC_STRUCT:
			fatal("Pclass D3DXPC_STRUCT unsupported");
			break;
		case D3DXPC_OBJECT:
			{
				switch (tinfo->Type)
				{
				case D3DXPT_SAMPLER:
				case D3DXPT_SAMPLER1D:
				case D3DXPT_SAMPLER2D:
				case D3DXPT_SAMPLER3D:
				case D3DXPT_SAMPLERCUBE:
					{
						// ***Register sampler***
						// We have determined all valuable info, search if constant already created
						ref_constant c = get(name);
						if (!c)
						{
							c =	NEW(res_constant);//.g_constant_allocator.create();
							c->m_name = name;
							c->m_destination = rc_dest_sampler;
							c->m_type = rc_sampler;
							constant_load& load	= c->m_samp;
							load.m_index = u16(r_index	+ ((dest == st_pixel_shader)? 0 : D3DVERTEXTEXTURESAMPLER0));
							load.m_cls   = rc_sampler;
							m_table.push_back(c);
						}
						else
						{
							R_ASSERT(c->m_destination == rc_dest_sampler);
							R_ASSERT(c->m_type == rc_sampler);
							constant_load& load	= c->m_samp;
							R_ASSERT(load.m_index == r_index);
							R_ASSERT(load.m_cls == rc_sampler);
							XRAY_UNREFERENCED_PARAMETER	( load );
						}
					}
					break;
				default:
					fatal("pclass D3DXPC_OBJECT - object isn't of 'sampler' type");
					break;
				}
			}
			skip = true;
			break;
		default:
			skip = true;
			break;
		}
		if (skip)
			continue;

		// We have determined all valuable info, search if constant already created
		ref_constant c = get(name);
		if (!c)
		{
			c = NEW(res_constant);//.g_constant_allocator.create();
			c->m_name = name;
			c->m_destination = (u16)dest;
			c->m_type = type;
			constant_load& load	= (dest == st_pixel_shader) ? c->m_ps : c->m_vs;
			load.m_index = r_index;
			load.m_cls   = r_type;
			m_table.push_back(c);
		}
		else
		{
			c->m_destination |=	dest;
			R_ASSERT(c->m_type == type);
			constant_load& load	= (dest == st_pixel_shader) ? c->m_ps : c->m_vs;
			load.m_index = r_index;
			load.m_cls   = r_type;
		}
	}
	std::sort(m_table.begin(), m_table.end(), p_sort);
	return		true;
}

//BOOL	res_const_table::parse	(void* _desc, u16 destination)
//{
	//D3DXSHADER_CONSTANTTABLE* desc	= (D3DXSHADER_CONSTANTTABLE*) _desc;
	//D3DXSHADER_CONSTANTINFO* it		= (D3DXSHADER_CONSTANTINFO*) (LPBYTE(desc)+desc->ConstantInfo);
	//LPBYTE					 ptr	= LPBYTE(desc);
	//for (u32 dwCount = desc->Constants; dwCount; dwCount--,it++)
	//{
	//	// Name
	//	LPCSTR	name		=	LPCSTR(ptr+it->Name);

	//	// Type
	//	u16		type		=	RC_float;
	//	if	(D3DXRS_BOOL == it->RegisterSet)	type	= RC_bool;
	//	if	(D3DXRS_INT4 == it->RegisterSet)	type	= RC_int;

	//	// Rindex,Rcount
	//	u16		r_index		=	it->RegisterIndex;
	//	u16		r_type		=	u16(-1);

	//	// TypeInfo + class
	//	D3DXSHADER_TYPEINFO*	T	= (D3DXSHADER_TYPEINFO*)(ptr+it->TypeInfo);
	//	BOOL bSkip					= FALSE;
	//	switch (T->Class)
	//	{
	//	case D3DXPC_SCALAR:			r_type		=	RC_1x1;		break;
	//	case D3DXPC_VECTOR:			r_type		=	RC_1x4;		break;
	//	case D3DXPC_MATRIX_ROWS:
	//		{
	//			switch (T->Columns)
	//			{
	//			case 4:
	//				switch (T->Rows)
	//				{
	//				case 3:
	//					switch (it->RegisterCount)
	//					{
	//					case 2:	r_type	=	RC_2x4;	break;
	//					case 3: r_type	=	RC_3x4;	break;
	//					default:
	//						Msg("Invalid matrix dimension:%dx%d in constant %s", it->RegisterCount, T->Columns, name);
	//						fatal		("MATRIX_ROWS: unsupported number of RegisterCount");
	//						break;
	//					}
	//					break;
	//				case 4:			r_type		=	RC_4x4;		VERIFY(4 == it->RegisterCount); break;
	//				default:
	//					fatal		("MATRIX_ROWS: unsupported number of Rows");
	//					break;
	//				}
	//				break;
	//			default:
	//				fatal		("MATRIX_ROWS: unsupported number of Columns");
	//				break;
	//			}
	//		}
	//		break;
	//	case D3DXPC_MATRIX_COLUMNS:
	//		fatal		("Pclass MATRIX_COLUMNS unsupported");
	//		break;
	//	case D3DXPC_STRUCT:
	//		fatal		("Pclass D3DXPC_STRUCT unsupported");
	//		break;
	//	case D3DXPC_OBJECT:
	//		{
	//			switch (T->Type)
	//			{
	//			case D3DXPT_SAMPLER:
	//			case D3DXPT_SAMPLER1D:
	//			case D3DXPT_SAMPLER2D:
	//			case D3DXPT_SAMPLER3D:
	//			case D3DXPT_SAMPLERCUBE:
	//				{
	//					// ***Register sampler***
	//					// We have determined all valuable info, search if constant already created
	//					ref_constant	C		=	get	(name);
	//					if (!C)	{
	//						C					=	xr_new<R_constant>();//.g_constant_allocator.create();
	//						C->name				=	name;
	//						C->destination		=	RC_dest_sampler;
	//						C->type				=	RC_sampler;
	//						R_constant_load& L	=	C->samp;
	//						L.index				=	u16(r_index	+ ( (destination&1)? 0 : D3DVERTEXTEXTURESAMPLER0 ));
	//						L.cls				=	RC_sampler	;
	//						table.push_back		(C);
	//					} else {
	//						R_ASSERT			(C->destination	==	RC_dest_sampler);
	//						R_ASSERT			(C->type		==	RC_sampler);
	//						R_constant_load& L	=	C->samp;
	//						R_ASSERT			(L.index		==	r_index);
	//						R_ASSERT			(L.cls			==	RC_sampler);
	//					}
	//				}
	//				break;
	//			default:
	//				fatal		("Pclass D3DXPC_OBJECT - object isn't of 'sampler' type");
	//				break;
	//			}
	//		}
	//		bSkip		= TRUE;
	//		break;
	//	default:
	//		bSkip		= TRUE;
	//		break;
	//	}
	//	if (bSkip)			continue;

	//	// We have determined all valuable info, search if constant already created
	//	ref_constant	C		=	get	(name);
	//	if (!C)	{
	//		C					=	xr_new<R_constant>();//.g_constant_allocator.create();
	//		C->name				=	name;
	//		C->destination		=	destination;
	//		C->type				=	type;
	//		R_constant_load& L	=	(destination&1)?C->ps:C->vs;
	//		L.index				=	r_index;
	//		L.cls				=	r_type;
	//		table.push_back		(C);
	//	} else {
	//		C->destination		|=	destination;
	//		VERIFY	(C->type	==	type);
	//		R_constant_load& L	=	(destination&1)?C->ps:C->vs;
	//		L.index				=	r_index;
	//		L.cls				=	r_type;
	//	}
	//}
	//std::sort	(table.begin(),table.end(),p_sort);
//	return		TRUE;
//}
#endif	//	USE_DX10

void res_const_table::merge(res_const_table* ctable)
{
	if (0 == ctable) return;

	// Real merge
	for (u32 it = 0; it < ctable->m_table.size(); ++it)
	{
		ref_constant src = ctable->m_table[it];
		ref_constant c   = get(src->get_name());
		if (!c)	
		{
			c					=	NEW(res_constant)(*src);//.g_constant_allocator.create();
			//c->m_name			=	src->m_name;
			//c->m_destination	=	src->m_destination;
			//c->m_type			=	src->m_type;
			//c->m_ps				=	src->m_ps;
			//c->m_vs				=	src->m_vs;
			//c->m_samp			=	src->m_samp;
			m_table.push_back(c);
		} 
		else 
		{
			ASSERT(!(c->m_destination & src->m_destination & rc_dest_sampler)
				&& "Can't have samplers or textures with the same name for PS, VS and GS.");
			c->m_destination |= src->m_destination;
			ASSERT(c->m_type == src->m_type);
			constant_load& sl	=	(src->m_destination&4)?src->m_samp:((src->m_destination&1)?src->m_ps:src->m_vs);
			constant_load& dl	=	(src->m_destination&4)?c->m_samp:((src->m_destination&1)?c->m_ps:c->m_vs);
			dl.m_index			=	sl.m_index;
			dl.m_cls			=	sl.m_cls;
		}
	}

	// Sort
	std::sort(m_table.begin(), m_table.end(), p_sort);

#ifdef	USE_DX10
	//	TODO:	DX10:	Implement merge with validity check
	m_CBTable.reserve( m_CBTable.size() + T->m_CBTable.size());
	for ( u32 i = 0; i < T->m_CBTable.size(); ++i )
		m_CBTable.push_back(T->m_CBTable[i]);
#endif	//	USE_DX10
}

void res_const_table::clear()                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
{
	m_table.clear();
//#ifdef	USE_DX10
//	m_CBTable.clear();
//#endif	//	
}

bool res_const_table::equal(res_const_table& other)
{
	if (m_table.size() != other.m_table.size())	return false;
	
	u32 size = m_table.size();
	for (u32 it=0; it<size; ++it)
	{
		if (!m_table[it]->equal(*other.m_table[it])) return false;
	}

	return true;
}

struct const_bindings::binding_compare
{
public:
	binding_compare(shared_string name): m_name(name) {}

	bool operator()(const binding& item)
	{return item.first == m_name;}

private:
	shared_string	m_name;
};

void const_bindings::add(shared_string name, const_setup_cb cb)
{
	vector<binding>::iterator	it  = std::find_if(m_bindings.begin(), m_bindings.end(), binding_compare(name));
	vector<binding>::iterator	end = m_bindings.end();

	if (it != end)
	{
		(*it).second = cb;
		return;
	}

	m_bindings.push_back(mk_pair(name, cb));
}

void const_bindings::clear()
{
	m_bindings.clear();
}

void const_bindings::apply(res_const_table* table)
{
	vector<binding>::iterator	it  = m_bindings.begin(),
								end = m_bindings.end();

	for (; it!=end; ++it)
	{
		ref_constant c = table->get(it->first);
		if (c && c->m_handler.empty())
			c->m_handler = it->second;
	}
}

} // namespace render 
} // namespace xray 
