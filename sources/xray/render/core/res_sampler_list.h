////////////////////////////////////////////////////////////////////////////
//	Created		: 06.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef SAMPLER_LIST_H_INCLUDED
#define SAMPLER_LIST_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>
#include <xray/render/core/sampler_slot.h>

namespace xray {
namespace render {

class res_sampler_list : public resource_intrusive_base {
//	friend class resource_manager;
public:
	res_sampler_list	( sampler_slots const & slots);
	~res_sampler_list	() {}
	void destroy_impl	() const;

	bool equal(const res_sampler_list& base) const;
	bool equal( sampler_slots const & base) const;

	inline u32 size() const { return m_samplers.size(); }
	inline ref_sampler_state const& operator [] ( u32 const index ) const { return m_samplers[index]; }
	inline ref_sampler_state& operator [] ( u32 const index ) { return m_samplers[index]; }

	typedef render::vector< ref_sampler_state >	samplers_type;
	typedef samplers_type::const_iterator	const_iterator;
	typedef samplers_type::iterator			iterator;
	inline const_iterator begin() const { return m_samplers.begin(); }
	inline const_iterator end() const { return m_samplers.end(); }
	inline iterator begin() { return m_samplers.begin(); }
	inline iterator end() { return m_samplers.end(); }

	inline bool	is_registered	() const{ return m_is_registered; }
	inline void	mark_registered	()		{ R_ASSERT( !m_is_registered); m_is_registered = true; }

	inline u32  id() const { return m_id; }
	inline void set_id( u32 const id ) { m_id = id; }

private:
	samplers_type	m_samplers;
	u32				m_id;
	bool			m_is_registered;
};
typedef intrusive_ptr<res_sampler_list, resource_intrusive_base, threading::single_threading_policy> ref_sampler_list;
typedef intrusive_ptr<res_sampler_list const, resource_intrusive_base const, threading::single_threading_policy> ref_sampler_list_const;


} // namespace render
} // namespace xray

#endif // #ifndef SAMPLER_LIST_H_INCLUDED