////////////////////////////////////////////////////////////////////////////
//	Created		: 28.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_SIGNATURE_H_INCLUDED
#define RES_SIGNATURE_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>

namespace xray {
namespace render {

//////////////////////////////////////////////////////////////////////////
// res_signature 
class res_signature : public resource_intrusive_base {
	friend class resource_intrusive_base;
	friend class resource_manager;

	res_signature	( ID3DBlob * singniture_blob): m_signature (singniture_blob), m_is_registered(false) { singniture_blob->AddRef();}
	~res_signature	();
	void destroy_impl() const;
public:

	ID3DBlob *			hw_signature() const { return m_signature;}

	bool equal ( ID3DBlob const * other)  { return other == m_signature; }
	inline bool		is_registered	() const{ return m_is_registered; }
	inline void		mark_registered	()		{ R_ASSERT( !m_is_registered); m_is_registered = true; }

private:
	ID3DBlob *			m_signature;	//signature;
	bool				m_is_registered;
};
typedef xray::intrusive_ptr<res_signature, res_signature, threading::single_threading_policy>				input_layout_element_signature_ptr;
typedef xray::intrusive_ptr<res_signature const, res_signature const, threading::single_threading_policy>	signature_const_ptr;

} // namespace render
} // namespace xray

#endif // #ifndef RES_SIGNATURE_H_INCLUDED