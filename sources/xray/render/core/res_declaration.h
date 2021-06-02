////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RES_DECLARATION_H_INCLUDED
#define RES_DECLARATION_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>
#include <xray/render/core/res_input_layout.h>
#include <xray/render/core/res_signature.h>

namespace xray {
namespace render {

class res_declaration;

typedef xray::intrusive_ptr<res_signature const, res_signature const, threading::single_threading_policy>	signature_const_ptr;

struct signature_layout_pair
{ 
	signature_layout_pair( res_declaration const* decl, res_signature const * shader);

	bool operator < ( res_signature const * other ) const { return &*signature < other;}
	bool operator == ( res_signature const * other ) const { return &*signature == other;}

	input_layout_ptr	input_layout;
	signature_const_ptr	signature;
};


class res_declaration :	public resource_intrusive_base, private boost::noncopyable {
//	friend class resource_manager;
//	friend class resource_intrusive_base;
public:
	res_declaration		( D3D_INPUT_ELEMENT_DESC const* decl, u32 count);
	~res_declaration	();
	void destroy_impl	() const;
public:

	bool equal( D3D_INPUT_ELEMENT_DESC const* other, u32 count) const
	{
		if (count != dcl_code.size())
			return false;

		return memcmp(&dcl_code[0], other, count*sizeof(D3D_INPUT_ELEMENT_DESC)) == 0;
	}

	inline res_input_layout * get( res_signature const * signature);

	inline bool		is_registered	() const{ return m_is_registered; }
	inline void		mark_registered	()		{ R_ASSERT( !m_is_registered); m_is_registered = true; }

// --Porting to DX10_
//	u32 get_stride(u32 stream) {return D3DXGetDeclVertexSize(&*m_dcl_code.begin(), stream);}
// --Porting to DX10_
//	void apply() {backend::ref().set_decl( this);}

	render::vector<signature_layout_pair>			vs_to_layout;	
	render::vector<D3D_INPUT_ELEMENT_DESC> const	dcl_code;				
	bool											m_is_registered;
};

typedef xray::intrusive_ptr<res_declaration, resource_intrusive_base, threading::single_threading_policy>	ref_declaration;
typedef xray::intrusive_ptr<res_declaration const, resource_intrusive_base const, threading::single_threading_policy>	ref_declaration_const;


//////////////////////////////////////////////////////////////////////////
// inline implementations
res_input_layout * res_declaration::get( res_signature const * signature) 
{ 
	render::vector<signature_layout_pair>::iterator it = std::lower_bound( vs_to_layout.begin(), vs_to_layout.end(), signature);

	if( it != vs_to_layout.end() &&  *it == signature)
		return &*it->input_layout;

	return &*vs_to_layout.insert( it, signature_layout_pair( this, signature))->input_layout;
}


} // namespace render 
} // namespace xray 


#endif // #ifndef RES_DECLARATION_H_INCLUDED
