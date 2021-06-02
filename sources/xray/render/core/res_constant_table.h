////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_CONST_TABLE_H_INCLUDED
#define RES_CONST_TABLE_H_INCLUDED

#include "res_common.h"
#include "constant.h"
#include "res_constant_buffer.h"
#include "constant_bindings.h"
#include "constant_defines.h"


namespace xray {
namespace render_dx10 {

//////////////////////////////////////////////////////////////////////////
// res_constant_table
class res_constant_table :	public res_flagged
{
	friend class resource_manager;
	template <enum_shader_type> friend class constants_handler;
//	friend template class constants_handler<enum_shader_type>;
	
public:
	typedef render::vector<ref_constant_buffer> c_buffers;
	typedef render::vector<constant>			c_table;

public:

	~res_constant_table() {}
	void _free() const;

	void clear();

	bool parse( ID3DShaderReflection* shader_reflection, enum_shader_type type);

// 	// deprecated
//  	void merge( res_constant_table* ctable);
//  	void merge( res_constant_table& ctable) {merge( &ctable);}

	void apply_bindings( constant_bindings const& bindings);
	
	constant const * get( LPCSTR const name) const;			// slow search ???
	constant const * get( shared_string& name) const;	// fast search ???

	bool equal( res_constant_table const& other) const;
	//bool equal( res_constant_table const* other) const {return equal( *other);}

	bool empty() const {return 0 == m_table.size();}

// 	c_buffers	const &  cbuffers() { return m_const_buffers; }
// 	c_table		const &  ctable()	{ return m_table; }

	bool parse_constant_buffer( ID3DShaderReflectionConstantBuffer* pTable, u32 buffer_index);


private:

	constant * get( LPCSTR const name);			// slow search ???
	
	// This need to be moved into an utility file/namespace
	inline void fatal( LPCSTR s);

protected:
	

protected:
	c_table					m_table;
	c_buffers				m_const_buffers;
};

typedef intrusive_ptr<res_constant_table, res_base, threading::single_threading_policy>			ref_constant_table;
typedef intrusive_ptr<res_constant_table const, res_base const, threading::single_threading_policy>	ref_constant_table_const;



} // namespace render 
} // namespace xray 


#endif // #ifndef RES_CONST_TABLE_H_INCLUDED
