////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_INPUT_LAYOUT_H_INCLUDED
#define RES_INPUT_LAYOUT_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>

namespace xray {
namespace render {

class res_declaration;

class res_signature;
typedef xray::intrusive_ptr<res_signature const, res_signature const, threading::single_threading_policy>	signature_const_ptr;

class res_input_layout : public resource_intrusive_base {
//	friend class resource_manager;
public:
	res_input_layout	( res_declaration const * decl, res_signature const * shader);
	~res_input_layout	();
	void destroy_impl	() const;

	bool equal( res_declaration const * decl, res_signature const * signature) const
	{
		return ( m_declaration == decl && m_signature == signature);
	}
	inline bool		is_registered	() const{ return m_is_registered; }
	inline void		mark_registered	()		{ R_ASSERT( !m_is_registered); m_is_registered = true; }
	
	ID3DInputLayout * hardware_layout() { return m_hw_input_layout;}

private:
	ID3DInputLayout *		m_hw_input_layout;
	res_declaration const *	m_declaration;
	signature_const_ptr		m_signature;
	bool					m_is_registered;
}; // class res_input_layout

typedef xray::intrusive_ptr<res_input_layout, resource_intrusive_base, threading::single_threading_policy>			input_layout_ptr;
typedef xray::intrusive_ptr<res_input_layout const, resource_intrusive_base const, threading::single_threading_policy>	ref_input_layout_const;


} // namespace render
} // namespace xray

#endif // #ifndef RES_INPUT_LAYOUT_H_INCLUDED