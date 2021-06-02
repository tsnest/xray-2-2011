////////////////////////////////////////////////////////////////////////////
//	Created		: 28.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_PINNED_PTR_H_INCLUDED
#define XRAY_RESOURCES_PINNED_PTR_H_INCLUDED

namespace xray {
namespace resources { 

template <class T>
class pinned_ptr_base
{
protected:
	pinned_ptr_base	(managed_resource_ptr ptr) : m_resource(ptr), m_data(ptr ? ptr->pin() : 0), m_size(ptr ? ptr->get_size() : 0) { }
	pinned_ptr_base	(pinned_ptr_base const & other) : m_resource(other.m_resource), m_data(other.m_resource ? (pcbyte)other.m_resource->pin() : 0), m_size(other.m_resource ? other.m_resource->get_size() : 0) {}
public:
							~pinned_ptr_base() { if (m_resource) m_resource->unpin(m_data); }

	pinned_ptr_base<T>&	operator =		(pinned_ptr_base<T> const & other)
	{
		if ( m_resource )
			m_resource->unpin	( m_data );

		m_resource			= other.m_resource;
		m_data				= m_resource ? static_cast<pcbyte>( m_resource->pin() ) : 0;
		m_size				= m_resource ? other.m_size : 0;
		return				*this;
	}
	operator				const_buffer	() const { return const_buffer( m_data, size()); }
	u32						size			() const { return m_size; }
	T *						c_ptr			() const { return (T *)m_data; }
	T *						operator ->		() const { return (T *)m_data; }
	T &						operator *		() const { return *(T *)m_data; }
//	T &						operator []		(u32 i) const { R_ASSERT(i < m_resource->get_size()); return m_data[i]; }

	friend class managed_resource;

private:
	managed_resource_ptr	m_resource;
	pcbyte					m_data;
	u32						m_size;
};

template <class T>
class pinned_ptr_const : public pinned_ptr_base<T const>
{
public:
							pinned_ptr_const	(managed_resource_ptr ptr) : pinned_ptr_base<T const>(ptr) { }
							pinned_ptr_const	(pinned_ptr_const<T> const & other) : pinned_ptr_base<T const>(other) {}
	pinned_ptr_const<T>&	operator =			(pinned_ptr_const<T> const & other) { pinned_ptr_base<T const>::operator = ( other ); return *this; }
};

template <class T>
class pinned_ptr_mutable : public pinned_ptr_base<T>
{
public:
							pinned_ptr_mutable	(pinned_ptr_mutable const & other) : pinned_ptr_base<T>(other) {}
	operator				mutable_buffer		() { return mutable_buffer((pbyte)pinned_ptr_base<T>::c_ptr(), pinned_ptr_base<T>::size()); }

private:
							pinned_ptr_mutable	(managed_resource_ptr ptr) : pinned_ptr_base<T>(ptr) { }
private:
	friend class			cook_base;
	friend class			query_result;
};

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_PINNED_PTR_H_INCLUDED