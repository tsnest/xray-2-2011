////////////////////////////////////////////////////////////////////////////
//	Created		: 14.04.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RES_TEXTURE_LIST_H_INCLUDED
#define RES_TEXTURE_LIST_H_INCLUDED

#include <xray/render/core/resource_intrusive_base.h>
#include <xray/render/core/texture_slot.h>

namespace xray {
namespace render {

class res_texture_list : public resource_intrusive_base {
//	friend class resource_manager;

public:
	res_texture_list	() : m_is_registered(false) {}
	res_texture_list	( texture_slots const & slots);
	~res_texture_list	() {}
	void destroy_impl	() const;

	bool equal( res_texture_list const & base) const;
	bool equal( texture_slots const & base) const;

	inline u32 size() const { return m_container.size(); }
	inline ref_texture const& operator [] ( u32 const index ) const { return m_container[index]; }
	inline ref_texture& operator [] ( u32 const index ) { return m_container[index]; }
	inline void resize( u32 const new_size, ref_texture const& value ) { m_container.resize(new_size, value); }

	typedef render::vector< ref_texture >	textures_type;
	typedef textures_type::const_iterator	const_iterator;
	typedef textures_type::iterator			iterator;
	inline const_iterator begin() const { return m_container.begin(); }
	inline const_iterator end() const { return m_container.end(); }
	inline iterator begin() { return m_container.begin(); }
	inline iterator end() { return m_container.end(); }

	inline bool	is_registered	() const{ return m_is_registered; }
	inline void	mark_registered	()		{ R_ASSERT( !m_is_registered); m_is_registered = true; }

	inline u32  id() const { return m_id; }
	inline void set_id( u32 const id ) { m_id = id; }

private:
	friend class res_effect; // For calculate the sizes of used textures.
	textures_type	m_container;
	u32				m_id;
	bool			m_is_registered;
};

typedef intrusive_ptr<res_texture_list, resource_intrusive_base, threading::single_threading_policy> ref_texture_list;
typedef intrusive_ptr<res_texture_list const, resource_intrusive_base const, threading::single_threading_policy> ref_texture_list_const;

} // namespace render
} // namespace xray

#endif // #ifndef RES_TEXTURE_LIST_H_INCLUDED
