////////////////////////////////////////////////////////////////////////////
//	Created		: 03.06.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_RESOURCE_PTR_H_INCLUDED
#define XRAY_RESOURCES_RESOURCE_PTR_H_INCLUDED

namespace xray {
namespace resources {

template < typename object_type, typename base_type >
class child_resource_ptr;

template < typename object_type, typename base_type >
class resource_ptr : public intrusive_ptr < object_type, base_type, threading::simple_lock >
{
public:
	typedef	child_resource_ptr< object_type, base_type >								child_type;

private:
	typedef resource_ptr  < object_type, base_type >									self_type;
	typedef intrusive_ptr < object_type, base_type, threading::simple_lock >	super;

public:
	inline			resource_ptr	() {}
					resource_ptr	(object_type * object);
					resource_ptr	(self_type const & object);
	
	self_type &		operator =		(object_type * object);
	self_type &		operator =		(self_type const & object);
}; // class resource_ptr

template < typename object_type, typename base_type >
class child_resource_ptr : public resource_ptr < object_type, base_type >
{
	typedef child_resource_ptr < object_type, base_type >		self_type;
	typedef resource_ptr < object_type, base_type >				super;

public:
	inline	child_resource_ptr	() : m_parent(NULL) {}
	inline	~child_resource_ptr	() { unlink_with_parent_if_needed(); }

	template < class parent_class >
	inline	void	initialize_and_set_parent	(parent_class *	const parent,
												 object_type *	const value)
			{
				if ( !value )
				{
					set_zero						();
					return;
				}

				unlink_with_parent_if_needed		();
				super::operator =					(value);
				m_parent						=	parent;
				if ( super::c_ptr() )
				{
					super::c_ptr()->link_parent_resource	(parent);
					parent->link_child_resource				(super::c_ptr());
				}
			}
			
			template < class parent_class >
			void	initialize_as_quality		(parent_class *	const parent,
												 object_type *	const value,
												 u32			const quality)
			{
				if ( !value )
				{
					set_zero						();
					return;
				}

				unlink_with_parent_if_needed		();
				super::operator =					(value);
				m_parent						=	parent;
				if ( super::c_ptr() )
				{
					super::c_ptr()->link_parent_resource	(parent, quality);
					parent->link_child_resource				(super::c_ptr(), quality);
				}
			}
				
			void	set_zero					()
			{
				unlink_with_parent_if_needed		();
				super::operator =					(NULL);
			}

private:
	inline	void	unlink_with_parent_if_needed()
	{
		if ( !m_parent )
			return;
		
		ASSERT									(* this);
		m_parent->unlink_child_resource			(super::c_ptr());
		super::c_ptr()->unlink_parent_resource	(m_parent);
		m_parent							=	NULL;
	}

	inline	child_resource_ptr	(child_resource_ptr const &) { NOT_IMPLEMENTED(); }

	void 		operator =		(object_type * ) {}
	void 		operator =		(super const & ) {}
	void 		operator =		(self_type const & ) {}

private:
	resource_base *				m_parent;

}; // class child_resource_ptr

} // namespace resources

template < typename dest_type, typename object_type, typename base_type >
dest_type		static_cast_resource_ptr	(resources::resource_ptr<object_type, base_type> const src_ptr);

} // namespace xray

#include <xray/resources_resource_ptr_inline.h>

#endif // #ifndef XRAY_RESOURCES_RESOURCE_PTR_H_INCLUDED
