////////////////////////////////////////////////////////////////////////////
//	Created 	: 24.02.2008
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef CONTACT_STORAGE_H_INCLUDED
#define CONTACT_STORAGE_H_INCLUDED

#include <xray/render/engine/base_classes.h>

namespace xray {
namespace render {

enum debug_user_id;

} // namespace render
} // namespace xray

class contact;
struct contact_joint_info;

struct contact_predicate
{
	virtual bool operator() (contact& c) = 0;

protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( contact_predicate )
}; // struct contact_predicate

class contact_storage
{
	vectora<contact*> m_contacts;
public:
				contact_storage		( );
	contact&	add					( const contact_joint_info& c );
	void		clear				( );
	void		for_each			( contact_predicate &predicate );
				~contact_storage	( )	{ clear( ); }
}; // class contact_storage


void render( contact_storage &contacts, render::scene_ptr const& scene, render::debug::renderer& renderer );

#endif // #ifndef CONTACT_STORAGE_H_INCLUDED