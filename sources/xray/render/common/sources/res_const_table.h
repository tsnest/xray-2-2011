////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_CONST_TABLE_H_INCLUDED
#	define RES_CONST_TABLE_H_INCLUDED

#	include "res_common.h"

namespace xray {
namespace render {


//#ifdef	USE_DX10
//#include "../xrRenderDX10/dx10ConstantBuffer.h"
//#endif	//	USE_DX10

enum
{
	rc_float		= 0,
	rc_int			= 1,
	rc_bool			= 2,
	rc_sampler		= 99,	//	DX9 shares index for sampler and texture
	rc_dx10texture	= 100	//	For DX10 sampler and texture are different resources
};

enum
{
	rc_1x1		= 0,					// vector1, or scalar
	rc_1x4,								// vector4
	rc_1x3,								// vector3
	rc_1x2,								// vector2
	rc_2x4,								// 4x2 matrix, transpose
	rc_3x4,								// 4x3 matrix, transpose
	rc_4x4,								// 4x4 matrix, transpose
	rc_1x4a,							// array: vector4
	rc_3x4a,							// array: 4x3 matrix, transpose
	rc_4x4a								// array: 4x4 matrix, transpose
};

enum
{
	//	Don't change this since some code relies on magic numbers
	rc_dest_pixel					= (1<<0),
	rc_dest_vertex					= (1<<1),
	rc_dest_sampler					= (1<<2),	//	For DX10 it's either sampler or texture
	rc_dest_geometry				= (1<<3),	//	DX10 only
	rc_dest_pixel_cb_index_mask		= 0xF000,	//	Buffer index == 0..14
	rc_dest_pixel_cb_index_shift	= 12,
	rc_dest_vertex_cb_index_mask	= 0x0F00,	//	Buffer index == 0..14
	rc_dest_vertex_cb_index_shift	= 8,
	rc_dest_geometry_cb_index_mask	= 0x00F0,	//	Buffer index == 0..14
	rc_dest_geometry_cb_index_shift	= 4,
};

enum	//	Constant buffer index masks
{
	cb_buffer_index_mask		= 0xF,	//	Buffer index == 0..14
	cb_buffer_type_mask			= 0x30,
	cb_buffer_pixel_shader		= 0x10,
	cb_buffer_vertex_shader		= 0x20,
	cb_buffer_geometry_shader	= 0x30
};

class constant_load
{
public:
	constant_load() :
		m_index(static_cast<u16>(-1)),
		m_cls(static_cast<u16>(-1))
	{};

	inline bool equal(constant_load& other) {return (m_index==other.m_index) && (m_cls == other.m_cls);}

public:
	u16	m_index;	// linear index (pixel)
	u16	m_cls;		// element class
};

// Automatic constant setup

class res_constant;
typedef fastdelegate::FastDelegate1<res_constant*>	const_setup_cb;

//!!! TODO : refactor this
class res_constant : public res_named
{
public:
	res_constant() : m_type(static_cast<u16>(-1)), m_destination(0), m_handler(NULL)
	{
	};

	~res_constant()
	{
	}

	inline bool equal(res_constant& other)
	{
		return (m_name == other.m_name) && (m_type == other.m_type)
			&& (m_destination == other.m_destination) &&
			m_ps.equal(other.m_ps) && m_vs.equal(other.m_vs) && m_samp.equal(other.m_samp) &&
			m_handler == other.m_handler;
	}

	inline bool equal(res_constant* other) {return equal(*other);}
	
	inline u16  get_type() {return m_type;}

public:
	//shared_string	m_name;		//name;		// HLSL-name
	u16				m_type;		//type;		// float=0/integer=1/boolean=2
	u16				m_destination;		//destination;// pixel/vertex/(or both)/sampler

	constant_load	m_ps;		//ps;
	constant_load	m_vs;		//vs;
#	ifdef	USE_DX10
	constant_load	m_gs;		//gs;
#	endif	//	USE_DX10
	constant_load	m_samp;		//samp;
	const_setup_cb	m_handler;	//handler;
};
typedef	intrusive_ptr<res_constant, res_base, threading::single_threading_policy> ref_constant;

class res_const_table :	public res_flagged
{
	friend class backend;

public:
	enum shader_type {st_vertex_shader = u16(0x01), st_pixel_shader = u16(0x02) };

public:
	~res_const_table();

	void clear();
	//BOOL parse(void* desc, u16 destination);
	bool parse(LPD3DXBUFFER shader_code, shader_type type);
	void merge(res_const_table* ctable);
	void merge(res_const_table& ctable) {merge(&ctable);}
	
	ref_constant get(LPCSTR	name);		// slow search
	ref_constant get(shared_string&	name);		// fast search

	bool equal(res_const_table& other);
	bool equal(res_const_table* other) {return equal(*other);}
	bool empty() {return 0 == m_table.size();}

#	ifdef	USE_DX10
	bool parse_constants(ID3D10ShaderReflectionConstantBuffer* pTable, u16 destination);
	bool parse_resources(ID3D10ShaderReflection* pReflection, int ResNum, u16 destination);
#	endif	//	USE_DX10

private:
	void fatal(LPCSTR s);

private:
	typedef render::vector<ref_constant> c_table;
	c_table		m_table;//table;

#	ifdef	USE_DX10
	typedef std::pair<u32,ref_cbuffer>	cb_table_record;
	typedef render::vector<cb_table_record>		cb_table;
	cb_table							m_cbuf_table;//m_CBTable;
#	endif	//	USE_DX10
};
typedef intrusive_ptr<res_const_table, res_base, threading::single_threading_policy>	ref_const_table;

//#ifdef	USE_DX10
//#include "../xrRenderDX10/dx10ConstantBuffer_impl.h"
//#endif	//	USE_DX10

class const_bindings
{
public:
	void add(shared_string name, const_setup_cb cb);
	void clear();
	void apply(res_const_table* table);

private:
	typedef std::pair<shared_string, const_setup_cb> binding;
	struct binding_compare;

private:
	render::vector<binding>	m_bindings;
};

} // namespace render 
} // namespace xray 


#endif // #ifndef RES_CONST_TABLE_H_INCLUDED
